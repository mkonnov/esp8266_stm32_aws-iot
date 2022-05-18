#ifndef AWS_IOT_MQTT_TASK_COMMON_SUB_HANDLER_H
#define AWS_IOT_MQTT_TASK_COMMON_SUB_HANDLER_H

#include <stdint.h>
#include <stdlib.h>

int common_sub_handler(const char *topic_name, uint16_t topic_name_len,
                void *payload, size_t payload_len, void *user_data);


#endif /* AWS_IOT_MQTT_TASK_COMMON_SUB_HANDLER_H */
