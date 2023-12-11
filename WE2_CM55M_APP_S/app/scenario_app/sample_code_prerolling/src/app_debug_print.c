#include "WE2_core.h"
#include "WE2_device_addr.h"
#include "WE2_debug.h"

#include "hx_drv_scu.h"
#include "hx_drv_swreg_aon.h"

#include "hx_drv_xdma.h"
#include "hx_drv_gpio.h"
#include "hx_drv_pmu_export.h"
#include "hx_drv_pmu.h"

#include "xprintf.h"

#define MAX_STRING  100

#define dbg_app_log(fmt, ...)       xprintf(fmt, ##__VA_ARGS__)

static char wakeup_event[][MAX_STRING]={
	{"[0]: PS_DPD : wakeup by (ext_force_pmu_dc_sync)"},
	{"[1]: PS_DPD wakeup by (RTC_timer_int)"},
	{"[2]: PS_DPD wakeup by (anti_tamp_int)"},
	{"[3]: PS_PD wakeup by ((REG_FORCE_EVENT_NSTATE==5'b10000) && REG_FORCE_EVENT_TRIG)"},
	{"[4]: PS_PD wakeup by (ext_int_nxt_dc)"},
	{"[5]: PS_PD wakeup by (RTC_timer_int)"},
	{"[6]: PS_PD wakeup by (|SB_timer_int[5:1](VerA))(|SB_timer_int[8:1](VerB))"},
	{"[7]: PS_PD wakeup by (CMP_int)"},
	{"[8]: PS_PD wakeup by (TS_int)"},
	{"[9]: PS_PD wakeup by (I2C_W_int)"},
	{"[10]: PS_PD wakeup by (REG_CPU_SCENARIO == 4'b0010 && SB_timer_int[0])"},
	{"[11]: PS_SCL wakeup by ((REG_FORCE_EVENT_NSTATE==5'b10000) && REG_FORCE_EVENT_TRIG)"},
	{"[12]: PS_SCL wakeup by (~pmu_sc_active && Sensor_EXT_INT)"},
	{"[13]: PS_SCL wakeup by (ext_int_nxt_dc)"},
	{"[14]: PS_SCL wakeup by (RTC_timer_int)"},
	{"[15]: PS_SCL wakeup by (SB_timer_int[5:1](VerA))(|SB_timer_int[8:1](VerB))"},
	{"[16]: PS_SCL wakeup by (ADCC_int)"},
	{"[17]: PS_SCL wakeup by (TS_int)"},
	{"[18]: PS_SCL wakeup by (I2C_W_int)"},
	{"[19]: PS_SCL wakeup by (pmu_wdg_timeout_int)"},
	{"[20]: PS_SCH wakeup by ((REG_FORCE_EVENT_NSTATE==5'b10000) && REG_FORCE_EVENT_TRIG)"},
	{"[21]: PS_SCH wakeup by (ext_int_nxt_dc)"},
	{"[22]: PS_SCH wakeup by (RTC_timer_int)"},
	{"[23]: PS_SCH wakeup by (|SB_timer_int[5:1](VerA))(|SB_timer_int[8:1](VerB))"},
	{"[24]: PS_SCH wakeup by (ADCC_int)"},
	{"[25]: PS_SCH wakeup by (TS_int)"},
	{"[26]: PS_SCH wakeup by (I2C_W_int)"},
	{"[27]: PS_SCH wakeup by (pmu_wdg_timeout_int)"},
	{"[28]: PS_SCH wakeup by (WRDMAx_abnormal_int)"},
	{"[29]: PS_SCH wakeup by (DP_ABNORMAL_INT)"},
	{"[30]: PS_SCH wakeup VID Only by (DP_CDM_MOTION_INT)"},
	{"[31]: PS_SCH wakeup VIDAUD by (DP_CDM_MOTION_INT)"},
};

static char wakeup_event1[][MAX_STRING]={
	{"[0]: PS_DPD wakeup by (~pmu_sc_active &&PAD_AON_GPIO_0)"},
	{"[1]: PS_DPD wakeup by (~pmu_sc_active &&PAD_VMUTE)"},
	{"[2]: PS_DPD wakeup by (~pmu_sc_active && ext_int_nxt_dc)"},
	{"[8]: PS_PD wakeup by (anti_tamp_int)"},
	{"[16]: PS_SCL wakeup by (anti_tamp_int)"},
	{"[24]: PS_SCH wakeup by (SC_sen_noack)"},
	{"[25]: PS_SCH wakeup by (anti_tamp_int)"}
};

static char hwclk_case[][MAX_STRING]={
	{"0:RC24MRC32K"},
	{"1:RC24MRC48MRC32K"},
	{"2:RC24MRC96MRC32K"},
	{"3:XTAL24MXTAL32K"},
	{"4:XTAL24MRC48MXTAL32K"},
	{"5:XTAL24MRC96MXTAL32K"},
	{"6:XTAL24MRC32K"},
	{"7:XTAL24MRC48MRC32K"},
	{"8:XTAL24MRC96MRC32K"},
	{"9:RC24M_XTAL32K"},
	{"10:RC24MRC48M_XTAL32K"},
	{"11:RC24MRC96M_XTAL32K"},
};

static char pll_case[][MAX_STRING]={
	{"0:disable PLL"},
	{"1:Enable PLL"},
};

static char div_case[][MAX_STRING]={
	{"0:div1"},
	{"1:div2"},
	{"2:div3"},
	{"3:div4"},
	{"4:div5"},
	{"5:div6"},
	{"6:div7"},
	{"7:div8"},
	{"8:div9"},
	{"9:div10"},
	{"10:div11"},
	{"11:div12"},
	{"12:div13"},
	{"13:div14"},
	{"14:div15"},
	{"15:div16"},
};

static char mclk_case[][MAX_STRING]={
	{"0:12M"},
	{"1:24M"},
	{"2:48M"},
	{"3:96M"}
};

static char hscclksrc[][MAX_STRING]={
	{"0:RC24M1M"},
	{"1:XTAL24M"},
	{"2:RC96M48M"},
	{"3:PLL"}
};

static char hscd12clksrc[][MAX_STRING]={
	{"0:HSC"},
	{"1:HSCDIV2"},

};

static char hsc32k[][MAX_STRING]={
	{"0:RC32K1K"},
	{"1:XTAL32K"},
};

static char pufclk[][MAX_STRING]={
	{"0:RC24M1M"},
	{"1:XTAL24M"},
};

static char dpclk[][MAX_STRING]={
	{"0:RC24M1M"},
	{"1:RC96M48M"},
	{"1:RC96M48M Div 2"},
	{"3:XTAL24M"},
};

static char rxclk[][MAX_STRING]={
	{"0:DP"},
	{"1:MIPIRX"},
	{"2:SENPARA"},
};
static char hscmipiclksrc[][MAX_STRING]={
	{"0:RC24M"},
	{"1:RC96M48M"},
	{"2:XTAL24M"},
	{"3:PLL"}
};
static char hscppitxclksrc[][MAX_STRING]={
	{"0:RC24M"},
	{"1:RC96M48M"},
	{"2:XTAL24M"},
	{"3:PLL"}
};

static char lscclksrc[][MAX_STRING]={
	{"0:RC24M1M"},
	{"1:XTAL24M"},
	{"2:RC96M48M"},
	{"3:PLL"},
};
static char lscref[][MAX_STRING]={
	{"0:RC24M1M"},
	{"1:RC96M48M"},
	{"2:XTAL24M"},
	{"3:PLL"},
};
static char lsc32k[][MAX_STRING]={
	{"0:RC32K1K"},
	{"1:XTAL32K"},
};
static char lscscmclksrc[][MAX_STRING]={
		{"0:RC24M1M"},
		{"1:RC96M48M"},
		{"2:XTAL24M"},
};
static char vad_d_clk[][MAX_STRING]={
		{"0:PDMPCM"},
		{"1:I2SHOSTPCM"},
		{"2:I2SSLAVEPCM"},
};
static char adcckin_src[][MAX_STRING]={
		{"0:RC24M1M"},
		{"1:XTAL24M"},
};
static char sbclksrc[][MAX_STRING]={
		{"0:RC24M1M"},
		{"1:XTAL24M"},
};
static char sbclk32k[][MAX_STRING]={
		{"0:RC32K1K"},
		{"1:XTAL32K"},
};

static char adc_lp_hv[][MAX_STRING]={
		{"0:RC32K1K"},
		{"1:XTAL32K"},
};

static char aonclk[][MAX_STRING]={
		{"0:APB1_RC32K1K"},
		{"1:APB1_XTAL32K"},
		{"2:RC32K1K"},
		{"3:XTAL32K"},
};

static char aonclk32k[][MAX_STRING]={
		{"0:RC32K1K"},
		{"1:XTAL32K"},
};

static char anticlk32k[][MAX_STRING]={
		{"0:RC32K1K"},
		{"1:XTAL32K"},
};

static char str_rc24m1msel[][MAX_STRING]={
		{"0:RC24M"},
		{"1:RC1M"},
};
static char str_rc96m48msel[][MAX_STRING]={
		{"0:RC48M"},
		{"1:RC96M"},
};
static char str_rc32k1ksel[][MAX_STRING]={
		{"0:RC1K"},
		{"1:RC32K"},
};


static char str_aonswregpllsel[][MAX_STRING]={
		{"0:PLL_DISABLE"},
		{"1:PLL_Enable"},
};


static char str_pllclksrc[][MAX_STRING]={
		{"0:RC24M1M,"},
		{"1:RC96M48M"},
		{"2:XTAL24M"}
};

static char str_qspiclksrc[][MAX_STRING]={
		{"0:RC24M1M"},
		{"1:RC96M48M"},
		{"2:XTAL24M"},
		{"3:PLL"},
};
static char str_ospiclksrc[][MAX_STRING]={
		{"0:RC24M1M"},
		{"1:RC96M48M"},
		{"2:XTAL24M"},
		{"3:PLL"},
};


static char str_ckmonsrc_clk[][MAX_STRING]={
		{"0:RC24M1M"},
		{"1:XTAL24M"},
};

static char str_sspimclksrc[][MAX_STRING]={
		{"0:RC24M1M"},
		{"1:RC96M48M"},
		{"2:XTAL24M"},
};

static char str_sspisclksrc[][MAX_STRING]={
		{"0:RC24M1M"},
		{"1:RC96M48M"},
		{"2:XTAL24M"},
};

static char str_i2cmsrc_clk[][MAX_STRING]={
		{"0:RC24M1M"},
		{"1:XTAL24M"},
};



void print_hsc_clkcfg()
{
	SCU_PDHSC_CLK_CFG_T cfg;

	hx_drv_scu_get_pdhsc_clk_cfg(&cfg);

	dbg_app_log("hscclksrc=%d(%s),hscclkdiv=%d\r\n", cfg.hscclk_related.hscclk.hscclksrc, hscclksrc[cfg.hscclk_related.hscclk.hscclksrc], cfg.hscclk_related.hscclk.hscclkdiv);
	dbg_app_log("hscd12clksrc=%d(%s), i3chcdiv=%d\r\n", cfg.hscclk_related.hscd12clksrc, hscd12clksrc[cfg.hscclk_related.hscd12clksrc] , cfg.hscclk_related.i3chcdiv);
	dbg_app_log("sdiodiv=%d, qspiclksrc=%d(%s)\r\n", cfg.hscclk_related.sdiodiv, cfg.qspiclksrc, str_qspiclksrc[cfg.qspiclksrc]);
	dbg_app_log("qspiclkdiv=%d, ospiclksrc=%d(%s)\r\n", cfg.qspiclkdiv, cfg.ospiclksrc, str_ospiclksrc[cfg.ospiclksrc]);
	dbg_app_log("ospiclkdiv=%d, hsc32k=%d(%s)\r\n", cfg.ospiclkdiv, cfg.hsc32k, hsc32k[cfg.hsc32k]);
	dbg_app_log("pufclk=%d(%s), dpclk=%d(%s)\r\n", cfg.pufclk, pufclk[cfg.pufclk], cfg.imageclk.dpclk, dpclk[cfg.imageclk.dpclk]);
	dbg_app_log("rxclk=%d(%s)\r\n", cfg.imageclk.rxclk, rxclk[cfg.imageclk.rxclk]);
	dbg_app_log("hscmipiclksrc=%d(%s), hscmipiclkdiv=%d\r\n", cfg.imageclk.mipiclk.hscmipiclksrc, hscmipiclksrc[cfg.imageclk.mipiclk.hscmipiclksrc], cfg.imageclk.mipiclk.hscmipiclkdiv);
	dbg_app_log("hscppitxclksrc=%d(%s), hscppitxclkdiv=%d\r\n", cfg.imageclk.ppitxclk.hscppitxclksrc, hscppitxclksrc[cfg.imageclk.ppitxclk.hscppitxclksrc], cfg.imageclk.ppitxclk.hscppitxclkdiv);

}


void print_hsc_clken()
{
	SCU_PDHSC_CLKEN_CFG_T cfg;

	hx_drv_scu_get_pdhsc_clken_cfg(&cfg);

	dbg_app_log("cm55m_clk_en=%d, u55_clk_en=%d\r\n", cfg.cm55m_clk_en, cfg.u55_clk_en);
	dbg_app_log("axi_clk_en=%d, ahb0_clk_en=%d\r\n", cfg.axi_clk_en, cfg.ahb0_clk_en);
	dbg_app_log("ahb5_clk_en=%d, ahb1_clk_en=%d\r\n", cfg.ahb5_clk_en, cfg.ahb1_clk_en);
	dbg_app_log("apb2_clk_en=%d, rom_clk_en=%d\r\n", cfg.apb2_clk_en, cfg.rom_clk_en);
	dbg_app_log("sram0_clk_en=%d, sram1_clk_en=%d\r\n", cfg.sram0_clk_en, cfg.sram1_clk_en);
	dbg_app_log("i3c_hc_clk_en=%d, puf_clk_en=%d\r\n", cfg.i3c_hc_clk_en, cfg.puf_clk_en);
	dbg_app_log("dma0_clk_en=%d, dma1_clk_en=%d\r\n", cfg.dma0_clk_en, cfg.dma1_clk_en);
	dbg_app_log("sdio_clk_en=%d, i2c2ahb_flash_w_clk_en=%d\r\n", cfg.sdio_clk_en, cfg.i2c2ahb_flash_w_clk_en);
	dbg_app_log("qspi_en=%d, ospi_en=%d\r\n", cfg.qspi_en, cfg.ospi_en);
	dbg_app_log("spi2ahb_en=%d\r\n", cfg.spi2ahb_en);

	dbg_app_log("xdma_w1_clk_en=%d, xdma_w2_clk_en=%d\r\n", cfg.imageclk_en.xdma_w1_clk_en, cfg.imageclk_en.xdma_w2_clk_en);
	dbg_app_log("xdma_w3_clk_en=%d, xdma_r_clk_en=%d\r\n", cfg.imageclk_en.xdma_w3_clk_en, cfg.imageclk_en.xdma_r_clk_en);
	dbg_app_log("scclk_clk_en=%d, inp_clk_en=%d\r\n", cfg.imageclk_en.scclk_clk_en, cfg.imageclk_en.inp_clk_en);
	dbg_app_log("dp_clk_en=%d, dp_2x2_clk_en=%d\r\n", cfg.imageclk_en.dp_clk_en, cfg.imageclk_en.dp_2x2_clk_en);
	dbg_app_log("dp_5x5_clk_en=%d, dp_cdm_clk_en=%d\r\n", cfg.imageclk_en.dp_5x5_clk_en, cfg.imageclk_en.dp_cdm_clk_en);
	dbg_app_log("dp_jpeg_clk_en=%d, dp_tpg_clk_en=%d\r\n", cfg.imageclk_en.dp_jpeg_clk_en, cfg.imageclk_en.dp_tpg_clk_en);
	dbg_app_log("dp_edm_clk_en=%d, dp_rgb2yuv_pclk_en=%d\r\n", cfg.imageclk_en.dp_edm_clk_en, cfg.imageclk_en.dp_rgb2yuv_pclk_en);
	dbg_app_log("dp_csc_pclk_en=%d, mipirx_clk_en=%d\r\n", cfg.imageclk_en.dp_csc_pclk_en, cfg.imageclk_en.mipirx_clk_en);
	dbg_app_log("mipitx_clk_en=%d\r\n", cfg.imageclk_en.mipitx_clk_en);

}

void print_lsc_clk()
{
	SCU_PDLSC_CLK_CFG_T cfg;

	hx_drv_scu_get_pdlsc_clk_cfg(&cfg);

	dbg_app_log("lscclksrc=%d(%s), lscclkdiv=%d\r\n", cfg.lscclk.lscclksrc, lscclksrc[cfg.lscclk.lscclksrc], cfg.lscclk.lscclkdiv);
	dbg_app_log("lscref=%d(%s), i2s_host_sclk_div=%d\r\n", cfg.lscref_related.lscref, lscref[cfg.lscref_related.lscref], cfg.lscref_related.i2s_host_sclk_div);
	dbg_app_log("pdmclk_div=%d, uart_div=%d\r\n", cfg.lscref_related.pdmclk_div, cfg.lscref_related.uart_div);
	dbg_app_log("i3cslv_div=%d, pwm_div=%d\r\n", cfg.lscref_related.i3cslv_div, cfg.lscref_related.pwm_div);
	dbg_app_log("ro_pd_div=%d, i2c_div=%d\r\n", cfg.lscref_related.ro_pd_div, cfg.lscref_related.i2c_div);
	dbg_app_log("lsc32k=%d(%s), vad_d_clk=%d\r\n", cfg.lsc32k, lsc32k[cfg.lsc32k], cfg.vad_d_clk);
	dbg_app_log("ckmonsrc_clk=%d(%s)\r\n", cfg.ckmonsrc_clk, str_ckmonsrc_clk[cfg.ckmonsrc_clk]);
	dbg_app_log("sspimsrc_clk=%d(%s), sspim_div=%d\r\n", cfg.sspimsrc_clk, str_sspimclksrc[cfg.sspimsrc_clk], cfg.sspim_div);
	dbg_app_log("sspissrc_clk=%d(%s), sspis_div=%d\r\n", cfg.sspissrc_clk, str_sspisclksrc[cfg.sspissrc_clk], cfg.sspis_div);
	dbg_app_log("lscscmclksrc=%d(%s), lscmclkdiv=%d\r\n", cfg.image_clk.scsenmclk.lscscmclksrc, lscscmclksrc[cfg.image_clk.scsenmclk.lscscmclksrc], cfg.image_clk.scsenmclk.lscmclkdiv);
	dbg_app_log("adcckin_src=%d(%s), adcckindiv=%d\r\n", cfg.adcckin.adcckin_src, adcckin_src[cfg.adcckin.adcckin_src], cfg.adcckin.adcckindiv);

}

void print_lsc_clken()
{
	SCU_PDLSC_CLKEN_CFG_T cfg;

	hx_drv_scu_get_pdlsc_clken_cfg(&cfg);

	dbg_app_log("cm55s_clk_en=%d, ahb_m_hclk_en=%d\r\n", cfg.cm55s_clk_en, cfg.ahb_m_hclk_en);
	dbg_app_log("ahb_2_hclk_en=%d, ahb_3_hclk_en=%d\r\n", cfg.ahb_2_hclk_en, cfg.ahb_3_hclk_en);
	dbg_app_log("apb_0_hclk_en=%d, sram2_clk_en=%d\r\n", cfg.apb_0_hclk_en, cfg.sram2_clk_en);
	dbg_app_log("dma2_clk_en=%d, dma3_clk_en=%d\r\n", cfg.dma2_clk_en, cfg.dma3_clk_en);
	dbg_app_log("i2s_host_sclk_en=%d, pdm_clk_en=%d\r\n", cfg.i2s_host_sclk_en, cfg.pdm_clk_en);
	dbg_app_log("uart0_clk_en=%d, uart1_clk_en=%d\r\n", cfg.uart0_clk_en, cfg.uart1_clk_en);
	dbg_app_log("uart2_clk_en=%d, i3c_slv0_sys_clk_en=%d\r\n", cfg.uart2_clk_en, cfg.i3c_slv0_sys_clk_en);
	dbg_app_log("i3c_slv1_sys_clk_en=%d, pwm012_clk_en=%d\r\n", cfg.i3c_slv1_sys_clk_en, cfg.pwm012_clk_en);
	dbg_app_log("i2s_slv_sclk_en=%d, ro_pd_clk_en=%d\r\n", cfg.i2s_slv_sclk_en, cfg.ro_pd_clk_en);
	dbg_app_log("i2c_slv0_ic_clk_en=%d, i2c_slv1_ic_clk_en=%d\r\n", cfg.i2c_slv0_ic_clk_en, cfg.i2c_slv1_ic_clk_en);
	dbg_app_log("i2c_mst_ic_clk_en=%d, i2c_mst_sen_ic_clk_en=%d\r\n", cfg.i2c_mst_ic_clk_en, cfg.i2c_mst_sen_ic_clk_en);
	dbg_app_log("sw_clk_en=%d, vad_d_clk_en=%d\r\n", cfg.sw_clk_en, cfg.vad_d_clk_en);
	dbg_app_log("adcck_en=%d, gpio_en=%d\r\n", cfg.adcck_en, cfg.gpio_en);
	dbg_app_log("sspim_en=%d, sspis_en=%d\r\n", cfg.sspim_en, cfg.sspis_en);
	dbg_app_log("ckmon_en=%d, sc_clk_lsc_en=%d\r\n", cfg.ckmon_en, cfg.imageclk_en.sc_clk_lsc_en);

}

void print_sb_clkcfg()
{
	SCU_PDSB_CLK_CFG_T cfg;

	hx_drv_scu_get_pdsb_clk_cfg(&cfg);

	dbg_app_log("sbclksrc=%d(%s), sbclkdiv=%d\r\n", cfg.sbclk.sbclksrc, sbclksrc[cfg.sbclk.sbclksrc], cfg.sbclk.sbclkdiv);
	dbg_app_log("sb32ksrc=%d(%s), himaxi2cm=%d(%s)\r\n", cfg.sb32ksrc, sbclk32k[cfg.sb32ksrc], cfg.himaxi2cm, str_i2cmsrc_clk[cfg.himaxi2cm]);

}

void print_sb_clkencfg()
{
	SCU_PDSB_CLKEN_CFG_T get_cfg;
	hx_drv_scu_get_pdsb_clken_cfg(&get_cfg);

	dbg_app_log("ahb_4_hclk_en=%d, ts_clk_en=%d\r\n", get_cfg.apb1_ahb4_pclk_en, get_cfg.ts_clk_en);
	dbg_app_log("adc_lp_hv_clk_en=%d, I2C2AHB_DBG_en=%d, WDT0_en=%d\r\n", get_cfg.adc_lp_hv_clk_en, get_cfg.I2C2AHB_DBG_en, get_cfg.WDT0_en);
	dbg_app_log("WDT1_en=%d, TIMER0_en=%d, TIMER1_en=%d\r\n", get_cfg.WDT1_en, get_cfg.TIMER0_en, get_cfg.TIMER1_en);
	dbg_app_log("TIMER2_en=%d, TIMER3_en=%d, TIMER4_en=%d\r\n", get_cfg.TIMER2_en, get_cfg.TIMER3_en, get_cfg.TIMER4_en);
	dbg_app_log("TIMER5_en=%d, TIMER6_en=%d, TIMER7_en=%d\r\n", get_cfg.TIMER5_en, get_cfg.TIMER6_en, get_cfg.TIMER7_en);
	dbg_app_log("TIMER8_en=%d, sb_gpio_en=%d, hmxi2cm_en=%d\r\n", get_cfg.TIMER8_en, get_cfg.sb_gpio_en, get_cfg.hmxi2cm_en);

}

void print_aon_clkcfg()
{
	SCU_PDAON_CLK_CFG_T cfg;

	hx_drv_scu_get_pdaon_clk_cfg(&cfg);

	dbg_app_log("aonclk=%d(%s), antitamp=%d(%s)\r\n", cfg.aonclk, aonclk[cfg.aonclk], cfg.antitamp, anticlk32k[cfg.antitamp]);

}

void print_aon_clkencfg()
{

	SCU_PDAON_CLKEN_CFG_T get_cfg;

	hx_drv_scu_get_pdaon_clken_cfg(&get_cfg);
	dbg_app_log("rtc0_clk_en=%d, rtc1_clk_en=%d, rtc2_clk_en=%d\r\n", get_cfg.rtc0_clk_en, get_cfg.rtc1_clk_en, get_cfg.rtc2_clk_en);
	dbg_app_log("pmu_clk_en=%d, aon_gpio_clk_en=%d, aon_swreg_clk_en=%d\r\n", get_cfg.pmu_clk_en, get_cfg.aon_gpio_clk_en, get_cfg.aon_swreg_clk_en);
	dbg_app_log("antitamper_clk_en=%d\r\n", get_cfg.antitamper_clk_en);

}

void dump_debug_reg_pmu()
{
	uint32_t reg_val = 0;
	uint32_t reg_addr = BASE_ADDR_APB_PMU_ALIAS;

	dbg_app_log("SCU start\n");
	for(reg_addr = BASE_ADDR_APB_SCU_AON_ALIAS; reg_addr<= (BASE_ADDR_APB_SCU_AON_ALIAS+0x4C); reg_addr=reg_addr+4)
	{
		reg_val = (*((volatile unsigned int*) reg_addr));
		dbg_app_log("0x%08x=0x%08x\n",reg_addr, reg_val);
	}
	for(reg_addr = BASE_ADDR_APB_SCU_AON_ALIAS+0x700; reg_addr<= (BASE_ADDR_APB_SCU_AON_ALIAS+0x710); reg_addr=reg_addr+4)
	{
		reg_val = (*((volatile unsigned int*) reg_addr));
		dbg_app_log("0x%08x=0x%08x\n",reg_addr, reg_val);
	}
	reg_addr = BASE_ADDR_APB_SCU_AON_ALIAS+0x730;
	reg_val = (*((volatile unsigned int*) reg_addr));
	dbg_app_log("0x%08x=0x%08x\n",reg_addr, reg_val);
	reg_addr = BASE_ADDR_APB_SCU_AON_ALIAS+0x734;
	reg_val = (*((volatile unsigned int*) reg_addr));
	dbg_app_log("0x%08x=0x%08x\n",reg_addr, reg_val);

	dbg_app_log("PMU start\n");
	for(reg_addr = BASE_ADDR_APB_PMU_ALIAS; reg_addr<= (BASE_ADDR_APB_PMU_ALIAS+0xFF); reg_addr=reg_addr+4)
	{
		reg_val = (*((volatile unsigned int*) reg_addr));
		dbg_app_log("0x%08x=0x%08x\n",reg_addr, reg_val);
	}

	dbg_app_log("\nSB Timer0 start\n");
	for(reg_addr = BASE_ADDR_APB_TIMER0_ALIAS; reg_addr<= (BASE_ADDR_APB_TIMER0_ALIAS+0x0C); reg_addr=reg_addr+4)
	{
		reg_val = (*((volatile unsigned int*) reg_addr));
		dbg_app_log("0x%08x=0x%08x\n",reg_addr, reg_val);
	}
}

void print_wakeup_event(uint32_t event, uint32_t event1)
{

	if((event & 0x1)  != 0)
	{
		dbg_app_log("event=%s\r\n", wakeup_event[0]);
	}
	if((event & 0x2) != 0)
	{
		dbg_app_log("event=%s\r\n", wakeup_event[1]);
	}
	if((event & 0x4) != 0)
	{
		dbg_app_log("event=%s\r\n", wakeup_event[2]);
	}
	if((event & 0x8) != 0)
	{
		dbg_app_log("event=%s\r\n", wakeup_event[3]);
	}
	if((event & 0x10) != 0)
	{
		dbg_app_log("event=%s\r\n", wakeup_event[4]);
	}
	if((event & 0x20) != 0)
	{
		dbg_app_log("event=%s\r\n", wakeup_event[5]);
	}
	if((event & 0x40) != 0)
	{
		dbg_app_log("event=%s\r\n", wakeup_event[6]);
	}
	if((event & 0x80) != 0)
	{
		dbg_app_log("event=%s\r\n", wakeup_event[7]);
	}
	if((event & 0x100) != 0)
	{
		dbg_app_log("event=%s\r\n", wakeup_event[8]);
	}
	if((event & 0x200) != 0)
	{
		dbg_app_log("event=%s\r\n", wakeup_event[9]);
	}
	if((event & 0x400) != 0)
	{
		dbg_app_log("event=%s\r\n", wakeup_event[10]);
	}
	if((event & 0x800) != 0)
	{
		dbg_app_log("event=%s\r\n", wakeup_event[11]);
	}
	if((event & 0x1000) != 0)
	{
		dbg_app_log("event=%s\r\n", wakeup_event[12]);
	}
	if((event & 0x2000) != 0)
	{
		dbg_app_log("event=%s\r\n", wakeup_event[13]);
	}
	if((event & 0x4000) != 0)
	{
		dbg_app_log("event=%s\r\n", wakeup_event[14]);
	}
	if((event & 0x8000) != 0)
	{
		dbg_app_log("event=%s\r\n", wakeup_event[15]);
	}
	if((event & 0x10000) != 0)
	{
		dbg_app_log("event=%s\r\n", wakeup_event[16]);
	}
	if((event & 0x20000) != 0)
	{
		dbg_app_log("event=%s\r\n", wakeup_event[17]);
	}
	if((event & 0x40000) != 0)
	{
		dbg_app_log("event=%s\r\n", wakeup_event[18]);
	}
	if((event & 0x80000) != 0)
	{
		dbg_app_log("event=%s\r\n", wakeup_event[19]);
	}
	if((event & 0x100000) != 0)
	{
		dbg_app_log("event=%s\r\n", wakeup_event[20]);
	}
	if((event & 0x200000) != 0)
	{
		dbg_app_log("event=%s\r\n", wakeup_event[21]);
	}
	if((event & 0x400000) != 0)
	{
		dbg_app_log("event=%s\r\n", wakeup_event[22]);
	}
	if((event & 0x800000) != 0)
	{
		dbg_app_log("event=%s\r\n", wakeup_event[23]);
	}
	if((event & 0x1000000) != 0)
	{
		dbg_app_log("event=%s\r\n", wakeup_event[24]);
	}
	if((event & 0x2000000) != 0)
	{
		dbg_app_log("event=%s\r\n", wakeup_event[25]);
	}
	if((event & 0x4000000) != 0)
	{
		dbg_app_log("event=%s\r\n", wakeup_event[26]);
	}
	if((event & 0x8000000) != 0)
	{
		dbg_app_log("event=%s\r\n", wakeup_event[27]);
	}
	if((event & 0x10000000) != 0)
	{
		dbg_app_log("event=%s\r\n", wakeup_event[28]);
	}
	if((event & 0x20000000) != 0)
	{
		dbg_app_log("event=%s\r\n", wakeup_event[29]);
	}
	if((event & 0x40000000) != 0)
	{
		dbg_app_log("event=%s\r\n", wakeup_event[30]);
	}
	if((event & 0x80000000) != 0)
	{
		dbg_app_log("event=%s\r\n", wakeup_event[31]);
	}

	if((event1 & 0x1)  != 0)
	{
		dbg_app_log("event1=%s\r\n", wakeup_event1[0]);
	}
	if((event1 & 0x2)  != 0)
	{
		dbg_app_log("event1=%s\r\n", wakeup_event1[1]);
	}
	if((event1 & 0x4)  != 0)
	{
		dbg_app_log("event1=%s\r\n", wakeup_event1[2]);
	}
	if((event1 & 0x100)  != 0)
	{
		dbg_app_log("event1=%s\r\n", wakeup_event1[3]);
	}
	if((event1 & 0x10000)  != 0)
	{
		dbg_app_log("event1=%s\r\n", wakeup_event1[4]);
	}
	if((event1 & 0x1000000)  != 0)
	{
		dbg_app_log("event1=%s\r\n", wakeup_event1[5]);
	}
	if((event1 & 0x2000000)  != 0)
	{
		dbg_app_log("event1=%s\r\n", wakeup_event1[6]);
	}

}

void getPMUExtIntStatus()
{
	uint32_t val;
	hx_drv_pmu_get_ctrl(PMU_AON_GPIO, &val);
	dbg_app_log("PMU_AON_GPIO intstatus=0x%x\n", val);
	hx_drv_pmu_get_ctrl(PMU_SB_GPIO, &val);
	dbg_app_log("PMU_SB_GPIO intstatus=0x%x\n", val);
	hx_drv_pmu_get_ctrl(PMU_WDOGINT0, &val);
	dbg_app_log("PMU_WDOGINT0 intstatus=0x%x\n", val);
	hx_drv_pmu_get_ctrl(PMU_WDOGINT1, &val);
	dbg_app_log("PMU_WDOGINT1 intstatus=0x%x\n", val);
	hx_drv_pmu_get_ctrl(PMU_PAD_AON_GPIO_0_STATUS, &val);
	dbg_app_log("PMU_PAD_AON_GPIO_0_STATUS intstatus=0x%x\n", val);
	hx_drv_pmu_get_ctrl(PMU_PAD_VMUTE_STATUS, &val);
	dbg_app_log("PMU_PAD_VMUTE_STATUS intstatus=0x%x\n", val);

}

void get_all_ip_int()
{
	uint32_t status, irq_raw_status, irq_status;
#ifdef IP_xdma
	uint32_t wdma1_status, wdma2_status, wdma3_status, rdma_status;
#endif
#ifdef IP_rtc
	for(uint8_t idx = RTC_ID_0; idx < RTC_ID_MAX; idx++)
	{
		hx_drv_rtc_get_alarm_rawint_status(idx, &status);
		dbg_app_log("RTC id=%d, raw status=%d\n", idx, status);
		hx_drv_rtc_get_alarm_maskint_status(idx, &status);
		dbg_app_log("RTC id=%d, mask status=%d\n", idx, status);
	}
#endif
#ifdef IP_timer
	for(uint8_t idx = TIMER_ID_0; idx < TIMER_ID_MAX; idx++)
	{
		status = hx_drv_timer_StatusIRQ(idx);
		dbg_app_log("Timer id=%d, raw status=%d\n", idx, status);
	}
#endif
#ifdef IP_watchdog
	for(uint8_t idx = WATCHDOG_ID_0; idx < WATCHDOG_ID_MAX; idx++)
	{
		irq_raw_status = hx_drv_watchdog_irq_raw_status(idx);

		irq_status = hx_drv_watchdog_irq_status(idx);

		dbg_app_log("WDG id=%d, irq_raw_status=0x%x, irq_status=0x%x\n", idx, irq_raw_status, irq_status);

	}
#endif
#ifdef IP_gpio
    status = hx_drv_gpio_get_int_status(AON_GPIO0);
    dbg_app_log("AON GPIO_0, int_status:%d \n", status);
    status = hx_drv_gpio_get_int_status(SB_GPIO0);
    dbg_app_log("SB GPIO_0, int_status:%d \n", status);
#endif
#ifdef IP_xdma
    hx_drv_xdma_get_WDMA1INTStatus(&wdma1_status);
    hx_drv_xdma_get_WDMA2INTStatus(&wdma2_status);
    hx_drv_xdma_get_WDMA3INTStatus(&wdma3_status);
    hx_drv_xdma_get_RDMAINTStatus(&rdma_status);
	dbg_app_log("wdma1_status=0x%x, wdma2_status=0x%x, wdma3_status=0x%x, rdma_status=0x%x\n", wdma1_status, wdma2_status, wdma3_status, rdma_status);
#endif
#ifdef IP_hxautoi2c_mst
	uint32_t autoi2c_rawstatus = 0;
	autoi2c_rawstatus = hx_drv_hxautoi2c_get_rawstatus();
    dbg_app_log("autoi2c_rawstatus:0x%x \n", autoi2c_rawstatus);
#endif
}

void app_debug_print()
{
    uint8_t spirxdelay;
    uint32_t freq;
    
    SCU_PLL_FREQ_E pmuwakeup_pll_freq;
	SCU_HSCCLKDIV_E pmuwakeup_cm55m_div;
	SCU_LSCCLKDIV_E pmuwakeup_cm55s_div;
	SCU_PLL_CLK_CFG_T pll_cfg;

    hx_drv_swreg_aon_get_pmuwakeup_freq(&pmuwakeup_pll_freq, &pmuwakeup_cm55m_div, &pmuwakeup_cm55s_div);
    hx_drv_swreg_aon_get_pllfreq(&freq);
    dbg_app_log("pmuwakeup_freq_type=%d, pmuwakeup_cm55m_div=%d, pmuwakeup_cm55s_div=%d\n", pmuwakeup_pll_freq, pmuwakeup_cm55m_div, pmuwakeup_cm55s_div);
    dbg_app_log("pmuwakeup_run_freq=%d\n", freq);
    hx_drv_scu_get_pll_cfg(&pll_cfg);
    dbg_app_log("init_pll_para=%d,sysclksrc_sel=%d(%s)\n", pll_cfg.init_pll_para, pll_cfg.sysclksrc_sel, hwclk_case[pll_cfg.sysclksrc_sel]);
    dbg_app_log("clk_src=%d,%s,freq=%d\n", pll_cfg.clk_src, str_pllclksrc[pll_cfg.clk_src], pll_cfg.freq);
    dbg_app_log("pllclksrc_div=%d,pll_freq=%d\n", pll_cfg.pllclksrc_div, pll_cfg.pll_freq);
    
#if 0
    print_hsc_clkcfg();
    print_lsc_clk();
    print_sb_clkcfg();
    print_aon_clkcfg();

    print_hsc_clken();
    print_lsc_clken();
    print_sb_clkencfg();
    print_aon_clkencfg();
#endif

    //hx_drv_swreg_aon_get_spi_rxsample_delay(&spirxdelay);
    //dbg_app_log("spirxdelay= %d\r\n", spirxdelay);

    getPMUExtIntStatus();
    get_all_ip_int();
}

