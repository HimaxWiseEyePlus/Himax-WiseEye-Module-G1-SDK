##
# platform (onchip ip) support feature
# Add all of supported ip list here
# The source code should be located in ~\drivers\{ip_name}\
##

DRIVERS_IP_LIST ?= iic \
					swreg_aon \
					swreg_lsc \
					scu \
                    timer \
					mpc \
					ppc \
					pmu \
                    uart \
					mb

DRIVERS_IP_INSTANCE ?= TIMER4 \
						TIMER5 \
						TIMER6 \
						TIMER7 \
                        TIMER8 \
                        UART1

DRIVERS_IP_NS_INSTANCE ?=
