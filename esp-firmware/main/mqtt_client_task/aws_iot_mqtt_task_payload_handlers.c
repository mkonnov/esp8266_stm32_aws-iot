#include <string.h>
#include <stdlib.h>

#include "jsmn.h"
#include "aws_iot_mqtt_task_payload_handlers.h"
#include "stm32_command_handlers.h"

#include <stdio.h>

#define MAX_JSON_TOKENS 20

#define TAG "sub_hadler"

/*
 * JSMN stuff
 */
static jsmntok_t tokens[MAX_JSON_TOKENS];
static jsmn_parser parser;

/*
 * Long enough to handle the MQTT payload without of stack overflow
 */
static uint8_t _common_buf[128];

static int jsoneq(const char *json, jsmntok_t *tok, const char *s) {
        if (tok->type == JSMN_STRING && (int)strlen(s) == tok->end - tok->start &&
                strncmp(json + tok->start, s, tok->end - tok->start) == 0) {
                return 0;
        }
        return -1;
}

static int jsmn_parse_array(const char *json_string, jsmntok_t *tokens, int num_tokens,
                int array_start, uint8_t out_array[]) {
        int ret = 0;
        if (tokens[array_start].type != JSMN_ARRAY) {
                ret = -1;
                goto out;
        }
        
        for (int i = 0; i < num_tokens; i++) {
                if (tokens[array_start + i + 1].type != JSMN_PRIMITIVE) {
                        break;
                }
                out_array[i] = (uint8_t) atoi((const char*) (json_string + tokens[array_start + i + 1].start));
                ret++;

        }
out:
        return ret;
}

int hdlr_open(const void *payload, size_t payload_len, void *p_user_data) {
        (void) p_user_data;

        int ret = 0;
        int bytes_num = 0;
        int r;

        jsmn_init(&parser);
        memset(_common_buf, 0, sizeof(_common_buf));
        memset(tokens, 0, sizeof(tokens));

        r = jsmn_parse(&parser, payload, payload_len, tokens,
                 sizeof(tokens) / sizeof(tokens[0]));

        /* Assume the top-level element is an object */
        if (r < 1 || tokens[0].type != JSMN_OBJECT) {
		return -1;
                goto out;
        }

        for (int i = 1; i <= r; i++) {
                if (jsoneq((const char *) payload, &tokens[i], "data") == 0) {
                        bytes_num = jsmn_parse_array((const char *) payload, tokens, r, i + 1, _common_buf);
                        if (bytes_num != 8) {
                                ret = -2;
                                goto out;
                        }
                }
        }

        ret = command_stm32_open(_common_buf, bytes_num);
out:
        return ret;
}

int hdlr_close(const void *payload, size_t payload_len, void *p_user_data) {
        (void) p_user_data;

        int ret = 0;
        int bytes_num = 0;
        int r;

        jsmn_init(&parser);
        memset(_common_buf, 0, sizeof(_common_buf));
        memset(tokens, 0, sizeof(tokens));

        r = jsmn_parse(&parser, payload, payload_len, tokens,
                 sizeof(tokens) / sizeof(tokens[0]));

        /* Assume the top-level element is an object */
        if (r < 1 || tokens[0].type != JSMN_OBJECT) {
		return -1;
                goto out;
        }

        for (int i = 1; i <= r; i++) {
                if (jsoneq((const char *) payload, &tokens[i], "data") == 0) {
                        bytes_num = jsmn_parse_array((const char *) payload, tokens, r, i + 1, _common_buf);
                        if (bytes_num != 8) {
                                ret = -2;
                                goto out;
                        }
                }
        }

        ret = command_stm32_close(_common_buf, bytes_num);
out:
        return ret;
}


