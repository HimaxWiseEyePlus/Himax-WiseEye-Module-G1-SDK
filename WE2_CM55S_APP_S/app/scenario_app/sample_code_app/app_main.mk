override SCENARIO_APP_SUPPORT_LIST := $(APP_TYPE)

#CIS = HM11B1_MONO
#CIS = HM0360_BAYER
#CIS = HM0360_MONO
#CIS = HM2170_BAYER

#APPL_DEFINES += -DAIOT_NB_EXAMPLE_TZ_S_ONLY
#APPL_DEFINES += -DEVT_CM55SMB_NBAPP
#APPL_DEFINES += -DEVT_DATAPATH
#APPL_DEFINES += -DEVT_CM55STIMER
APPL_DEFINES += -DDBG_MORE
#APPL_DEFINES += -DCM55M_ENABLE_CM55S
#APPL_DEFINES += -DENABLE_EVENT_IDLE_WFI
#APPL_DEFINES += -DTF_LITE_STATIC_MEMORY

#For PMU dump register
#APPL_DEFINES += -DLIBPWRMGMT_PMUDUMP
#APPL_DEFINES += -DAPP_PMU_REG_EXP

#EVENTHANDLER_SUPPORT = event_handler
#EVENTHANDLER_SUPPORT_LIST += evt_datapath
#EVENTHANDLER_SUPPORT_LIST += evt_cm55mmb_nbapp
#EVENTHANDLER_SUPPORT_LIST += evt_cm55stimer
#EVENTHANDLER_SUPPORT_LIST += evt_i2ccomm


#TRANSMIT_PROTOCOL = SPI_MASTER
#TRANSMIT_PROTOCOL = SPI_SLAVE
#TRANSMIT_PROTOCOL = UART

################
## Config APP ##
################
#include $(SCENARIO_APP_ROOT)/app_api/app_api_dummy.inc
include $(SCENARIO_APP_ROOT)/app_api/app_api.inc
APPL_APP_ROOT = $(SCENARIO_APP_ROOT)/$(APP_TYPE)

####################
## C source files ##
####################
APPL_APP_CSRC_LIST = src/app_main.c \
                     src/app_i2c_cmd.c


#####################
## CC source files ##
#####################
APPL_APP_CCSRC_LIST =

##################
## Header files ##
##################
APPL_APP_INCDIR_LIST = include

## append file path
APPL_APP_CSRCS = $(addprefix $(APPL_APP_ROOT)/, $(APPL_APP_CSRC_LIST))
APPL_APP_CCSRCS = $(addprefix $(APPL_APP_ROOT)/, $(APPL_APP_CCSRC_LIST))
APPL_APP_INCDIRS = $(addprefix $(APPL_APP_ROOT)/, $(APPL_APP_INCDIR_LIST))


##
# library support feature
# Add new library here
# The source code should be loacted in ~\library\{lib_name}\
##
#LIB_SEL = hxevent
#LIB_SEL = pwrmgmt
LIB_SEL += sensordp
LIB_SEL += spi_ptl
#LIB_SEL += tflm 
#LIB_SEL += spi_eeprom
#LIB_SEL += img_proc 
LIB_SEL += i2c_comm
#LIB_SEL += audio
##
# middleware support feature
# Add new middleware here
# The source code should be loacted in ~\middleware\{mid_name}\
##
MID_SEL =

override undefine OS_SEL
override TRUSTZONE := y
override TRUSTZONE_TYPE := security
override TRUSTZONE_FW_TYPE := 1
override EPII_USECASE_SEL := drv_daulcore_cm55s_s_only
override CIS_SEL := HM_COMMON
override ALGO_TYPE := 

#CIS_SUPPORT_INAPP = cis_sensor
#CIS_SUPPORT_INAPP_MODEL = cis_hm11b1

ifeq ($(strip $(TOOLCHAIN)), arm)
override LINKER_SCRIPT_FILE := $(SCENARIO_APP_ROOT)/$(APP_TYPE)/TrustZone_S_ONLY.sct
else#TOOLChain
override LINKER_SCRIPT_FILE := $(SCENARIO_APP_ROOT)/$(APP_TYPE)/TrustZone_S_ONLY.ld
endif

#ifeq ("$(ALGO_TYPE)","TEST_CV_ALGO")
#LIB_SEL += test_cv_algo
#else #default algo
#endif

##
# Add new external device here
# The source code should be located in ~\external\{device_name}\
##
#EXT_DEV_LIST += 


####################################
## Transmit Protocol APPL_DEFINES ##
####################################
APPL_DEFINES += -DI2C_COMM
#ifeq ($(TRANSMIT_PROTOCOL), UART)
#	APPL_DEFINES += -DUART_PROTOCOL
#else ifeq ($(TRANSMIT_PROTOCOL), SPI_MASTER)
#	APPL_DEFINES += -DSPI_MASTER_SEND
#else ifeq ($(TRANSMIT_PROTOCOL), SPI_SLAVE)
#	APPL_DEFINES += -DSPI_SLAVE_SEND
#endif