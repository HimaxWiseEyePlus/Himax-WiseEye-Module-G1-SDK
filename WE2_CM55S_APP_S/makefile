## EPII application makefile template ##
##
# root dir of EPII_ROOT
##
EPII_ROOT = .

# Application name
APPL ?= WE2_CM55S

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
EPII_CORE ?= SMALL

##
# IC package select : LQFP128/WLCSP65
##
IC_PACKAGE_SEL = LQFP128

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
TOOLCHAIN ?= gnu
##
# Definition of Tool Paths
##
#ARM_TOOLPATH ?=
#GNU_TOOLPATH ?=
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
NONTRUSTZONE_SRC_DIR ?= 

##
# standalone (y==> no bootrom, bootloader) (n ==> have bootrom, bootloader)
# y or n
STANDALONE ?= n

##
# CIS_SENSOR support feature HM_COMMON, HM2140, HM01B0_MONO, HM01B0_BAYER
# HM0360_MONO, HM0360_MONO_OSC, HM0360_BAYER, HM0360_BAYER_OSC, HM11B1, HM11B1_REVC
# HM0360_MONO_REVB, HM0360_MONO_OSC_REVB, HM0360_BAYER_REVB, HM0360_BAYER_OSC_REVB, HM2130
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
EPII_USECASE_SEL = drv_dualcore_tz_cm55s_s_pmu

EPII_NSC_SEL = cm55s_nsc_cfg

# USING Pre-built BSP library
# 1 for enable
# other for disable
##
LIB_BSP_PREBUILT = 0

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
# Integrate Support
# CLI_HELLOWORLD_TZ_S_ONLY
# CLI_HELLOWORLD_NOTZ
# CLI_PLATFORM_AT_S
# CLI_TZS_NOTZ
# CLI_TZS_MPC_AT_S
# CLI_TZS_PPC_AT_S
# CLI_TZS_TZ_AT_S_MSC_DMA
# CLI_PMU_TZ_CM55SS_ONLY
# CLI_CPU_TZ_S_ONLY
# CLI_MEM_SRAM2_TEST
# HWACCBYTPG_AUTO_TEST
# CLI_PERIPHERAL
# CLI_HM01B0_MONO_DP_LIB
# CLI_HM01B0_BAYER_DP_LIB
# CLI_HM11B1_DP_LIB
# CLI_HM0360_MONO_DP_LIB
# CLI_HM0360_BAYER_DP_LIB
# CLI_HM2140_DP_LIB
# CLI_HM2130_DP_LIB
# CLI_AUDIO_PREROLLING
# CLI_PLATFORM_AT_S_CASE5

# PS: As follow are scenario_app app name should be the same as directory name
# hello_world_tz
# hello_world_freertostz
# hello_world_freertostz_mpu
# hello_world_tz_s_only
# hello_world_evt_tz_s_only
# hello_world_freertos_tz_s_only
# hello_world_freertos_mpu_tz_s_only
# hello_world_notz
# tz_to_cli_dual_pmu
# tz_to_cli_dual
# pmu_sim_audpre_cm55s_timer1
# pmu_sim_audpre_cm55s_vad
# pmu_sim_audvidpre_cm55s_timer1
# pmu_sim_audvidpre_cm55s_vad
# pmu_sim_dualcore_cm55s_bus_access
# tz_to_cli_dual_timer
# dc_scl_sch_tz_s_only
##
#APP_TYPE = tz_to_cli_dual_pmu
APP_TYPE = sample_code_app_dualcore_s_only
### Options above must be added before include options.mk ###
override EPII_ROOT := $(strip $(subst \,/,$(EPII_ROOT)))
include $(EPII_ROOT)/options/options.mk
