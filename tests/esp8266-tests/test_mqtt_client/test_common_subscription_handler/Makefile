# ===================================
# Required variables:
# APP_SRCS - target application files that we're covering by tests
# TEST_SRCS - sources of the unit tests
# TEST_NAME - a name of test
#
# Optional variables:
# INCLUDES_TO_MOCK - the header files used to generate the mocks
# CFLAGS
# SYMBOLS
# INC_DIRS
# ===================================

TEST_NAME = test_common_subscription_handler

APP_SRCS += ../../../../esp-firmware/main/mqtt_client_task/aws_iot_mqtt_task_common_sub_handler.c
APP_SRCS += ../../../../esp-firmware/main/mqtt_client_task/aws_iot_mqtt_task_handler_procedures.c
APP_SRCS += ../../../../esp-firmware/ESP8266_RTOS_SDK/components/jsmn/src/jsmn.c

TEST_SRCS = test_common_subscription_handler.c

INCLUDES_TO_MOCK = ../../../../esp-firmware/main/mqtt_client_task/aws_iot_mqtt_task_payload_handlers.h

INC_DIRS += -I ../../../../esp-firmware/main/mqtt_client_task
INC_DIRS += -I ../../../../esp-firmware/ESP8266_RTOS_SDK/components/jsmn/include

CFLAGS += -Wno-switch-default



include $(TEST_MAKEFILE)
