## EPII application makefile template ##
##
# root dir of EPII_ROOT
##
EPII_ROOT = .

# Application name
APPL ?= WE2_CM55M

# Optimization Level
OLEVEL ?= O2
DEBUG ?= 1
##
# Current Board And Core
# BOARD: epii_fpga, epii_evb
##
BOARD ?= epii_evb
BD_VER ?= 10
CORTEX_M ?= 55
#BIG, SMALL
EPII_CORE ?= BIG

##
# IC package select : LQFP128/WLCSP65
##
IC_PACKAGE_SEL = WLCSP65

##
# IC Version: 10(chipA) 20(chipB)..
##
IC_VER ?= 30

##
# Core Volt 0P9V, 0P8V
##
CoreVolt ?= 0P9V

##
# Set toolchain
# arm, gnu
##
TOOLCHAIN ?= arm
##
# Definition of Tool Paths
##
ARM_TOOLPATH ?=
GNU_TOOLPATH ?=
#ARM_TOOLPATH ?= C:/Program Files/Arm/Development Studio 2020.1/sw/ARMCompiler6.15/bin
#GNU_TOOLPATH ?= D:/Project/WE-II/CMSIS/cmsis_tool/gcc-arm-none-eabi-10-2020-q4-major-win32/gcc-arm-none-eabi-10-2020-q4-major/bin

##
# Selected OS
# bare-matal: mark OS_SEL, support os: un-mark OS_SEL
OS_SEL ?= 

##
# Selected MPU
# MPU: y or n
MPU ?= y

##
# TrustZone
# TrustZone support y or n
# TRUSTZONE_TYPE security or non-security
# TRUSTZONE_FW_TYPE 0(S and NS) or 1(S only)
TRUSTZONE ?= y
TRUSTZONE_TYPE ?= security
TRUSTZONE_FW_TYPE ?= 0
NONTRUSTZONE_SRC_DIR ?= D:\WEII_Workspace_Old\EPII_CM55M_APP_NS_Case2



##
# CIS_SENSOR support feature HM_COMMON, HM2140, HM01B0_MONO, HM01B0_BAYER
# HM0360_MONO, HM0360_MONO_OSC, HM0360_BAYER, HM0360_BAYER_OSC, HM11B1, HM11B1_REVC
# HM0360_MONO_REVB, HM0360_MONO_OSC_REVB, HM0360_BAYER_REVB, HM0360_BAYER_OSC_REVB, HM2130, HM2170
##
CIS_SEL =

##
# Platform IP Use Case Selection
# drv_onecore_cm55m_s_only,
# drv_onecore_cm55s_s_only,
# drv_onecore_cm55m_tz_s, drv_onecore_cm55m_tz_ns , 
# drv_onecore_cm55s_tz_s, drv_onecore_cm55s_tz_ns ,
# drv_dualcore_cm55m_s_only, drv_dualcore_cm55s_s_only, 
# drv_dualcore_tz_cm55m_s, drv_dualcore_tz_cm55m_ns, drv_dualcore_tz_cm55s_s, drv_dualcore_tz_cm55s_ns, 
# drv_user_defined 
##
EPII_USECASE_SEL = drv_onecore_cm55m_s_only


EPII_NSC_SEL = cm55m_nsc_cfg

# USING Inference Engine library with CMSIS_NN
# 1 for enable
# other for disable
##
LIB_CMSIS_NN_ENALBE = 0

# USING Pre-built CV library
# 1 for enable
# other for disable
##
LIB_CV_PREBUILT = 0

# USING Pre-built Audio Algo library
# 1 for enable
# other for disable
##
LIB_AUDIOALGO_PREBUILT = 0

##
# cmsis_drivers Use Case Selection
# cmsis_drivers_list
##
CMSIS_DRIVERS_USECASE_SEL =

# USING Pre-built Secure crypto library
# y for enable
# other for disable
##
SUPPORT_SEC_LIB ?= n
##
# Integrate Support
# CLI_CPU_NOTZ
# CLI_CPU_TZ_S_ONLY
# CLI_PLATFORM_AT_S
# CLI_TZS_NOTZ
# CLI_TZS_TZ_S_ONLY
# CLI_MEM_TCM_TEST
# CLI_CM55S_SRAM2_TEST
# CLI_TZS_MPC_AT_S
# CLI_TZS_PPC_AT_S
# CLI_TZS_TZ_AT_S_MSC_DMA
# CLI_TZS_TZ_AT_S_MSC_DPDMA
# CLI_TZS_TZ_AT_S_MSC_PER
# CLI_HELLOWORLD_TZ_S_ONLY
# CLI_HELLOWORLD_NOTZ
# CLI_PERIPHERAL
# HWACCBYTPG_AUTO_TEST
# CLI_PERIPHERAL
# CLI_DATAPATH
# CLI_HM01B0_MONO_DP_LIB
# CLI_HM01B0_BAYER_DP_LIB
# CLI_HM11B1_DP_LIB
# CLI_HM0360_MONO_DP_LIB
# CLI_HM0360_BAYER_DP_LIB
# CLI_HM2140_DP_LIB
# CLI_HM2130_DP_LIB
# CLI_HM2170_DP_LIB
# CLI_HM1246_DP_LIB
# CLI_HM0435_DP_LIB
# CLI_HM2056_DP_LIB
# CLI_HM5532_DP_LIB
# CLI_OV02C10_DP_LIB
# CLI_SOIK351P_DP_LIB
# CLI_CKGEN_TZ_S_ONLY
# CLI_PMU_TZ_S_ONLY
# CLI_HM01B0_MONO_DP_LIB_PMU
# CLI_HM0360_MONO_DP_LIB_PMU
# CLI_U55
# CLI_SDIO
# CLI_SECURITY_IP

# PS: As follow are scenario_app app name should be the same as directory name
# hello_world_tz
# hello_world_freertostz
# hello_world_freertostz_mpu
# hello_world_tz_s_only
# hello_world_evt_tz_s_only
# hello_world_freertos_tz_s_only
# hello_world_freertos_mpu_tz_s_only
# hello_world_notz
# hello_world_notz_dual
# hello_world_freertos_notz
# hello_world_freertos_mpu_notz
# hello_world_dual_s
# freertos_tz_s_only_sample
# freertos_tz_s_dp_sample
# pmu_tz_s_only
# pmu_tz_s_only_dual
# pmu_sim_tz_s_only
# pmu_dpd_sim_tz_s_only
# pmu_sim_tz_s_only_vidpre
# pmu_sim_tz_s_only_cm55m_audpre_timer1
# pmu_sim_tz_s_only_cm55m_audpre_vad
# pmu_sim_tz_s_only_cm55m_audvidpre_timer1
# pmu_sim_tz_s_only_cm55m_audvidpre_vad
# pmu_sim_tz_s_only_dual_bus_access
# tz_to_cli
# tz_to_cli_timer
# tz_to_cli_dppmu
# tz_to_cli_dual
# tz_to_cli_dppmu_dual
# tz_to_cli_dual_timer
# tflm_notz
# auto_test (need set CIS_SEL = HM2170)
# tflm_fd_fl (need set CIS_SEL = HM0360_MONO_REVB)
# dc_scl_sch_tz_s_only
# aiot_nb_example_tz_s_only
# tflm_2in1_fd_fl_pl
# allon_sensor
# demo_tfm170_s
# pmu_dpd_sample
# pmu_pd_sample
# pmu_vidpre_sample
# tz_pmu_sample
# tz_pmu_sample_dual
##
#APP_TYPE = CLI_PLATFORM_AT_S
#APP_TYPE = CLI_PERIPHERAL
#APP_TYPE = CLI_PMU_TZ_S_ONLY

APP_TYPE = sample_code_app_singlecore_s_only

#APP_TYPE = sample_code_app_dpd_singlecore_s_only
#APP_TYPE = sample_code_app_vidpre_singlecore_s_only

### Options above must be added before include options.mk ###
override EPII_ROOT := $(strip $(subst \,/,$(EPII_ROOT)))
include $(EPII_ROOT)/options/options.mk
