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

all: test_mqtt_client

test_mqtt_client:
	$(MAKE) -C $@

.PHONY: test_mqtt_client
