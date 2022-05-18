#include <string.h>

#include "aws_iot_mqtt_task_handler_procedures.h"

/*
 * MQTT payload handlers holder
 */
static struct payload_handler payload_handlers[MAX_JSON_HANDLERS] = {0};

struct payload_handler *aws_iot_mqtt_task_payload_handlers_get(void) {
        return payload_handlers;
}

int aws_iot_mqtt_task_payload_handlers_register(const char *topic_name, 
                int (*hdlr)(const void *payload, size_t payload_len, void *user_data)) {
        int idx = 0;
        for (; idx < MAX_JSON_HANDLERS; idx++) {
                if (payload_handlers[idx].hdlr == NULL)
                        break;
        }
        if (idx == MAX_JSON_HANDLERS)
                return -1;
        strcpy(payload_handlers[idx].topic_name, topic_name);
        payload_handlers[idx].hdlr = hdlr;
        return 0;
}


