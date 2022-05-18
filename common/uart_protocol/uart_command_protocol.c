#include <string.h>
#include <stdio.h>

#include "uart_command_protocol.h"
#include "crc16.h"

#ifdef UART_COMMAND_PROTOCOL_DBG
        #define DBG_PRINTF(fmt, ...) printf((fmt), __VA_ARGS__)

#else
        #define DBG_PRINTF(fmt, ...)
#endif /* UART_COMMAND_PROTOCOL_DBG */

/*
 * Finite state machine state codes.
 * They represents the state that FSM _already_ reached.
 * E,g, UART_PROTOCOL_STATE_COMMAND indicates that 
 * machine has received 'command' byte and at the moment
 * the 'len' bytes should be processed.
 */
typedef enum {
        UART_PROTOCOL_STATE_IDLE = 0, // No controlling codes received
        UART_PROTOCOL_STATE_START, // Start byte received
        UART_PROTOCOL_STATE_COMMAND, // Command code received and validated
        UART_PROTOCOL_STATE_DATA_LEN_UPPER, // Data length upper byte received
        UART_PROTOCOL_STATE_DATA_LEN_LOWER, // Data length lower byte received, length validated
        UART_PROTOCOL_STATE_DATA, // Data bytes
        UART_PROTOCOL_STATE_CRC_UPPER, // CRC upper byte received
        UART_PROTOCOL_STATE_CRC_LOWER, // CRC lower byte received, CRC validated
} uart_protocol_state_t;


/*
 * The finite state machine state descriptor.
 */
struct {
        /* Current state of state machine */
        uart_protocol_state_t state;    
        /* Received command code */
        uint8_t command;                
        /* Number of bytes to be copied to 'data' array */
        uint16_t data_length;           
        /* The last saved index of data array */
        uint16_t data_idx;              
        /* Data array to store the received bytes */
        uint8_t data[DATA_LEN_MAX];     
        /* CRC received */
        uint16_t crc;

        /*
         * Callbacks that's to be called on the receprion and validation of the whole packet.
         * Before being called the callback should be registered against the corresponding 
         * command code.
         */
        struct {
                uint8_t command_code;
                uart_protocol_command_cb_t cb;
        } callbacks[CALLBACKS_NUM_MAX];
} state_d = {
        .state = UART_PROTOCOL_STATE_IDLE,
        .data_length = 0,
        .data_idx = 0,
        .data = {0},
        .callbacks = {},
};

int uart_command_protocol_register_cb(uint8_t command_code, uart_protocol_command_cb_t cb) {
        /* It should either write the callback to the first free slot or
         * rewrite the one that's already occupied with the same command code.
         */
        int16_t free_idx = -1;
        int16_t occ_idx = -1;
        int ret = -1;
        for (int i = 0; i < CALLBACKS_NUM_MAX; i++) {
                if (state_d.callbacks[i].cb == NULL) {
                        free_idx = i;
                }
                if (state_d.callbacks[i].command_code == command_code) {
                        occ_idx = i;
                }
        }
        if (occ_idx != -1) {
                state_d.callbacks[occ_idx].cb = cb;
                state_d.callbacks[occ_idx].command_code = command_code;
                ret = 0;
                goto out;
        }
        if (free_idx != -1) {
                state_d.callbacks[free_idx].cb = cb;
                state_d.callbacks[free_idx].command_code = command_code;
                ret = 0;
                goto out;
        }
out:
        return ret;
}

int uart_command_protocol_deregister_cb(uint8_t command_code) {
        int ret = -1;
        for (int i = 0; i < CALLBACKS_NUM_MAX; i++) {
                if (state_d.callbacks[i].command_code == command_code) {
                        state_d.callbacks[i].command_code = 0;
                        state_d.callbacks[i].cb = NULL;
                        ret = 0;
                        goto out;
                }
        }
out:
        return ret;
}

void uart_command_protocol_reset_state(void) {
        state_d.state = UART_PROTOCOL_STATE_IDLE;
        state_d.data_length = 0;
        state_d.data_idx = 0;
        memset(state_d.data, 0, DATA_LEN_MAX);
}

#ifdef UART_COMMAND_PROTOCOL_DBG
static void __state_num_to_str(uint8_t state_num, char *out_buf) {
        switch (state_num) {
        
        case UART_PROTOCOL_STATE_IDLE:
                sprintf(out_buf, "IDLE");
                break;
        case UART_PROTOCOL_STATE_START:
                sprintf(out_buf, "START");
                break;
        case UART_PROTOCOL_STATE_COMMAND:
                sprintf(out_buf, "COMMAND");
                break;
        case UART_PROTOCOL_STATE_DATA_LEN_UPPER:
                sprintf(out_buf, "LEN_UPPER");
                break;
        case UART_PROTOCOL_STATE_DATA_LEN_LOWER:
                sprintf(out_buf, "LEN_LOWER");
                break;
        case UART_PROTOCOL_STATE_DATA:
                sprintf(out_buf, "DATA");
                break;
        case UART_PROTOCOL_STATE_CRC_UPPER:
                sprintf(out_buf, "CRC_UPPER");
                break;
        case UART_PROTOCOL_STATE_CRC_LOWER:
                sprintf(out_buf, "CRC_LOWER");
                break;
        default:
                break;
        }
}

static inline void __dbg_print_state(void) {
        char buf[DATA_LEN_MAX];
        memset(buf, 0, DATA_LEN_MAX);
        __state_num_to_str(state_d.state, buf);
        DBG_PRINTF("state = %s\n", buf);
        DBG_PRINTF("command = %d\n", state_d.command);
        DBG_PRINTF("data_length = 0x%X\n", state_d.data_length);
        memset(buf, 0, DATA_LEN_MAX);
        for (uint16_t i = 0; i < state_d.data_length; i++) {
                sprintf(buf + strlen(buf), "0x%X ", state_d.data[i]);
        }
        DBG_PRINTF("data: %s\n", buf);
        DBG_PRINTF("crc: %X\n\n", state_d.crc);
}
#else 
static inline void __dbg_print_state(void) {}
#endif /* UART_COMMAND_PROTOCOL_DBG */

/**
 * __calculate_crc - wrapper around the actual CRC16 implementation, just for сonvenience.
 * @command     - the command code
 * @len         - data length
 * @data        - pointer to data
 */
static uint16_t __calculate_crc(uint8_t command, uint16_t len, const uint8_t *data) {
        uint16_t crc = 0xffff;
        crc = crc16(crc, 1, &command);
        crc = crc16(crc, 2, (uint8_t *)&len);
        crc = crc16(crc, len, data);
        return crc;
}

static uart_protocol_command_cb_t __get_callback(uint8_t command_code) {
        uart_protocol_command_cb_t cb = NULL;
        for (int i = 0; i < CALLBACKS_NUM_MAX; i++) {
                if (state_d.callbacks[i].command_code == command_code) {
                        cb = state_d.callbacks[i].cb;
                        break;
                }
        }
        return cb;
}

static size_t __construct_payload_internal(uint8_t command_code, const uint8_t *data, uint16_t len, uint8_t *out, size_t out_buf_len) {
        size_t ret;
        uint16_t idx = 0;
        uint16_t crc_calculated;
        /**
         * Take into account start byte, command code, 2 len bytes and 2 CRC bytes.
         */
        if ((size_t)(len + 6) > out_buf_len) {
                DBG_PRINTF("%s: %d\n", __func__, __LINE__);
                ret = -1;
                goto out;
        }
        DBG_PRINTF("%s: %d\n", __func__, __LINE__);

        memset(out, 0, out_buf_len);
        out[idx++] = 0xA5;
        out[idx++] = command_code;
        out[idx++] = (len >> 8) & 0xff;
        out[idx++] = len & 0xff;
        if (len > 0) {
                DBG_PRINTF("%s: %d\n", __func__, __LINE__);
                if (out != NULL) {
                        DBG_PRINTF("%s: %d\n", __func__, __LINE__);
                        memcpy(out + idx, data, len);
                }
        }

        idx += len;

        crc_calculated = __calculate_crc(command_code, len, data);

        out[idx++] = (crc_calculated >> 8) & 0xff ; //CRC16 upper byte
        out[idx++] = crc_calculated & 0xff; //CRC16 lower byte
        ret = idx;
        DBG_PRINTF("%s: %d\n", __func__, __LINE__);
out:
        return ret;
}


void uart_command_protocol_process_byte(uint8_t c) {
        __dbg_print_state();
        switch(state_d.state) {
        case UART_PROTOCOL_STATE_IDLE:
                DBG_PRINTF("%s: %d\n", __func__, __LINE__);
                /*
                 * Just handle the reception of a start byte
                 */
                if (c == 0xA5) {
                        state_d.state = UART_PROTOCOL_STATE_START;
                        state_d.data_length = 0;
                        state_d.data_idx = 0;
                        memset(state_d.data, 0, DATA_LEN_MAX);
                }
                break;
        case UART_PROTOCOL_STATE_START:
                DBG_PRINTF("%s: %d\n", __func__, __LINE__);
                state_d.command = c;
                state_d.state = UART_PROTOCOL_STATE_COMMAND;
                break;
        case UART_PROTOCOL_STATE_COMMAND:
                DBG_PRINTF("%s: %d\n", __func__, __LINE__);
                /*
                 * Copy the upper length byte
                 */
                state_d.data_length = (c << 8) & 0xff00;
                state_d.state = UART_PROTOCOL_STATE_DATA_LEN_UPPER;
                break;
        case UART_PROTOCOL_STATE_DATA_LEN_UPPER:
                DBG_PRINTF("%s: %d\n", __func__, __LINE__);
                /*
                 * Copy the lower length byte and validate the length value
                 */
                state_d.data_length |= c & 0x00ff;
                if (state_d.data_length > DATA_LEN_MAX) {
                        state_d.state = UART_PROTOCOL_STATE_IDLE;
                } else { 
                        state_d.state = UART_PROTOCOL_STATE_DATA_LEN_LOWER;
                }
                break;
        case UART_PROTOCOL_STATE_DATA_LEN_LOWER:
                DBG_PRINTF("%s: %d\n", __func__, __LINE__);
                if (state_d.data_length > 0) {
                        DBG_PRINTF("%s: %d\n", __func__, __LINE__);
                        state_d.state = UART_PROTOCOL_STATE_DATA;
                } else {
                        DBG_PRINTF("%s: %d\n", __func__, __LINE__);
                        state_d.state = UART_PROTOCOL_STATE_CRC_UPPER;
                }
                __attribute__((fallthrough));
        case UART_PROTOCOL_STATE_DATA:
                DBG_PRINTF("%s: %d\n", __func__, __LINE__);
                if (state_d.data_idx == state_d.data_length) {
                        /*
                         * All data bytes is received and the incoming byte
                         * is a CRC upper byte
                         */
                        DBG_PRINTF("%s: %d\n", __func__, __LINE__);
                        DBG_PRINTF("c = 0x%X\n", c);
                        state_d.crc = (c << 8) & 0xff00;
                        state_d.state = UART_PROTOCOL_STATE_CRC_UPPER;
                } else {
                        /*
                         * Copy the received byte
                         */
                        DBG_PRINTF("%s: %d\n", __func__, __LINE__);
                        state_d.data[state_d.data_idx++] = c;
                }
                break;
        case UART_PROTOCOL_STATE_CRC_UPPER:
                DBG_PRINTF("%s: %d\n", __func__, __LINE__);
                state_d.crc |= ((uint16_t) c & 0x00ff);
                state_d.state = UART_PROTOCOL_STATE_CRC_LOWER;
                __attribute__((fallthrough));
        case UART_PROTOCOL_STATE_CRC_LOWER:
                DBG_PRINTF("%s: %d\n", __func__, __LINE__);
                uint16_t crc_calculated;
                uart_protocol_command_cb_t cb;

                crc_calculated = __calculate_crc(state_d.command, state_d.data_length, state_d.data);
                DBG_PRINTF("crc calc: 0x%X\n", crc_calculated);
                DBG_PRINTF("crc recv: 0x%X\n", state_d.crc);

                if (state_d.crc == crc_calculated) {
                        cb = __get_callback(state_d.command);
                        if (cb != NULL) {
                                cb(state_d.data_length, state_d.data);
                        }
                }

                state_d.state = UART_PROTOCOL_STATE_IDLE;
                break;
        default:
                DBG_PRINTF("%s: %d\n", __func__, __LINE__);
                break;
        }
}

size_t uart_command_protocol_construct_payload(uart_command_protocol_packet_t *pkt, 
                uint8_t *out, size_t out_buf_len) {
        return __construct_payload_internal(pkt->command_code, 
                        pkt->data, pkt->data_len, out, out_buf_len);
}
