#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"
#include "uart_command_protocol.h"
#include "common_defs.h"

#include "driver/uart.h"

#define BUF_SIZE (256)

#define UART_NUM 0

static uint8_t rx_data[BUF_SIZE];

void uart_rx_task(void *arg) {
        int len;
        int idx;

        uart_config_t uart_config = {
                .baud_rate = UART_BAUD_RATE,
                .data_bits = UART_DATA_8_BITS,
                .parity    = UART_PARITY_DISABLE,
                .stop_bits = UART_STOP_BITS_1,
                .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
        };

        uart_wait_tx_done(UART_NUM, 5000 / portTICK_PERIOD_MS);

        uart_driver_delete(UART_NUM);

        uart_param_config(UART_NUM, &uart_config);
        uart_driver_install(UART_NUM, BUF_SIZE * 2, 0, 0, NULL, 0);

        while (true) {
                len = uart_read_bytes(UART_NUM, rx_data, BUF_SIZE, 20 / portTICK_RATE_MS);
                for (idx = 0; idx != len; idx++) {
                        uart_command_protocol_process_byte(rx_data[idx]);
                }
        }
}
