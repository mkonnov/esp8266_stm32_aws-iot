CMOCK_ROOT=../../cmock

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

APP_SRCS=../../../common/uart_protocol/uart_command_protocol.c \
	 ../../../common/crc16/crc16.c

TEST_SRCS=test.c
TEST_NAME=uart_command_protocol

INC_DIRS=-I$(UNITY_ROOT)/src -I../../../common/uart_protocol/include \
	-I../../../common/crc16/include
#SYMBOLS=-DUART_COMMAND_PROTOCOL_DBG

include ../Makefile_stm32.inc
