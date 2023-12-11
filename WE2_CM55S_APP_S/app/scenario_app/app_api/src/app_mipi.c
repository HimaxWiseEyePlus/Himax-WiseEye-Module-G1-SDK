#include "WE2_ARMCM55.h"

#include "WE2_device_addr.h"
#include "WE2_debug.h"

#include "app_cisdp_cfg.h"

#ifdef TRUSTZONE_SEC
#ifdef IP_INST_NS_csirx
#define	CSIRX_REGS_BASE 				BASE_ADDR_MIPI_RX_CTRL
#define CSIRX_DPHY_REG					(BASE_ADDR_APB_MIPI_RX_PHY+0x50)
#define CSIRX_DPHY_TUNCATE_REG			(BASE_ADDR_APB_MIPI_RX_PHY+0x48)
#else
#define CSIRX_REGS_BASE             	BASE_ADDR_MIPI_RX_CTRL_ALIAS
#define CSIRX_DPHY_REG					(BASE_ADDR_APB_MIPI_RX_PHY_ALIAS+0x50)
#define CSIRX_DPHY_TUNCATE_REG			(BASE_ADDR_APB_MIPI_RX_PHY_ALIAS+0x48)
#endif
#else
#ifndef TRUSTZONE
#define CSIRX_REGS_BASE             	BASE_ADDR_MIPI_RX_CTRL_ALIAS
#define CSIRX_DPHY_REG					(BASE_ADDR_APB_MIPI_RX_PHY_ALIAS+0x50)
#define CSIRX_DPHY_TUNCATE_REG			(BASE_ADDR_APB_MIPI_RX_PHY_ALIAS+0x48)
#else
#define CSIRX_REGS_BASE             	BASE_ADDR_MIPI_RX_CTRL
#define CSIRX_DPHY_REG					(BASE_ADDR_APB_MIPI_RX_PHY+0x50)
#define CSIRX_DPHY_TUNCATE_REG			(BASE_ADDR_APB_MIPI_RX_PHY+0x48)
#endif
#endif

void app_set_mipi_csirx_enable()
{
	dbg_printf(DBG_LESS_INFO, "MIPI CSI Init Enable\n");

    volatile uint32_t *dphy_reg = CSIRX_DPHY_REG;
    volatile uint32_t *csi_static_cfg_reg = (CSIRX_REGS_BASE+0x08);
    volatile uint32_t *csi_dphy_lane_control_reg = (CSIRX_REGS_BASE+0x40);
    volatile uint32_t *csi_stream0_control_reg = (CSIRX_REGS_BASE+0x100);
    volatile uint32_t *csi_stream0_data_cfg = (CSIRX_REGS_BASE+0x108);
    volatile uint32_t *csi_stream0_cfg_reg = (CSIRX_REGS_BASE+0x10C);

    sensordplib_csirx_enable(SENDPLIB_MIPIRX_LANE_NB);

    dbg_printf(DBG_LESS_INFO, "0x%08X = 0x%08X\n", dphy_reg, *dphy_reg);
    dbg_printf(DBG_LESS_INFO, "0x%08X = 0x%08X\n", csi_static_cfg_reg, *csi_static_cfg_reg);
    dbg_printf(DBG_LESS_INFO, "0x%08X = 0x%08X\n", csi_dphy_lane_control_reg, *csi_dphy_lane_control_reg);
    dbg_printf(DBG_LESS_INFO, "0x%08X = 0x%08X\n", csi_stream0_data_cfg, *csi_stream0_data_cfg);
    dbg_printf(DBG_LESS_INFO, "0x%08X = 0x%08X\n", csi_stream0_control_reg, *csi_stream0_control_reg);

}

void app_set_mipi_csirx_disable()
{
	dbg_printf(DBG_LESS_INFO, "MIPI CSI Disable\n");

    volatile uint32_t *dphy_reg = CSIRX_DPHY_REG;
    volatile uint32_t *csi_static_cfg_reg = (CSIRX_REGS_BASE+0x08);
    volatile uint32_t *csi_dphy_lane_control_reg = (CSIRX_REGS_BASE+0x40);
    volatile uint32_t *csi_stream0_control_reg = (CSIRX_REGS_BASE+0x100);
    volatile uint32_t *csi_stream0_data_cfg = (CSIRX_REGS_BASE+0x108);
    volatile uint32_t *csi_stream0_cfg_reg = (CSIRX_REGS_BASE+0x10C);

    sensordplib_csirx_disable();

    dbg_printf(DBG_LESS_INFO, "0x%08X = 0x%08X\n", dphy_reg, *dphy_reg);
    dbg_printf(DBG_LESS_INFO, "0x%08X = 0x%08X\n", csi_static_cfg_reg, *csi_static_cfg_reg);
    dbg_printf(DBG_LESS_INFO, "0x%08X = 0x%08X\n", csi_dphy_lane_control_reg, *csi_dphy_lane_control_reg);
    dbg_printf(DBG_LESS_INFO, "0x%08X = 0x%08X\n", csi_stream0_data_cfg, *csi_stream0_data_cfg);
    dbg_printf(DBG_LESS_INFO, "0x%08X = 0x%08X\n", csi_stream0_control_reg, *csi_stream0_control_reg);

}
