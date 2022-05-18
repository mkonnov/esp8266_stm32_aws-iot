#ifndef APP_UART_H
#define APP_UART_H

void app_uart_init(uint8_t uart_num, uint32_t baud_rate);
int app_uart_read_bytes(uint8_t uart_num, uint8_t *buf, uint32_t length, unsigned long ticks_to_wait);
int app_uart_write_bytes(uint8_t uart_num, const char *src, size_t size);

#endif /* APP_UART_H */
