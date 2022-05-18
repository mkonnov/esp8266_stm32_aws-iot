#include <string.h>
#include <stdio.h>

#include "esp_log.h"

#include "app_uart.h"

#include "stm32_command_handlers.h"
#include "uart_command_protocol.h"

#include "common_defs.h"

#define UART_NUM 0

int command_stm32_open(const uint8_t *data, size_t len) {
        char dbg_buf[64] = {0};
        uint8_t uart_buf[32] = {0};
        size_t uart_len;
        for (int i = 0; i < len; i++) {
                sprintf(dbg_buf + strlen(dbg_buf), "0x%X ", data[i]);
        }
        ESP_LOGI("open", "%s", dbg_buf);
        uart_command_protocol_packet_t pkt = {
                .command_code = COMMAND_OPEN_CODE,
                .data_len = len,
                .data = data
        };

        uart_len = uart_command_protocol_construct_payload(&pkt, uart_buf, sizeof(uart_buf));
        app_uart_write_bytes(UART_NUM, (char*)uart_buf, (size_t)uart_len);

        return 0;
}


int command_stm32_close(const uint8_t *data, size_t len) {
        char dbg_buf[64] = {0};
        uint8_t uart_buf[32] = {0};
        size_t uart_len;
        for (int i = 0; i < len; i++) {
                sprintf(dbg_buf + strlen(dbg_buf), "0x%X ", data[i]);
        }
        ESP_LOGI("close", "%s", dbg_buf);
        uart_command_protocol_packet_t pkt = {
                .command_code = COMMAND_CLOSE_CODE,
                .data_len = len,
                .data = data
        };

        uart_len = uart_command_protocol_construct_payload(&pkt, uart_buf, sizeof(uart_buf));
        app_uart_write_bytes(UART_NUM, (char*)uart_buf, (size_t)uart_len);

        return 0;
}

