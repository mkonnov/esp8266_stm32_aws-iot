#ifndef TASK_UART_RECEIVER
#define TASK_UART_RECEIVER

void task_uart_receiver(void *arg);
void task_uart_receiver_queue_putc_IRQ(uint8_t c);


#endif /* TASK_UART_RECEIVER */
