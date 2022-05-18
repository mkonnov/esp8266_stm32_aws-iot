#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "esp_log.h"
#include "app_aws_iot_mqtt_client.h"

#include "aws_iot_mqtt_task.h"
#include "aws_iot_mqtt_task_handler_procedures.h"
#include "aws_iot_mqtt_task_payload_handlers.h"
#include "aws_iot_mqtt_task_common_sub_handler.h"

#define TAG "mqtt_task"

static QueueHandle_t pub_queue = NULL;

static void _common_sub_handler_internal(AWS_IoT_Client *pClient, char *topic_name, uint16_t topic_name_len,
                IoT_Publish_Message_Params *params, void *user_data) {
        int ret = common_sub_handler(topic_name, topic_name_len,
                params->payload, params->payloadLen, user_data);
        if (ret != 0) {
                ESP_LOGE(TAG, "%.*s: failed to handle payload", 
                        topic_name_len, topic_name);
        }
}

void aws_iot_mqtt_client_task(void *param) {

        BaseType_t queue_ret;
        payload_t pub_payload;
        pub_queue = xQueueCreate(PUB_QUEUE_LENGTH, PUB_QUEUE_ITEM_SIZE);

        struct aws_iot_mqtt_client_task_params *params = (struct aws_iot_mqtt_client_task_params *) param;

        char topic_buf[64];
        size_t topic_len = 0;

	IoT_Error_t rc = FAILURE;

	rc = app_aws_iot_mqtt_client_init(params->host_address, params->port, 
                        params->aws_root_ca_pem, params->cert_pem_crt, params->private_pem_key);
	if(SUCCESS != rc) {
		ESP_LOGE(TAG, "aws_iot_mqtt_init returned error : %d ", rc);
		abort();
	}

	ESP_LOGI(TAG, "Connecting to AWS...");
	do {
		rc = app_aws_iot_mqtt_client_connect(params->client_id, params->client_id_len);
		if(SUCCESS != rc) {
			ESP_LOGE(TAG, "Error(%d) connecting to %s:%d", rc, params->host_address,
                                        params->port);
			vTaskDelay(1000 / portTICK_RATE_MS);
		}
	} while(SUCCESS != rc);


        memset(topic_buf, 0, 64);
        sprintf(topic_buf, "%.*s/commands/open", params->client_id_len, params->client_id);
        aws_iot_mqtt_task_payload_handlers_register(topic_buf, hdlr_open);

        memset(topic_buf, 0, 64);
        sprintf(topic_buf, "%.*s/commands/close", params->client_id_len, params->client_id);
        aws_iot_mqtt_task_payload_handlers_register(topic_buf, hdlr_close);

        struct payload_handler *hdlrs = aws_iot_mqtt_task_payload_handlers_get();

        for (int i = 0; i < MAX_JSON_HANDLERS; i++) {
                if (hdlrs[i].hdlr == NULL) {
                        break;
                }

                ESP_LOGI(TAG, "subscribe to %s", hdlrs[i].topic_name); 
                rc = app_aws_iot_mqtt_client_subscribe(hdlrs[i].topic_name, strlen(hdlrs[i].topic_name),
                        QOS0, _common_sub_handler_internal, NULL);
                if (SUCCESS != rc) {
                        ESP_LOGE(TAG, "%s:%d, rc = %d", __func__, __LINE__, rc);
                }
        }


	while(1) {

		//Max time the yield function will wait for read messages
                rc = app_aws_iot_mqtt_client_yield(100);
		if(NETWORK_ATTEMPTING_RECONNECT == rc) {
			// If the client is attempting to reconnect we will skip the rest of the loop.
			continue;
		}

                memset(&pub_payload, 0, sizeof(pub_payload));
                queue_ret = xQueueReceive(pub_queue, &pub_payload, 0);
                if (queue_ret == pdTRUE) {
                        memset(topic_buf, 0, sizeof(topic_buf));

                        switch (pub_payload.topic) {
                        case PUB_TOPIC_REPORT_SYSTEM:
                                sprintf(topic_buf, "%.*s/report/system", params->client_id_len,
                                                params->client_id);
                                topic_len = strlen(topic_buf);
                                break;
                        case PUB_TOPIC_REPORT_DEVICES:
                                sprintf(topic_buf, "%.*s/report/devices", params->client_id_len,
                                                params->client_id);
                                topic_len = strlen(topic_buf);
                                break;
                        default:
                                break;
                        }
                        rc = app_aws_iot_mqtt_client_publish(topic_buf, topic_len, QOS1, pub_payload.payload);
                }

		if (rc == MQTT_REQUEST_TIMEOUT_ERROR) {
			ESP_LOGW(TAG, "QOS1 publish ack not received.");
			rc = SUCCESS;
		}
                vTaskDelay(1000 / portTICK_RATE_MS);
	}

	ESP_LOGE(TAG, "An error occurred in the main loop.");
	abort();
}

int aws_iot_mqtt_client_task_push_to_pub_queue(payload_t *pld, uint32_t ticks_to_wait) {
        BaseType_t ret;
        if (pub_queue == NULL) {
                return -2;
        }

        ret = xQueueSend(pub_queue, pld, ticks_to_wait);
        if (ret != pdTRUE) {
                return -1;
        }

        return 0;
}
