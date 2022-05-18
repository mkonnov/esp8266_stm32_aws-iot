# ===================================
# Required variables:
# APP_SRCS - target application files that we're covering by tests
# TEST_SRCS - sources of the unit tests
# TEST_NAME - a name of test
#
# Optional variables:
# CFLAGS
# SYMBOLS
# INC_DIRS
# ===================================
#

TEST_NAME = test_payload_handlers

INC_DIRS += -I.
INC_DIRS += -I ../../../../esp-firmware/main 
INC_DIRS += -I ../../../../esp-firmware/ESP8266_RTOS_SDK/components/jsmn/include 
INC_DIRS += -I../../../../esp-firmware/main/mqtt_client_task
INC_DIRS += -I build

INCLUDES_TO_MOCK = ../../../../esp-firmware/main/stm32_command_handlers.h

APP_SRCS = ../../../../esp-firmware/main/mqtt_client_task/aws_iot_mqtt_task_payload_handlers.c \
	../../../../esp-firmware/ESP8266_RTOS_SDK/components/jsmn/src/jsmn.c

CFLAGS += -Wno-switch-default

TEST_SRCS = test_parse_payload_handlers.c

include $(TEST_MAKEFILE)
