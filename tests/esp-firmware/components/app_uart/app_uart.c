#include "FreeRTOS.h"

#include "driver/uart.h"

#define BUF_SIZE (256)

void app_uart_init(uint8_t uart_num, uint32_t baud_rate) {

        uart_config_t uart_config = {
                .baud_rate = baud_rate,
                .data_bits = UART_DATA_8_BITS,
                .parity    = UART_PARITY_DISABLE,
                .stop_bits = UART_STOP_BITS_1,
                .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
        };

        uart_wait_tx_done(uart_num, 5000 / portTICK_PERIOD_MS);

        uart_driver_delete(uart_num);

        uart_param_config(uart_num, &uart_config);
        uart_driver_install(uart_num, BUF_SIZE * 2, 0, 0, NULL, 0);
}

int app_uart_read_bytes(uint8_t uart_num, uint8_t *buf, uint32_t length, unsigned long ticks_to_wait) {
        return uart_read_bytes(uart_num, buf, length, ticks_to_wait);
}

int app_uart_write_bytes(uint8_t uart_num, const char *src, size_t size) {
        return  uart_write_bytes(uart_num, src, size);
}
