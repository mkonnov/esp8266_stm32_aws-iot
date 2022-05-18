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

.DEFAULT_GOAL := all



all: test_command_callback_operations test_payload_operations

test_payload_operations:
	$(MAKE) -C $@

test_command_callback_operations:
	$(MAKE) -C $@


.PHONY: test_payload_operations test_command_callback_operations
