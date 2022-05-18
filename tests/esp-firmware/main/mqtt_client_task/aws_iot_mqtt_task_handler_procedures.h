#ifndef AWS_IOT_MQTT_TASK_HANDLER_PROCEDURES_H
#define AWS_IOT_MQTT_TASK_HANDLER_PROCEDURES_H

#include <stdlib.h>

#define MAX_JSON_HANDLERS 8

struct payload_handler {
        char topic_name[64];
        int (*hdlr)(const void *payload, size_t payload_len, void *user_data);
};

struct payload_handler *aws_iot_mqtt_task_payload_handlers_get(void);
int aws_iot_mqtt_task_payload_handlers_register(const char *topic_name, 
                int (*hdlr)(const void *payload, size_t payload_len, void *user_data));


#endif /* AWS_IOT_MQTT_TASK_HANDLER_PROCEDURES_H */
