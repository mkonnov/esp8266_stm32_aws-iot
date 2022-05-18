TARGET = $(BUILD_DIR)/$(TEST_NAME)$(TARGET_EXTENSION)

BUILD_DIR = build

RUNNER_SRCS = $(patsubst %.c,$(BUILD_DIR)/%_runner.c,$(TEST_SRCS))
MOCK_SRCS = $(patsubst %.h, $(BUILD_DIR)/mock_%.c, $(notdir $(INCLUDES_TO_MOCK)))


INC_DIRS += -I $(BUILD_DIR)

SRCS += $(APP_SRCS) $(TEST_SRCS) $(RUNNER_SRCS) $(MOCK_SRCS)

$(MOCK_SRCS): $(INCLUDES_TO_MOCK)
	mkdir -p $(BUILD_DIR)
ifneq ($(INCLUDES_TO_MOCK),)
	ruby $(CMOCK_ROOT)/lib/cmock.rb -oconfig.yml $(INCLUDES_TO_MOCK)
endif # INCLUDES_TO_MOCK

$(RUNNER_SRCS): $(TEST_SRCS)
	mkdir -p $(BUILD_DIR)
	ruby $(UNITY_ROOT)/auto/generate_test_runner.rb $< $@

all: test clean

test: $(SRCS)
	@echo RUNNING $(TEST_NAME)...
	$(C_COMPILER) $(CFLAGS) $(INC_DIRS) $(SYMBOLS) $(SRCS) -o $(TARGET)
	- ./$(TARGET)

clean:
	$(CLEANUP) $(BUILD_DIR)

.PHONY: test
.DEFAULT_GOAL := all

