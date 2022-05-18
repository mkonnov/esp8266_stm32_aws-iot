CLEANUP = rm -rf
MKDIR = mkdir -p
TARGET_EXTENSION=.out

TEST_MAKEFILE = ${TEST_BUILD_DIR}/MakefileTestSupport
OBJ ?= ${BUILD_DIR}/obj
OBJ_DIR = ${OBJ}

# Silent build by default
ifndef VERBOSE
.SILENT:
endif

C_COMPILER=gcc

UNITY_ROOT=$(CMOCK_ROOT)/vendor/unity

#CFLAGS=-std=c89
CFLAGS=-Wall
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

TARGET = $(TEST_NAME)$(TARGET_EXTENSION)

UNITY_SRCS=$(UNITY_ROOT)/src/unity.c 
#UNITY_SRCS+=$(UNITY_ROOT)/extras/fixture/src/unity_fixture.c
CMOCK_SRCS=$(CMOCK_ROOT)/src/cmock.c
RUNNER_SRCS=$(patsubst %.c,%_runner.c,$(TEST_SRCS))

SRCS=$(UNITY_SRCS) $(APP_SRCS) $(APP_MOCK_SRCS) $(TEST_SRCS) $(RUNNER_SRCS) $(CMOCK_SRCS)

INC_DIRS+=-I $(UNITY_ROOT)/src \
        -I $(UNITY_ROOT)/extras/fixture/src \
        -I $(UNITY_ROOT)/extras/memory/src \
        -I $(CMOCK_ROOT)/src

all: clean default

default: $(SRCS)
	$(C_COMPILER) $(CFLAGS) $(INC_DIRS) $(SYMBOLS) $(SRCS) -o $(TARGET)
	- ./$(TARGET)

$(RUNNER_SRCS): $(TEST_SRCS)
	ruby $(UNITY_ROOT)/auto/generate_test_runner.rb $< $@

clean:
	$(CLEANUP) $(TARGET) $(RUNNER_SRCS) $(BUILD_DIR)

#ci: CFLAGS += -Werror
#ci: default

.PHONY: setup default
