#include "unity.h"
#include "uart_command_protocol.h"
#include <stdio.h>
#include <string.h>

void setUp(void) {}

void tearDown(void) {}

void test_payload_should_match_no_data(void) {

        uint8_t out_buf[32] = {0};
        size_t len;

	uart_command_protocol_packet_t pkt = {
                .command_code = 0x01,
                .data_len = 0,
                .data = NULL
	};

        len = uart_command_protocol_construct_payload(&pkt, out_buf, 32);

        TEST_ASSERT_EQUAL_INT(6, len);

        uint8_t exp[] = {0xA5, 0x01, 0x00, 0x00, 0xFB, 0xAC};

        TEST_ASSERT_EQUAL_UINT8_ARRAY(exp, out_buf, len);
}

void test_payload_should_match_with_data(void) {
        uint8_t out_buf[32] = {0};
        size_t len;

        uint8_t data[] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07};

	uart_command_protocol_packet_t pkt = {
                .command_code = 0x01,
                .data_len = 8,
                .data = data
        };

        len = uart_command_protocol_construct_payload(&pkt, out_buf, 32);

        TEST_ASSERT_EQUAL_INT(14, len);

        uint8_t exp[] = {0xA5, 0x01, 0x00, 0x08, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x60, 0xBE};
        TEST_ASSERT_EQUAL_UINT8_ARRAY(exp, out_buf, len);
}

void test_payload_should_not_be_generated_buffer_overfill(void) {
        size_t len;

        uint8_t out_buf[8] = {0};

        uint8_t data[] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07};

	uart_command_protocol_packet_t pkt = {
                .command_code = 0x01,
                .data_len = 32,
                .data = data
        };

        len = uart_command_protocol_construct_payload(&pkt, out_buf, 32);

        TEST_ASSERT_EQUAL_INT(-1, len);
}

/**
void test_callback_should_be_registered_without_being_registered_yet(void) {
        int ret = uart_command_protocol_register_cb(0x00, common_cb);
        TEST_ASSERT_EQUAL_INT(0, ret);
}

void test_callback_should_be_registered_being_already_registered(void) {
        uart_command_protocol_register_cb(0x00, common_cb);

        int ret = uart_command_protocol_register_cb(0x00, common_cb);
        TEST_ASSERT_EQUAL_INT(0, ret);
}

void test_callback_should_not_be_registered_max_num_reached(void) {
        int i;
        for (i = 0; i < CALLBACKS_NUM_MAX; i++) {
                uart_command_protocol_register_cb(i, common_cb);
        }

        int ret = uart_command_protocol_register_cb(i+1, common_cb);
        TEST_ASSERT_EQUAL_INT(-1, ret);
}

void test_registered_callback_should_be_called(void) {

        uint8_t out_buf[32] = {0};
        uart_command_protocol_register_cb(0x00, common_cb);

	uart_command_protocol_packet_t pkt = {
                .command_code = 0x00,
                .data_len = 0,
                .data = NULL
        };

        size_t len = uart_command_protocol_construct_payload(&pkt, out_buf, 32);
        
        callback_0_Expect(0, NULL);

        for (int i = 0; i < (int)len; i++) {
                uart_command_protocol_process_byte(out_buf[i]);
        }
}
*/
