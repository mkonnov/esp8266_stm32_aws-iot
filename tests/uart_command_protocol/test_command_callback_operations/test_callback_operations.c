#include "unity.h"
#include "uart_command_protocol.h"
#include "build/mock_callbacks.h"
#include <stdio.h>
#include <string.h>

struct state_struct {
        uint32_t state;    
        uint8_t command;                
        uint16_t data_length;           
        uint16_t data_idx;              
        uint8_t data[DATA_LEN_MAX];     
        uint16_t crc;

        struct {
                uint8_t command_code;
                void (*cb)(uint16_t data_len, const uint8_t *data);
        } callbacks[CALLBACKS_NUM_MAX];
};
extern struct state_struct state_d;


void setUp(void) {}

void tearDown(void) {}

void test_callback_should_be_registered_without_being_registered_yet(void) {
        int ret = uart_command_protocol_register_cb(0x00, callback_0);
        TEST_ASSERT_EQUAL_INT(0, ret);
}

void test_callback_should_be_registered_being_already_registered(void) {
        uart_command_protocol_register_cb(0x00, callback_0);

        int ret = uart_command_protocol_register_cb(0x00, callback_0);
        TEST_ASSERT_EQUAL_INT(0, ret);
}

void test_callback_should_not_be_registered_max_num_reached(void) {
        int i;
        for (i = 0; i < CALLBACKS_NUM_MAX; i++) {
                uart_command_protocol_register_cb(i, callback_0);
        }

        int ret = uart_command_protocol_register_cb(i+1, callback_0);
        TEST_ASSERT_EQUAL_INT(-1, ret);
}

void test_callback_registered_should_be_called_no_data(void) {
        uint8_t out_buf[32] = {0};
        uart_command_protocol_packet_t pkt = {
                .command_code = 0x00,
                .data_len = 0,
                .data = NULL
        };

        size_t len = uart_command_protocol_construct_payload(&pkt, out_buf, sizeof(out_buf));

        uart_command_protocol_register_cb(0x00, callback_0);

        callback_0_Expect(0, state_d.data);

        for (int i = 0; i < (int) len; i++) {
                uart_command_protocol_process_byte(out_buf[i]);
        }
}

void test_callback_registered_should_be_called_with_data(void) {
        uint8_t out_buf[32] = {0};
        char data[] = "some_data";

        uart_command_protocol_packet_t pkt = {
                .command_code = 0x00,
                .data_len = sizeof(data),
                .data = (uint8_t*)data
        };

        size_t len = uart_command_protocol_construct_payload(&pkt, out_buf, sizeof(out_buf));

        uart_command_protocol_register_cb(0x00, callback_0);

        callback_0_ExpectWithArray(sizeof(data), (uint8_t*)data, sizeof(data));

        for (int i = 0; i < (int) len; i++) {
                uart_command_protocol_process_byte(out_buf[i]);
        }
}

void test_callback_not_registered_should_not_be_called(void) {
        uint8_t out_buf[32] = {0};
        char data[] = "some_data";

        uart_command_protocol_packet_t pkt = {
                .command_code = 0x00,
                .data_len = sizeof(data),
                .data = (uint8_t*)data
        };

        size_t len = uart_command_protocol_construct_payload(&pkt, out_buf, sizeof(out_buf));

        uart_command_protocol_deregister_cb(0x00);

        for (int i = 0; i < (int) len; i++) {
                uart_command_protocol_process_byte(out_buf[i]);
        }
}
