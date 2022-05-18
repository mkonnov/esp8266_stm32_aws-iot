#include <string.h>
#include <stdbool.h>

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#include "main.h"

#include "uart_command_protocol.h"
#include "common_defs.h"

#include <stdio.h>

#define UART_BUF_SIZE 128

#define BUTTON_PRESSED_STATE GPIO_PIN_RESET

extern UART_HandleTypeDef huart3, huart2;

static uint8_t uart_buf[UART_BUF_SIZE];

SemaphoreHandle_t semphr_button_pressed;

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;

        xSemaphoreGiveFromISR(semphr_button_pressed, &xHigherPriorityTaskWoken);
        (void)xHigherPriorityTaskWoken;
}


#if 0
/*
 * For payload debug
 */
static void hexdump(char *out_buf, size_t buf_len, const uint8_t *buf) {
        for (int i = 0; i < buf_len; i++) {
                if (i % 16 == 0) {
                        sprintf(out_buf + strlen(out_buf), "\n");
                }
                sprintf(out_buf + strlen(out_buf), "%.2X ", (int)buf[i]);
        }
}
#endif

void task_button(void *arg) {

        semphr_button_pressed = xSemaphoreCreateBinary();

#if defined(STM32_BUTTON_SEND_WPS_COMMAND)
        uart_command_protocol_packet_t pkt = {
                .command_code = COMMAND_WPS_BEGIN_CODE ,
                .data_len = 0,
                .data = NULL
        };
#elif defined(STM32_BUTTON_SEND_DIRECT_CREDS_COMMAND)

        struct wifi_creds wifi_creds;
        wifi_creds.ssid_len = strlen(WIFI_SSID);
        wifi_creds.password_len = strlen(WIFI_PASSWORD);
        memset(wifi_creds.ssid, 0, sizeof(wifi_creds.ssid));
        memset(wifi_creds.password, 0, sizeof(wifi_creds.password));
        strcpy((char*)wifi_creds.ssid, WIFI_SSID);
        strcpy((char*)wifi_creds.password, WIFI_PASSWORD);

        uart_command_protocol_packet_t pkt = {
                .command_code = COMMAND_DIRECT_WIFI_CREDS_CODE,
                .data_len = sizeof(wifi_creds),
                .data = (const uint8_t *)&wifi_creds
        };

#endif /* STM32_BUTTON_SEND_WPS_COMMAND | STM32_BUTTON_SEND_DIRECT_CREDS_COMMAND */

        size_t uart_len = uart_command_protocol_construct_payload(&pkt, uart_buf, sizeof(uart_buf));

        while(1) {
                xSemaphoreTake(semphr_button_pressed, portMAX_DELAY);
                HAL_UART_Transmit(&huart3, uart_buf, uart_len, 1000);
        }
}
