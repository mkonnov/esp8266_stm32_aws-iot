#ifndef APP_AWS_IOT_MQTT_CLIENT_H
#define APP_AWS_IOT_MQTT_CLIENT_H

#include "aws_iot_error.h"
#include "aws_iot_mqtt_client.h"

IoT_Error_t app_aws_iot_mqtt_client_init(const char *host, uint16_t port, 
                const char *aws_rootCA,
                const char *client_cert,
                const char *client_key);

IoT_Error_t app_aws_iot_mqtt_client_connect(const char *client_id, uint16_t client_id_len);

IoT_Error_t app_aws_iot_mqtt_client_autoreconnect_set_status(bool status);

IoT_Error_t app_aws_iot_mqtt_client_subscribe(const char *topic_name, size_t topic_name_len,
                QoS qos, pApplicationHandler_t pApplicationHandler, void *pApplicationHandlerData);

IoT_Error_t app_aws_iot_mqtt_client_publish(const char *topic_name, uint16_t topic_name_len,
                QoS qos, void *payload);

IoT_Error_t app_aws_iot_mqtt_client_yield(uint32_t timeout_ms);

AWS_IoT_Client *app_aws_iot_mqtt_client_get_client(void);


#endif /* APP_AWS_IOT_MQTT_CLIENT_H */
