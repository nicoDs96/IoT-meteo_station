# name of your application
APPLICATION = lora_station

# If no BOARD is found in the environment, use this default:
BOARD ?= b-l072z-lrwan1

BOARD_INSUFFICIENT_MEMORY := arduino-duemilanove arduino-uno \
                             nucleo-f031k6 nucleo-f042k6 nucleo-l031k6

BOARD_BLACKLIST := msb-430 msb-430h pic32-clicker pic32-wifire \
                   telosb wsn430-v1_3b wsn430-v1_4 z1


# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../../

LORA_DRIVER ?= sx1276
LORA_REGION ?= EU868

#Lora stuff
USEPKG += semtech-loramac
USEMODULE += $(LORA_DRIVER)

USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += ps
USEMODULE += fmt
# For testing we also include the ping6 command and some stats
USEMODULE += xtimer
USEMODULE += semtech_loramac_rx


# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
DEVELHELP ?= 1


FEATURES_OPTIONAL += periph_eeprom

CFLAGS += -DREGION_$(LORA_REGION)
CFLAGS += -DLORAMAC_ACTIVE_REGION=LORAMAC_REGION_$(LORA_REGION)

# Comment this out to join RPL DODAGs even if DIOs do not contain
# DODAG Configuration Options (see the doc for more info)
# CFLAGS += -DGNRC_RPL_DODAG_CONF_OPTIONAL_ON_JOIN

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

include $(RIOTBASE)/Makefile.include
