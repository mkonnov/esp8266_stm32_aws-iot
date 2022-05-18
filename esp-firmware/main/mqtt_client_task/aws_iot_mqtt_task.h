#ifndef MQTT_CLIENT_H
#define MQTT_CLIENT_H

#include "jsmn.h"

#define PUB_QUEUE_LENGTH 3
#define PUB_QUEUE_ITEM_SIZE sizeof(payload_t)

struct aws_iot_mqtt_client_task_params {
        char host_address[255];
        uint16_t port;
        char client_id[32];
        uint16_t client_id_len;
        char *aws_root_ca_pem;
        char *private_pem_key;
        char *cert_pem_crt;
};

typedef struct {
        enum {
                PUB_TOPIC_REPORT_SYSTEM,
                PUB_TOPIC_REPORT_DEVICES,
        } topic;
        uint8_t payload[256];
} payload_t;

void aws_iot_mqtt_client_task(void *param);

int aws_iot_mqtt_client_task_push_to_pub_queue(payload_t *pld, uint32_t ticks_to_wait);


#endif /* MQTT_CLIENT_H */
