#ifndef COMMON_DEFS_H
#define COMMON_DEFS_H

/*
 * UART serial connection defs
 */
#define UART_BAUD_RATE 38400

/* 
 * UART Comand Protocol defs
 */
#define COMMAND_OPEN_CODE 0x01
#define COMMAND_CLOSE_CODE 0x02
#define COMMAND_WPS_BEGIN_CODE 0x03
#define COMMAND_DIRECT_WIFI_CREDS_CODE 0x04


/*
 * STM32 button behavior defs. Mutually exclusive.
 */
#define STM32_BUTTON_SEND_DIRECT_CREDS_COMMAND
//#define STM32_BUTTON_SEND_WPS_COMMAND



#if defined(STM32_BUTTON_SEND_DIRECT_CREDS_COMMAND) && defined(STM32_BUTTON_SEND_WPS_COMMAND)
#error "Symbol STM32_BUTTON_SEND_DIRECT_CREDS_COMMAND or STM32_BUTTON_SEND_WPS_COMMAND should be defined"
#endif

/*
 * Direct WiFi creds
 */
struct wifi_creds {
        uint8_t ssid_len;
        uint8_t password_len;
        uint8_t ssid[32];
        uint8_t password[64];
};

#define WIFI_SSID "ssid"
#define WIFI_PASSWORD "passwd"

#endif /* COMMON_DEFS_H */
