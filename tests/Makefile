.DEFAULT_GOAL := all

# Silent build by default
ifndef VERBOSE
.SILENT:
endif


export CLEANUP = rm -rf
export MKDIR = mkdir -p
export TARGET_EXTENSION=.out

export OBJ ?= ${BUILD_DIR}/obj
export OBJ_DIR = ${OBJ}

export C_COMPILER=gcc

export CMOCK_ROOT=$(realpath cmock)
export UNITY_ROOT=$(CMOCK_ROOT)/vendor/unity

export SYMBOLS=

#CFLAGS=-std=c89
#CFLAGS=-Wall
CFLAGS += -Wunused-parameter
CFLAGS += -Wextra
CFLAGS += -Wpointer-arith
CFLAGS += -Wcast-align
CFLAGS += -Wwrite-strings
CFLAGS += -Wswitch-default
CFLAGS += -Wunreachable-code
CFLAGS += -Winit-self
CFLAGS += -Wmissing-field-initializers
CFLAGS += -Wno-unknown-pragmas
CFLAGS += -Wstrict-prototypes
CFLAGS += -Wundef
CFLAGS += -Wold-style-definition
#CFLAGS += -Wno-misleading-indentation
CFLAGS += -Werror=maybe-uninitialized

export CFLAGS

export UNITY_SRCS=$(UNITY_ROOT)/src/unity.c 
#UNITY_SRCS+=$(UNITY_ROOT)/extras/fixture/src/unity_fixture.c
export CMOCK_SRCS=$(CMOCK_ROOT)/src/cmock.c

export SRCS = $(UNITY_SRCS) $(CMOCK_SRCS)

export INC_DIRS = -I $(UNITY_ROOT)/src \
        -I $(UNITY_ROOT)/extras/fixture/src \
        -I $(UNITY_ROOT)/extras/memory/src \
        -I $(CMOCK_ROOT)/src

export TEST_MAKEFILE=$(realpath test.mk)

all: test_uart_command_protocol test_esp8266

test_uart_command_protocol:
	$(MAKE) -C uart_command_protocol

test_esp8266:
	$(MAKE) -C esp8266-tests

test_stm32:
	$(MAKE) -C stm32-tests

.PHONY: test_uart_command_protocol test_esp8266 test_stm32
