#include <string.h>
#include <stdio.h>
#include <stdint.h>

#include "aws_iot_mqtt_task_handler_procedures.h"

int common_sub_handler(const char *topic_name, uint16_t topic_name_len,
                void *payload, size_t payload_len, void *user_data) {
        int ret = 0;
        int idx = 0;
        size_t topic_name_len_longest;

        struct payload_handler *payload_handlers = aws_iot_mqtt_task_payload_handlers_get();

        for (; idx < MAX_JSON_HANDLERS; idx++) {
                if (strlen(payload_handlers[idx].topic_name) >= topic_name_len) {
                        topic_name_len_longest = strlen(payload_handlers[idx].topic_name);
                } else {
                        topic_name_len_longest = topic_name_len;
                }
                if (strncmp(topic_name, payload_handlers[idx].topic_name, topic_name_len_longest) == 0) {
                        break;
                }
        }
        if (idx == MAX_JSON_HANDLERS) {
                ret = -1;
                goto out;
        }
        if (payload_handlers[idx].hdlr == NULL) {
                ret = -2;
                goto out;
        }
        ret = payload_handlers[idx].hdlr(payload, payload_len, user_data);
out:
        return ret;
}
