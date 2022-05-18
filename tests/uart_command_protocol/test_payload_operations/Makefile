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

INC_DIRS+=-I.
INC_DIRS+=-I../../../common/uart_protocol/include
INC_DIRS+=-I../../../common/crc16/include

APP_SRCS+=../../../common/uart_protocol/uart_command_protocol.c
APP_SRCS+=../../../common/crc16/crc16.c

#INCLUDES_TO_MOCK = 

TEST_SRCS = test_payload_operations.c

TEST_NAME = payload_operations

include $(TEST_MAKEFILE)
