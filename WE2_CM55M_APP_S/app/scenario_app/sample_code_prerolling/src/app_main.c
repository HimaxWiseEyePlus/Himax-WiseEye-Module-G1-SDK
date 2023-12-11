#include "app_platform.h"
#include "app_api.h"
#include "app_sys_info_cmd.h"
#include "app_datapath.h"
#include "app_board.h"
#include "app_xdma_cfg.h"
#include "app_pmu.h"
#include "app_preroll.h"

#include "hx_drv_pmu.h"
#include "hx_drv_scu_export.h"

#ifdef IP_rtc
#include "hx_drv_rtc.h"
#endif

#ifdef IP_timer
#include "hx_drv_timer.h"
#endif

#ifdef IP_watchdog
#include "hx_drv_watchdog.h"
#endif

#ifdef IP_gpio
#include "hx_drv_gpio.h"
#endif

#ifdef IP_hxautoi2c_mst
#include "hx_drv_hxautoi2c_mst.h"
#endif

#include "app_debug_print.h"
#include "preroll_info.h"

#define SW_VER "sample_code_vidpre_singlecode_s_only v0.1"

/* PIN MUX */
#if defined(HX6538_AIoT_EVB_QFN88_V10) || defined(HX6538_AIoT_EVB_LQFP128_V10)
#define WRITE_DATA_HAND_SHAKE   HX_GPIO_2       /*output*/
#define SOC_PWR_OFF_MSG         HX_AON_GPIO_0   /*input*/
#define SOC_READY               HX_AON_GPIO_0   /*input*/
#define PIR_NOTIFY_PIN          HX_AON_GPIO_0   /*input*/
#else
#define WRITE_DATA_HAND_SHAKE   HX_GPIO_0      /*output*/
#define SOC_PWR_OFF_MSG         HX_AON_GPIO_1  /*input*/
#define SOC_READY               HX_AON_GPIO_1  /*input*/
#define PIR_NOTIFY_PIN          HX_AON_GPIO_1  /*input*/
#endif

#define GPIO2GROUP(x) ((x >> 4) & 0x0F)
#define GPIO2INDEX(x) (x & 0x0F)

#define PREROLL_FPS         (5)
#define SENSOR_RTC_MS       (1000/PREROLL_FPS - 30)

//#define SYS_RTC_MS     (30*60*1000)

typedef struct{
    struct{
        HXGPIO no;
        uint8_t level;
    }gpio[HX_GPIO_MAX];
    uint8_t gpio_num;
    void (*ota_poll_func)(void);
} POLL_FUNC;

typedef struct{
    HXGPIO_CB_T int_func;
    uint32_t int_flag;
} GPIO_INT;


/* local variables */
static uint16_t data_types = TX_DATA_JPEG;
static GPIO_INT g_gpio_int[HX_GPIO_MAX] = {0};

#define PREROLL_SLOT_NUM    (10)

static volatile uint8_t g_wdma1_data[0] = {};
static volatile uint8_t g_wdma2_data[WDMA2_SLOT_SIZE*PREROLL_SLOT_NUM] = {};
static volatile uint8_t g_wdma3_data[WDMA3_SIZE] = {};
static volatile uint8_t g_jpeg_autofill_data[JPEG_AUTOFILL_SIZE] = {};

static void priv_i2c_cstm_cmd_handler(uint8_t cmd_id, uint8_t *cmd_payload, uint16_t cmd_payload_len);
static void priv_tx_frame(app_auto_preroll_status_t *ppreroll_status);
static void priv_get_preroll_info(PREROLL_INFO *ppreroll_info);
static void priv_send_preroll_info(PREROLL_INFO *ppreroll_info);
static void priv_preroll_active_proc();
#if defined(PIR_NOTIFY_PIN)
static void priv_pir_trig_mb_isr(uint8_t group, uint8_t idx);
#endif
#if defined(SOC_READY)
static void priv_soc_ready_int_isr(uint8_t group, uint8_t idx);
#endif
#if defined(SOC_PWR_OFF_MSG)
static void priv_soc_pwr_off_msg_int_isr(uint8_t group, uint8_t idx);
#endif

void clear_wakeup_evt()
{
	uint32_t wakeup_event, wakeup_event1;
	hx_lib_pm_clear_event();
	hx_drv_pmu_get_ctrl(PMU_pmu_wakeup_EVT, &wakeup_event);
    dbg_printf(DBG_LESS_INFO, "after clear WakeupEvt=0x%x\n", wakeup_event);
	hx_drv_pmu_get_ctrl(PMU_pmu_wakeup_EVT1, &wakeup_event1);
    dbg_printf(DBG_LESS_INFO, "after clear WakeupEvt1=0x%x\n", wakeup_event1);
}

void clear_all_sysip_int()
{
#ifdef IP_rtc
	hx_drv_rtc_clear_alarm_int_status(RTC_ID_0);
	hx_drv_rtc_clear_alarm_int_status(RTC_ID_1);
	hx_drv_rtc_clear_alarm_int_status(RTC_ID_2);
#endif
#ifdef IP_timer
	hx_drv_timer_ClearIRQ(TIMER_ID_0);
	hx_drv_timer_ClearIRQ(TIMER_ID_1);
	hx_drv_timer_ClearIRQ(TIMER_ID_2);
	hx_drv_timer_ClearIRQ(TIMER_ID_3);
#ifdef WE2_SINGLE_CORE
	hx_drv_timer_ClearIRQ(TIMER_ID_4);
	hx_drv_timer_ClearIRQ(TIMER_ID_5);
	hx_drv_timer_ClearIRQ(TIMER_ID_6);
	hx_drv_timer_ClearIRQ(TIMER_ID_7);
	hx_drv_timer_ClearIRQ(TIMER_ID_8);
#endif
#endif
#ifdef IP_watchdog
	hx_drv_watchdog_irq_clear(WATCHDOG_ID_0);
	hx_drv_watchdog_irq_clear(WATCHDOG_ID_1);
#endif
#ifdef IP_gpio
	hx_drv_gpio_clr_int_status(AON_GPIO0);
	hx_drv_gpio_clr_int_status(AON_GPIO1);
	hx_drv_gpio_clr_int_status(SB_GPIO0);
	hx_drv_gpio_clr_int_status(SB_GPIO1);
#endif
#ifdef IP_hxautoi2c_mst
	hx_drv_hxautoi2c_clr_noack_int(HXAUTOI2CHC_INTSTATUS_DATA_NO_ACK);
	hx_drv_hxautoi2c_clr_noack_int(HXAUTOI2CHC_INTSTATUS_ADDR_NO_ACK);
#endif
}

void app_init()
{
    app_dp_cfg_t app_dp_init_cfg = {0};
    APP_SPI_CFG_T spi_cfg = {0};

    app_board_pinmux_settings();

#ifdef WAKEUP_SOC
    hx_drv_scu_set_PB10_pinmux(SCU_PB10_PINMUX_GPIO1);
    hx_drv_gpio_set_output(GPIO1, GPIO_OUT_LOW);
#endif

#ifdef SPI_MASTER_SEND /*master*/
    spi_cfg.mst_id = SPI_MST_1; /**< id*/
    spi_cfg.mst_freq = 5000000;  /**< frequency*/
#endif
    
#ifdef SPI_SLAVE_SEND /*slave*/
    #if defined(HX6538_AIoT_EVB_QFN88_V10) || defined(HX6538_AIoT_EVB_LQFP128_V10)
    spi_cfg.slv_handshake_pin = WRITE_DATA_HAND_SHAKE;         /**< handshake gpio pin number*/
    #else
    spi_cfg.slv_handshake_pin = WRITE_DATA_HAND_SHAKE;         /**< handshake gpio pin number*/
    #endif
    dbg_printf(DBG_LESS_INFO, "HX_GPIO_%02x\n", spi_cfg.slv_handshake_pin);
    spi_cfg.slv_handshake_actv_lvl = HX_GPIO_HIGH;     /**< handshake gpio pin active level*/

    dbg_printf(DBG_LESS_INFO, "spi_cfg.slv_handshake_pin %02x\n", spi_cfg.slv_handshake_pin);
    dbg_printf(DBG_LESS_INFO, "spi_cfg.slv_handshake_actv_lvl %u\n", spi_cfg.slv_handshake_actv_lvl);
#endif

    app_i2c_cmd_init(USE_DW_IIC_SLV_0);
    app_spi_init(&spi_cfg);

    app_dp_get_def_init_cfg(&app_dp_init_cfg);
    app_dp_init_cfg.wdma1 = (uint32_t)g_wdma1_data;
    app_dp_init_cfg.wdma2 = (uint32_t)g_wdma2_data;
    app_dp_init_cfg.wdma3 = (uint32_t)g_wdma3_data;
    app_dp_init_cfg.jpeg_auto_fill_data = (uint32_t)g_jpeg_autofill_data;
    app_dp_init_cfg.wdma2_cyclic_buffer_cnt = (uint8_t)(PREROLL_SLOT_NUM);
    app_dp_init(&app_dp_init_cfg);
}

void app_main()
{
    uint32_t wakeup_event = 0;
    uint32_t wakeup_event1 = 0;

    uint8_t jpegloop_flag = 0;
	uint8_t nextjpeg_frame_no = 0;
	uint8_t jpeg_total_slot = 0;
    uint8_t first_frame_cap = 0;

    AppCfgCustGpio_t gpio_cfg = {0};
    uint32_t sys_rtc_id = 0;

#ifdef WE2_SINGLE_CORE
	dbg_printf(DBG_LESS_INFO, "CM55M sample_code_app_vidpre_singlecore_s_only\n");
#else
	dbg_printf(DBG_LESS_INFO, "CM55M sample_code_app_vidpre_dualcore_s_only\n");
#endif

    hx_drv_pmu_get_ctrl(PMU_pmu_wakeup_EVT, &wakeup_event);
    dbg_printf(DBG_LESS_INFO, "WakeupEvt=0x%x\n", wakeup_event);

	hx_drv_pmu_get_ctrl(PMU_pmu_wakeup_EVT1, &wakeup_event1);
    dbg_printf(DBG_LESS_INFO, "WakeupEvt1=0x%x\n", wakeup_event1);
    print_wakeup_event(wakeup_event, wakeup_event1);
#ifdef WE2_DUAL_CORE
	dbg_printf(DBG_LESS_INFO, "CM55M_ENABLE_CM55S \n\n\n");
    hx_drv_scu_set_cm55s_state(SCU_CM55S_STATE_RESET);
    hx_drv_scu_set_cm55s_state(SCU_CM55S_STATE_NORMAL);
    hx_drv_scu_set_CM55S_CPUWAIT(SCU_CM55_CPU_RUN);
    
    dbg_printf(DBG_LESS_INFO, "app_main start(dual core).\n");
#endif

    app_debug_print();

	hx_drv_timer_sensor_stop(); /*TIMER_ID_0*/
#ifdef SYS_RTC_MS
    hx_drv_timer_hw_stop(APP_PMU_SYS_RTC_ID); /*TIMER_ID_1*/
#endif
	
	hx_drv_timer_hw_stop(TIMER_ID_2); /*TIMER_ID_2*/
	hx_drv_timer_hw_stop(TIMER_ID_3); /*TIMER_ID_3*/
    
    hx_lib_pm_pmu_dpdone_check(NULL);

    hx_lib_pm_ctrl_fromPMUtoCPU(NULL);

     /* Setup GPIO Interrupts. */
#if defined(SOC_PWR_OFF_MSG)
    g_gpio_int[SOC_PWR_OFF_MSG].int_func = priv_soc_pwr_off_msg_int_isr;
    g_gpio_int[SOC_PWR_OFF_MSG].int_flag = 0;
#endif
    
#if defined(SOC_READY)
    g_gpio_int[SOC_READY].int_func = priv_soc_ready_int_isr;
    g_gpio_int[SOC_READY].int_flag = 0;
#endif
    
#if defined(PIR_NOTIFY_PIN)
    g_gpio_int[PIR_NOTIFY_PIN].int_func = priv_pir_trig_mb_isr;
    g_gpio_int[PIR_NOTIFY_PIN].int_flag = 0;
#endif

#if defined(PIR_NOTIFY_PIN)
    app_gpio_intr_register(PIR_NOTIFY_PIN, priv_pir_trig_mb_isr, HX_GPIO_IRQ_TRIG_TYPE_EDGE_RISING);
#endif
    
    dbg_printf(DBG_LESS_INFO, "app_main starting...\n");

    /* Init Preroll. */
    app_auto_preroll_config_t preroll_config = {0};
    
    preroll_config.wdma1_data = (uint32_t)g_wdma1_data;
    preroll_config.wdma2_data = (uint32_t)g_wdma2_data;
    preroll_config.wdma3_data = (uint32_t)g_wdma3_data;
    preroll_config.jpeg_auto_fill_data = (uint32_t)g_jpeg_autofill_data;
    preroll_config.slot_size = (uint32_t)(WDMA2_SLOT_SIZE);
    preroll_config.slot_num = (uint8_t)(PREROLL_SLOT_NUM);
    app_auto_preroll_init(&preroll_config);

    app_init();

    if((wakeup_event == PMU_WAKEUP_NONE) && (wakeup_event1 == PMU_WAKEUPEVENT1_NONE))
    {

    }
    else
    {
        app_auto_preroll_status_t preroll_status = {0};
        app_auto_preroll_get_status(&preroll_status);
        if(preroll_status.first_frame_cap != 0) /*cpatured one frame at least*/
        {
            /*wakeup SoC and send the preroll images.*/
            priv_preroll_active_proc();
        }
    }

    clear_wakeup_evt();
    clear_all_sysip_int();
    app_auto_preroll_set_datapath();

    gpio_cfg.act_wakeupCPU_pin_cnt = 0;
#if defined(PIR_NOTIFY_PIN)
    gpio_cfg.wakeupCPU_int_pin[gpio_cfg.act_wakeupCPU_pin_cnt] = PIR_NOTIFY_PIN;
    gpio_cfg.act_wakeupCPU_pin_cnt += 1;
#endif

#ifdef SYS_RTC_MS
    app_pmu_enter_sleep1_aos(gpio_cfg, SENSOR_RTC_MS, SYS_RTC_MS, 1);
#else
    app_pmu_enter_sleep1_aos(gpio_cfg, SENSOR_RTC_MS, 0, 1);
#endif
}

int8_t app_sys_info_get_formal_version(INFO_FORMAL_VERSION *formal_version)
{
    return 0;
}

int8_t app_sys_info_get_tx_protocol(INFO_TX_PROTOCOL *tx_protocol)
{
    tx_protocol->intf_type = TX_IF_SPI_M;
    tx_protocol->speed = 10000000;
    tx_protocol->data_types = data_types;
    return 0;
}

int8_t app_sys_info_get_raw_setting(INFO_RAW_SETTING *raw_setting)
{
    raw_setting->format = RAW_YUV420;
    raw_setting->width = 640;
    raw_setting->height = 480;
    return 0;
}

typedef enum
{
    CSTM_CMD_1 = 0x00,
    CSTM_CMD_2 = 0x01,
    CSTM_CMD_3 = 0x02,
} CSTM_I2C_CMDS;

/*
* i2c data[0] = I2C_CMD_CSTM_FEATURE_1 (0x81)
* i2c data[1] = CSTM_I2C_CMD (0x00 = CSTM_CMD_1, 0x01 = CSTM_CMD_2, ...)
* i2c data[2] = length of command payload (LSB)
* i2c data[3] = length of command payload (MSB)
* i2c data[4 ~ n] = payload (only present when payload length > 0)
* e.g. IR Cut enable = [0x81, 0x00, 0x01, 0x00, 0x01]
*/
static void priv_i2c_cstm_cmd_handler(uint8_t cmd_id, uint8_t *cmd_payload, uint16_t cmd_payload_len)
{
    dbg_printf(DBG_LESS_INFO, "priv_i2c_cstm_cmd_handler executed. (cmd_id = 0x%02x, cmd_payload_len = %d)\n", cmd_id, cmd_payload_len);

    switch (cmd_id)
    {
        case CSTM_CMD_1:
        {
            break;
        }
        case CSTM_CMD_2:
        {
            break;
        }
        case CSTM_CMD_3:
        {
            break;
        }
        break;
    }
}

static void priv_tx_frame(app_auto_preroll_status_t *ppreroll_status)
{
    uint32_t data;
    uint32_t size;
    
    if(ppreroll_status->first_frame_cap)
    {
        uint8_t cnt;
        
        if(ppreroll_status->loop_flag)
        {
            cnt = ppreroll_status->total_slot;
        }
        else
        {
            if(ppreroll_status->next_frame_no >= ppreroll_status->start_frame_no)
                cnt = ppreroll_status->next_frame_no - ppreroll_status->start_frame_no;
            else
                cnt = (ppreroll_status->total_slot + ppreroll_status->next_frame_no) - ppreroll_status->start_frame_no;
        }

        for(uint8_t i = 0; i < cnt; i++)
        {
            uint8_t j;

            j = (ppreroll_status->start_frame_no + i) % ppreroll_status->total_slot;

            hx_drv_jpeg_get_FillFileSizeToMem(j, (uint32_t)ppreroll_status->preroll_data.jpeg_auto_fill_data, &size);
            hx_drv_jpeg_get_MemAddrByFrameNo(j, (uint32_t)ppreroll_status->preroll_data.wdma2_data, &data);

            if(data != 0 && size != 0)
            {
                app_spi_write((uint32_t)data, (uint32_t)size, DATA_TYPE_JPG);
            }
        }
    }
}

static void priv_get_preroll_info(PREROLL_INFO *ppreroll_info)
{
    app_auto_preroll_status_t preroll_status = {0};
	
    app_auto_preroll_get_status(&preroll_status);

    if(preroll_status.loop_flag)
    {
        ppreroll_info->images_info.frame_num = preroll_status.total_slot;
    }
    else
    {
        if(preroll_status.next_frame_no >= preroll_status.start_frame_no)
        {
            ppreroll_info->images_info.frame_num = preroll_status.next_frame_no - preroll_status.start_frame_no;
        }
        else
        {
            ppreroll_info->images_info.frame_num = (preroll_status.total_slot + preroll_status.next_frame_no) - preroll_status.start_frame_no;
        }
    }

    ppreroll_info->images_info.width = DP_JPEG_ENC_WIDTH;
    ppreroll_info->images_info.height = DP_JPEG_ENC_HEIGHT;
    ppreroll_info->images_info.compress_rate = DP_JPEG_ENCQTABLE;
}

static void priv_send_preroll_info(PREROLL_INFO *ppreroll_info)
{
    app_spi_write((uint32_t)ppreroll_info, sizeof(PREROLL_INFO), DATA_TYPE_PREROLL_INFO);
}

#if defined(PIR_NOTIFY_PIN)
static void priv_pir_trig_mb_isr(uint8_t group, uint8_t idx)
{
    dbg_printf(DBG_LESS_INFO, "\033[1;33m[pir Triggered]\033[0m\n");
    g_gpio_int[PIR_NOTIFY_PIN].int_flag = 1;
    hx_drv_gpio_clr_int_status(PIR_NOTIFY_PIN);
}
#endif

#if defined(SOC_READY)
static void priv_soc_ready_int_isr(uint8_t group, uint8_t idx)
{
    dbg_printf(DBG_LESS_INFO, "\033[1;33m[soc ready Triggered]\033[0m\n");
    g_gpio_int[SOC_READY].int_flag = 1;
    hx_drv_gpio_clr_int_status(SOC_READY);
}
#endif

#if defined(SOC_PWR_OFF_MSG)
static void priv_soc_pwr_off_msg_int_isr(uint8_t group, uint8_t idx)
{
    dbg_printf(DBG_LESS_INFO, "\033[1;33m[soc power off Triggered]\033[0m\n");
    g_gpio_int[SOC_PWR_OFF_MSG].int_flag = 1;
    hx_drv_gpio_clr_int_status(SOC_PWR_OFF_MSG);
}
#endif

static void priv_wait_soc_gpio_resp(POLL_FUNC *poll)
{
    uint8_t bit_mask[6] = {0};
    uint8_t reg_val[6] = {0};
    uint8_t int_val[6] = {0};
    uint8_t val_expt[6] = {0};
    uint8_t i = 0;
    uint8_t val = 0;
    uint8_t match = 0;
    
    dbg_printf(DBG_LESS_INFO, "wait gpio ");
    for(i = 0; i < poll->gpio_num; i++)
    {
        dbg_printf(DBG_LESS_INFO, "gpio[%x] ", poll->gpio[i].no);
        
        bit_mask[GPIO2GROUP(poll->gpio[i].no)] |= (0x1 << GPIO2INDEX(poll->gpio[i].no));
        val_expt[GPIO2GROUP(poll->gpio[i].no)] |= (poll->gpio[i].level << GPIO2INDEX(poll->gpio[i].no));
    }
    dbg_printf(DBG_LESS_INFO, "\n");
    for(i = 0; i < poll->gpio_num; i++)
    {
        g_gpio_int[poll->gpio[i].no].int_flag = 0;
        app_gpio_intr_register(poll->gpio[i].no, g_gpio_int[poll->gpio[i].no].int_func, (poll->gpio[i].level == 0)?GPIO_IRQ_TRIG_TYPE_EDGE_FALLING:GPIO_IRQ_TRIG_TYPE_EDGE_RISING);
    }

    while(1)
    {
        if(poll->ota_poll_func != NULL)
            poll->ota_poll_func();

        for(i = 0; i < poll->gpio_num; i++)
        {
            val = 0;
            
            hx_drv_gpio_get_in_value(poll->gpio[i].no, &val);
            reg_val[GPIO2GROUP(poll->gpio[i].no)] |= ((val & 0x1) << GPIO2INDEX(poll->gpio[i].no));

            if(g_gpio_int[poll->gpio[i].no].int_flag == 1)
                int_val[GPIO2GROUP(poll->gpio[i].no)] |= poll->gpio[i].level << GPIO2INDEX(poll->gpio[i].no);
            else
                int_val[GPIO2GROUP(poll->gpio[i].no)] |= (!poll->gpio[i].level) << GPIO2INDEX(poll->gpio[i].no);
        }

        match = 1;
        for(i = 0; i < 6; i++)
        {
            if(bit_mask[i])
            {
                if(((int_val[i] | reg_val[i]) & bit_mask[i]) == val_expt[i]) /*register + interrupt bit*/
                {
                    //dbg_printf(DBG_LESS_INFO, "\033[1;33mreg[%u]+int_val[%u] = 0x%08x\033[0m\n", i, i, ((int_val[i] | reg_val[i]) & bit_mask[i]));
                }
                else
                {
                    //dbg_printf(DBG_LESS_INFO, "\033[1;33mreg[%u]+int_val[%u] = 0x%08x mismatch\033[0m\n", i, i, ((int_val[i] | reg_val[i]) & bit_mask[i]));
                    match = 0;
                    break;
                }
            }
        }
        
        if(match == 1)
            break;

        board_delay_ms(1);
    }
    
    for(i = 0; i < poll->gpio_num; i++)
    {
        g_gpio_int[poll->gpio[i].no].int_flag = 0;
        app_gpio_intr_unregister(poll->gpio[i].no);
    }
}

static void priv_i2c_polling()
{
    app_i2c_cmd_polling(USE_DW_IIC_SLV_0);
}

static void priv_preroll_active_proc()
{
    app_auto_preroll_status_t preroll_status = {0};
    PREROLL_INFO preroll_info = {0};
    POLL_FUNC poll = {0};
    
    /* Toggle GPIO to wakeup the SoC. */
#ifdef WAKEUP_SOC
    app_gpio_set_value(WAKEUP_SOC, 1);
#endif
    
    /* Waiting SoC to toggle the SOC_READY pin. */
    poll.gpio_num = 0;
#ifdef SOC_READY
    poll.gpio[poll.gpio_num].no = SOC_READY;
    poll.gpio[poll.gpio_num].level = 1;
    poll.gpio_num += 1;
#endif
    poll.ota_poll_func = priv_i2c_polling;
    priv_wait_soc_gpio_resp(&poll);

    /* Got the SOC_READY signal, start to send preroll info & frames. */
    priv_get_preroll_info(&preroll_info);
    priv_send_preroll_info(&preroll_info);
    app_auto_preroll_get_status(&preroll_status);
    priv_tx_frame(&preroll_status);

    /* Clean the preroll status since all the frames has been sent out. */
    app_auto_preroll_reset_status();
    //hx_drv_pmu_set_ctrl(PMU_SEN_INIT, 1); /*sensor init is required after pmu wakeup*/

    /* Waiting SoC to finish its tasks(e.g. video recording or sending event through WiFi) */
    poll.gpio_num = 0;
#ifdef SOC_PWR_OFF_MSG
    poll.gpio[poll.gpio_num].no = SOC_PWR_OFF_MSG;
    poll.gpio[poll.gpio_num].level = 1;
    poll.gpio_num += 1;
#endif
    poll.ota_poll_func = priv_i2c_polling;
    priv_wait_soc_gpio_resp(&poll);

    /* pull down the WAKEUP_SOC signal since the SoC had complete its tasks. */
#ifdef WAKEUP_SOC
    app_gpio_set_value(WAKEUP_SOC, 0);
#endif
}

