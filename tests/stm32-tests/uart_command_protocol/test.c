#include "unity.h"
#include "uart_command_protocol.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "crc16.h"

void setUp(void) {
        uart_command_protocol_register_crc16_calc_function(crc16);
}

void tearDown(void) {}

static bool called_hdlr_no_data = false;
static bool called_hdlr_with_data = false;
static bool called_hdlr_should_not_be_called = false;

static void handler_no_data(uint16_t data_len, const uint8_t *data) {
        (void) data;
        TEST_ASSERT_EQUAL(0, data_len);
}

static void handler_with_data(uint16_t data_len, const uint8_t *data) {
        uint8_t expected[] = {0x01,0x02,0x03,0x04};
        called_hdlr_with_data = true;
        TEST_ASSERT_EQUAL(4, data_len);
        TEST_ASSERT_EQUAL_UINT8_ARRAY(expected, data, data_len);
}

static void handler_should_not_be_called(uint16_t data_len, const uint8_t *data) { 
        (void) data_len;
        (void) data;
        called_hdlr_should_not_be_called = true;
}

void test_receive_command_no_data(void) {
        uint8_t payload[] = {
                0xa5, //start
                0x01, //command
                0x00, //len upper
                0x00, //len lower
                0xfb, //crc upper
                0xac, //crc lower
        };

        called_hdlr_no_data = true;

        uart_command_protocol_register_cb(0x01, handler_no_data);

        for (uint32_t i = 0; i < sizeof(payload); i++) {
                uart_command_protocol_process_byte(payload[i]);
        }
        TEST_ASSERT_EQUAL(true, called_hdlr_no_data);
}

void test_receive_command_with_data(void) {
        uint8_t payload[] = {
                0xa5, //start
                0x02, //command
                0x00, //len upper
                0x04, //len lower
                0x01,
                0x02,
                0x03,
                0x04,
                0x9a, //crc upper
                0x8f, //crc lower
        };

        uart_command_protocol_register_cb(0x02, handler_with_data);

        for (uint32_t i = 0; i < sizeof(payload); i++) {
                uart_command_protocol_process_byte(payload[i]);
        }
        TEST_ASSERT_EQUAL(true, called_hdlr_with_data);
}

void test_handler_should_not_be_called_wrong_crc(void) {
        uint8_t payload[] = {
                0xa5, //start
                0x01, //command
                0x00, //len upper
                0x00, //len lower
                0x01, //crc upper
                0x00, //crc lower
        };

        uart_command_protocol_register_cb(0x01, handler_should_not_be_called);

        for (uint32_t i = 0; i < sizeof(payload); i++) {
                uart_command_protocol_process_byte(payload[i]);
        }

        TEST_ASSERT_EQUAL(false, called_hdlr_should_not_be_called);
}

void test_construct_payload_no_data_should_succeed_and_payload_match_expected(void) {
        uint8_t out_buf[32] = {0};
        size_t s;
        uart_command_protocol_packet_t pkt = {
                .command_code = 0x01,
                .data_len = 0,
                .data = NULL
        };
        s = uart_command_protocol_construct_payload(&pkt, out_buf, 32);
        uint8_t expected[] = {0xA5, 0x1, 0x0, 0x0, 0xFB, 0xAC};
        TEST_ASSERT_EQUAL(6, s);
        TEST_ASSERT_EQUAL_UINT8_ARRAY(expected, out_buf, s);
}

void test_construct_payload_with_data_should_succeed_and_payload_match_expected(void) {
        uint8_t out_buf[32] = {0};
        uint8_t in_buf[] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07};
        size_t s;
        uart_command_protocol_packet_t pkt = {
                .command_code = 0x01,
                .data_len = sizeof(in_buf),
                .data = in_buf
        };
        s = uart_command_protocol_construct_payload(&pkt, out_buf, 32);

        uint8_t expected[] = {0xa5, 0x1, 0x0, 0x7, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x19, 0x64};
        TEST_ASSERT_EQUAL(13, s);
        TEST_ASSERT_EQUAL_UINT8_ARRAY(expected, out_buf, s);
}

void test_construct_payload_with_data_should_not_succeed_out_buf_size_less_than_required(void) {
        uint8_t out_buf[4] = {0};
        uint8_t in_buf[] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07};
        size_t s;
        uart_command_protocol_packet_t pkt = {
                .command_code = 0x01,
                .data_len = sizeof(in_buf),
                .data = in_buf
        };
        s = uart_command_protocol_construct_payload(&pkt, out_buf, sizeof(out_buf));
        TEST_ASSERT_EQUAL(-1, s);
}

