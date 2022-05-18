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

all: test_common_subscription_handler test_payload_handlers

test_common_subscription_handler:
	$(MAKE) -C $@

test_payload_handlers:
	$(MAKE) -C $@


.PHONY: test_common_subscription_handler test_payload_handlers
