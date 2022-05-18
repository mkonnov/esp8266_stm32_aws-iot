#include <string.h>

#include "aws_iot_mqtt_task_payload_handlers.h"
#include "mock_stm32_command_handlers.h"

#include "unity.h"
#include "unity_fixture.h"

/*
 * hdlr_open
 */
void test_command_open_correct_payload_should_return_success(void) {
        char payload[] = "{\"data\": [0, 1, 0, 0, 1, 0, 0, 1]}";
        uint8_t args[] = {0,1,0,0,1,0,0,1};

        command_stm32_open_ExpectAndReturn(args, 8, 0);
        TEST_ASSERT_EQUAL(0, hdlr_open(payload, strlen(payload), NULL));
}

void test_command_open_excessive_elements_should_fail(void) {
        char payload[] = "{\"data\": [0, 1, 0, 0, 1, 0, 0, 1, 1, 0, 0]}";
        int ret = hdlr_open(payload, strlen(payload), NULL);
        TEST_ASSERT_EQUAL(-2, ret);
}

void test_command_open_missed_elements_should_fail(void) {
        char payload[] = "{\"data\": [0, 1, 0, 0, 1, 0]}";
        int ret = hdlr_open(payload, strlen(payload), NULL);
        TEST_ASSERT_EQUAL(-2, ret);
}

void test_command_open_incorrect_payload_1_should_fail(void) {
        char payload[] = "{\"data\": [0, 1, 0, 0, 1, 0";
        int ret = hdlr_open(payload, strlen(payload), NULL);
        TEST_ASSERT_EQUAL(-1, ret);
}

void test_command_open_incorrect_payload_2_should_fail(void) {
        char payload[] = "{\"data\": [0, 1, 0, 0, 1, 0}";
        int ret = hdlr_open(payload, strlen(payload), NULL);
        TEST_ASSERT_EQUAL(-1, ret);
}


/*
 * hdlr_close
 */
void test_command_close_correct_payload_should_return_success(void) {
        char payload[] = "{\"data\": [0, 1, 0, 0, 1, 0, 0, 1]}";
        uint8_t args[] = {0,1,0,0,1,0,0,1};

        command_stm32_close_ExpectAndReturn(args, 8, 0);
        TEST_ASSERT_EQUAL(0, hdlr_close(payload, strlen(payload), NULL));
}

void test_command_close_excessive_elements_should_fail(void) {
        char payload[] = "{\"data\": [0, 1, 0, 0, 1, 0, 0, 1, 1, 0, 0]}";
        int ret = hdlr_close(payload, strlen(payload), NULL);
        TEST_ASSERT_EQUAL(-2, ret);
}

void test_command_close_missed_elements_should_fail(void) {
        char payload[] = "{\"data\": [0, 1, 0, 0, 1, 0]}";
        int ret = hdlr_close(payload, strlen(payload), NULL);
        TEST_ASSERT_EQUAL(-2, ret);
}

void test_command_close_incorrect_payload_1_should_fail(void) {
        char payload[] = "{\"data\": [0, 1, 0, 0, 1, 0";
        int ret = hdlr_close(payload, strlen(payload), NULL);
        TEST_ASSERT_EQUAL(-1, ret);
}

void test_command_close_incorrect_payload_2_should_fail(void) {
        char payload[] = "{\"data\": [0, 1, 0, 0, 1, 0}";
        int ret = hdlr_close(payload, strlen(payload), NULL);
        TEST_ASSERT_EQUAL(-1, ret);
}
