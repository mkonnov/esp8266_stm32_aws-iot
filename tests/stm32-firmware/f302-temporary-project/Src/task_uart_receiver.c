#include <stdio.h>
#include <string.h>

#include "FreeRTOS.h"
#include "queue.h"
#include "timers.h"

#include "uart_command_protocol.h"
#include "common_defs.h"

#include "main.h"

#define WATCHDOG_PERIOD_MS 1000

static QueueHandle_t char_queue = NULL;

extern UART_HandleTypeDef huart2;

void task_uart_receiver_queue_putc_IRQ(uint8_t c) {


	BaseType_t xHigherPriorityTaskWoken = pdFALSE;

        if (char_queue == NULL)
                return;

        xQueueSendFromISR(char_queue, &c, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

static void handler_command_open(uint16_t data_len, const uint8_t *data) {
        char dbg_buf[64] = {0};
        sprintf((char*)dbg_buf, "OPEN: ");
        for (uint16_t i = 0; i < data_len; i++) {
                sprintf(dbg_buf + strlen(dbg_buf), "0x%X ", data[i]);
        }
        sprintf(dbg_buf + strlen(dbg_buf), "\n");

        HAL_UART_Transmit(&huart2, (uint8_t*)dbg_buf, strlen(dbg_buf), 1000);
}

static void handler_command_close(uint16_t data_len, const uint8_t *data) {
        char dbg_buf[64] = {0};
        sprintf((char*)dbg_buf, "CLOSE: ");
        for (uint16_t i = 0; i < data_len; i++) {
                sprintf(dbg_buf + strlen(dbg_buf), "0x%X ", data[i]);
        }
        sprintf(dbg_buf + strlen(dbg_buf), "\n");

        HAL_UART_Transmit(&huart2, (uint8_t*)dbg_buf, strlen(dbg_buf), 1000);
}


static void watchdog_timer_cb(TimerHandle_t xTimer) {
        (void) xTimer;
        uart_command_protocol_reset_state();
}

void task_uart_receiver(void *arg) {

        uint8_t c;
        char_queue = xQueueCreate(128, 1); 
        TimerHandle_t watchdog_timer = xTimerCreate(
           "uart_wdg",
           pdMS_TO_TICKS(WATCHDOG_PERIOD_MS), 
           pdFALSE,
           NULL,
           watchdog_timer_cb);

        uart_command_protocol_register_cb(COMMAND_OPEN_CODE, handler_command_open);
        uart_command_protocol_register_cb(COMMAND_CLOSE_CODE, handler_command_close);

        for (;;) {
                /**
                 * Receive the byte from UART
                 */
                xQueueReceive(char_queue, &c, portMAX_DELAY); 

                /**
                 * Kick the watchdog timer
                 */
                xTimerReset(watchdog_timer, portMAX_DELAY);
                uart_command_protocol_process_byte(c);
        }
}

