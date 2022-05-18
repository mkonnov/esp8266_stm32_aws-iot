#ifndef UART_COMMAND_PROTOCOL_H
#define UART_COMMAND_PROTOCOL_H

#define DATA_LEN_MAX 256
#define CALLBACKS_NUM_MAX 16

#include <stdint.h>

/**
 * User callback typedef
 */
typedef void (*uart_protocol_command_cb_t)(uint16_t data_len, const uint8_t *data);

/**
 * The structure representing the packet.
 */
typedef struct {
        uint8_t command_code;
        uint16_t data_len;
        const uint8_t *data;
} uart_command_protocol_packet_t;

size_t uart_command_protocol_construct_payload(uart_command_protocol_packet_t *pkt, 
                uint8_t *out, size_t out_buf_len);


/**
 * uart_command_protocol_register_cb - Register user callback in an UART command protocol FSM.
 *
 * @command_code:       The command code that's to be associated with a registering callback
 * @cb:                 The user callback that's to be executed after a successfull incoming
 *   packet parsing.
 *
 * Returns 0 if an empty slot for this callback has been found and reserved. Otherwize returns -1.
 * Callbacks with a same command code would be successfully registered with an old one replaced.
 */
int uart_command_protocol_register_cb(uint8_t command_code, uart_protocol_command_cb_t cb);

/**
 * uart_command_protocol_deregister_cb - Free the callback slot for a given command code.
 * @command_code:       The command code associated with a callback that's to be removed.
 *
 * Returns 0 if a callback has been successfully removed. If a callback has not been found, returns -1.
 */
int uart_command_protocol_deregister_cb(uint8_t command_code);


/**
 * uart_command_protocol_process_byte - Process the incoming data byte with a state machine.
 * @c:                  Incoming data byte.
 *
 * Processes the incoming byte according to the UART command protocol packet structure:
 * |Byte num    | Description |                    |
 * -------------+-------------+--------------------+
 * |       0    | Start byte. Should be 0xA5.      |
 * +------------+----------------------------------+
 * |       1    | Command code                     |
 * +------------+----------------------------------+
 * |       2    | Length upper byte (MSB)          |
 * +------------+----------------------------------+
 * |       3    | Length lower byte (LSB)          |
 * +------------+----------------------------------+
 * |4..(4+LEN-1)| Data bytes if length != 0.       |
 * |            | Otherwize, this field is ignored.|
 * +------------+----------------------------------+
 * |  4 + LEN   | CRC upper byte                   |
 * +------------+----------------------------------+
 * |  5 + LEN   | CRC lower byte                   |
 * +-----------------------------------------------+
 * Example payloads:
 *
 * 1. Payload to call a command with code 0x01 without of data bytes:
 * 0xa5 0x01 0x00 0x00 0x0e 0xc9
 *
 * 2. Payload to call a command with code 0x05 with 8 data bytes:
 * 0xa5 0x05 0x00 0x08 0xae 0x12 0xcd 0x90 0x11 0xf4 0xa1 0xd9 0x51 0xab
 */
void uart_command_protocol_process_byte(uint8_t c);

/**
 * uart_command_protocol_reset_state - Reset the command protocol state to the initial value.
 */
void uart_command_protocol_reset_state(void);

#endif /* UART_COMMAND_PROTOCOL_H */
