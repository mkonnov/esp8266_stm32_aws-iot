/*
 * Copyright 2010-2015 Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * Additions Copyright 2016 Espressif Systems (Shanghai) PTE LTD
 *
 * Licensed under the Apache License, Version 2.0 (the "License").
 * You may not use this file except in compliance with the License.
 * A copy of the License is located at
 *
 *  http://aws.amazon.com/apache2.0
 *
 * or in the "license" file accompanying this file. This file is distributed
 * on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied. See the License for the specific language governing
 * permissions and limitations under the License.
 */
/**
 * @file subscribe_publish_sample.c
 * @brief simple MQTT publish and subscribe on the same topic
 *
 * This example takes the parameters from the build configuration and establishes a connection to the AWS IoT MQTT Platform.
 * It subscribes and publishes to the same topic - "test_topic/esp32"
 *
 * Some setup is required. See example README for details.
 *
 */
#include <stdio.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"

#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_wps.h"
#include "esp_event_loop.h"
#include "esp_log.h"

#include "app_flash.h"

#include "uart_command_protocol.h"

#include "aws_iot_mqtt_task.h"
#include "uart_rx_task.h"

#include "common_defs.h"

#define TAG "main"

#define CERTS_AWS_ROOT_CA_SIZE 1188
#define CERTS_PRIVATE_PEM_KEY_SIZE 1679
#define CERTS_CERTIFICATE_PEM_CRT_SIZE 1224

/* FreeRTOS event group to signal when we are connected & ready to make a request */
static EventGroupHandle_t wifi_event_group;

/* The event group allows multiple bits for each event,
   but we only care about one event - are we connected
   to the AP with an IP? */
static const int CONNECTED_BIT = BIT0;

static struct aws_iot_mqtt_client_task_params aws_iot_mqtt_client_task_params = {
        .host_address = CONFIG_AWS_IOT_MQTT_HOST,
        .port = 8883,
        .aws_root_ca_pem = NULL,
        .private_pem_key = NULL,
        .cert_pem_crt = NULL
};

static esp_wps_config_t config = WPS_CONFIG_INIT_DEFAULT(WPS_TYPE_PBC);

static void wps_begin_cb(uint16_t data_len, const uint8_t *data);
static void wifi_direct_creds_handle_cb(uint16_t data_len, const uint8_t *data);

static esp_err_t event_handler(void *ctx, system_event_t *event)
{
	/* For accessing reason codes in case of disconnection */
	system_event_info_t *info = &event->event_info;
	
	switch(event->event_id) {
	case SYSTEM_EVENT_STA_START:
                ESP_LOGI(TAG, "SYSTEM_EVENT_STA_START");
		esp_wifi_connect();
		break;
	case SYSTEM_EVENT_STA_GOT_IP:
                ESP_LOGI(TAG, "SYSTEM_EVENT_STA_GOT_IP");
		xEventGroupSetBits(wifi_event_group, CONNECTED_BIT);
		break;
	case SYSTEM_EVENT_STA_DISCONNECTED:
		ESP_LOGE(TAG, "Disconnect reason : %d", info->disconnected.reason);
		if (info->disconnected.reason == WIFI_REASON_BASIC_RATE_NOT_SUPPORT) {
			/*Switch to 802.11 bgn mode */
			esp_wifi_set_protocol(ESP_IF_WIFI_STA, WIFI_PROTOCAL_11B | WIFI_PROTOCAL_11G | WIFI_PROTOCAL_11N);
		}
		esp_wifi_connect();
		xEventGroupClearBits(wifi_event_group, CONNECTED_BIT);
                break;
        case SYSTEM_EVENT_STA_WPS_ER_SUCCESS:
        	/*point: the function esp_wifi_wps_start() only get ssid & password
        	 * so call the function esp_wifi_connect() here
        	 * */
        	ESP_LOGI(TAG, "SYSTEM_EVENT_STA_WPS_ER_SUCCESS");
        	ESP_ERROR_CHECK(esp_wifi_wps_disable());
        	ESP_ERROR_CHECK(esp_wifi_connect());
                break;

        case SYSTEM_EVENT_STA_WPS_ER_FAILED:
        	ESP_LOGI(TAG, "SYSTEM_EVENT_STA_WPS_ER_FAILED");
        	ESP_ERROR_CHECK(esp_wifi_wps_disable());
        	ESP_ERROR_CHECK(esp_wifi_wps_enable(&config));
        	ESP_ERROR_CHECK(esp_wifi_wps_start(0));
                break;

    	case SYSTEM_EVENT_STA_WPS_ER_TIMEOUT:
        	ESP_LOGI(TAG, "SYSTEM_EVENT_STA_WPS_ER_TIMEOUT");
        	ESP_ERROR_CHECK(esp_wifi_wps_disable());
        	ESP_ERROR_CHECK(esp_wifi_wps_enable(&config));
        	ESP_ERROR_CHECK(esp_wifi_wps_start(0));
        	break;

	default:
		break;
	}
	return ESP_OK;
}

static void initialise_wifi(const wifi_sta_config_t *sta_cfg)
{
	wifi_init_config_t init_cfg = WIFI_INIT_CONFIG_DEFAULT();

	tcpip_adapter_init();
	wifi_event_group = xEventGroupCreate();
	ESP_ERROR_CHECK( esp_event_loop_init(event_handler, NULL) );
	ESP_ERROR_CHECK( esp_wifi_init(&init_cfg) );
        ESP_ERROR_CHECK( esp_wifi_set_storage(WIFI_STORAGE_FLASH));

        ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_STA) );
  
        if (sta_cfg != NULL) {
                wifi_config_t wifi_config;
                memcpy(&(wifi_config.sta), sta_cfg, sizeof(wifi_sta_config_t));

                ESP_LOGI(TAG, "Setting WiFi configuration %s:%s...", wifi_config.sta.ssid, wifi_config.sta.password);
                ESP_ERROR_CHECK( esp_wifi_set_config(WIFI_IF_STA, &wifi_config) );
        }
        ESP_ERROR_CHECK( esp_wifi_start() );
}

static esp_err_t read_certs(void) {
        esp_err_t err;

        aws_iot_mqtt_client_task_params.aws_root_ca_pem = malloc(CERTS_AWS_ROOT_CA_SIZE + 1);
        aws_iot_mqtt_client_task_params.private_pem_key = malloc(CERTS_PRIVATE_PEM_KEY_SIZE + 1);
        aws_iot_mqtt_client_task_params.cert_pem_crt = malloc(CERTS_CERTIFICATE_PEM_CRT_SIZE + 1);

        bzero(aws_iot_mqtt_client_task_params.aws_root_ca_pem, CERTS_AWS_ROOT_CA_SIZE + 1);
        bzero(aws_iot_mqtt_client_task_params.private_pem_key, CERTS_PRIVATE_PEM_KEY_SIZE + 1);
        bzero(aws_iot_mqtt_client_task_params.cert_pem_crt, CERTS_CERTIFICATE_PEM_CRT_SIZE + 1);

        err = app_flash_read_data_nvs_partition("aws_root_ca_pem", 
                        aws_iot_mqtt_client_task_params.aws_root_ca_pem, 
                        CERTS_AWS_ROOT_CA_SIZE);
        if (err != ESP_OK) goto out;

        err = app_flash_read_data_nvs_partition("private_pem_key", 
                        aws_iot_mqtt_client_task_params.private_pem_key, 
                        CERTS_PRIVATE_PEM_KEY_SIZE);
        if (err != ESP_OK) goto out;

        err = app_flash_read_data_nvs_partition("cert_pem_crt", 
                        aws_iot_mqtt_client_task_params.cert_pem_crt, 
                        CERTS_CERTIFICATE_PEM_CRT_SIZE);
        if (err != ESP_OK) goto out;

out:
        return err;
}

void app_main() {

        uint8_t mac[6];
        char mac_buf[32] = {0};
        size_t sz;
        esp_err_t err;
        wifi_sta_config_t sta = {0};

        app_flash_init();

        uart_command_protocol_register_cb(COMMAND_WPS_BEGIN_CODE, wps_begin_cb);
        uart_command_protocol_register_cb(COMMAND_DIRECT_WIFI_CREDS_CODE, wifi_direct_creds_handle_cb);

        xTaskCreate(&uart_rx_task, "uart_rx_task", 2048, NULL, 6, NULL);

        ESP_ERROR_CHECK(app_flash_load_item("sta_settings", (void*) &sta, &sz));

        if (sz == 0) {
                ESP_LOGW(TAG, "No STA settings saved");
                initialise_wifi(NULL);
        } else {
	        initialise_wifi(&sta);
        }

	/* Wait for WiFI to show as connected */
	xEventGroupWaitBits(wifi_event_group, CONNECTED_BIT,
						false, true, portMAX_DELAY);

        esp_wifi_wps_disable();
        if ((err = read_certs()) != ESP_OK)
                        ESP_LOGE(TAG, "err %d\n", err);

        if (err != ESP_OK) {
                ESP_LOGE(TAG, "AWS certificates are not installed");
                abort();
        }

        /*
         * Derive MQTT client ID from MAC address.
         */
        esp_wifi_get_mac(WIFI_MODE_STA, mac);
        for (int i = 0; i < 6; i++) {
                sprintf(mac_buf + strlen(mac_buf), "%X", mac[i]);
        }
        strncpy(aws_iot_mqtt_client_task_params.client_id, mac_buf, strlen(mac_buf));
        aws_iot_mqtt_client_task_params.client_id_len = strlen(mac_buf);

        xTaskCreate(&aws_iot_mqtt_client_task, "mqtt_client_task", 5 * 1024, 
                        (void *) &aws_iot_mqtt_client_task_params, 6, NULL);

#if 0
        payload_t pld;

        for (;;) {

                memset(pld.payload, 0, sizeof(pld.payload));
                pld.topic = PUB_TOPIC_REPORT_SYSTEM;
                strcpy((char*)pld.payload, "{\"test_obj\": [0,1,2,3,4,5]}");
                aws_iot_mqtt_client_task_push_to_pub_queue(&pld, portMAX_DELAY);

                memset(pld.payload, 0, sizeof(pld.payload));
                pld.topic = PUB_TOPIC_REPORT_DEVICES;
                strcpy((char*)pld.payload, "{\"test_obj\": [0,1,2,3,4,5]}");
                aws_iot_mqtt_client_task_push_to_pub_queue(&pld, portMAX_DELAY);

                vTaskDelay(1000 / portTICK_RATE_MS);
        }
#endif
}

static void wps_begin_cb(uint16_t data_len, const uint8_t *data) {
        (void) data_len;
        (void) data;
        esp_wifi_wps_enable(&config);
        esp_wifi_wps_start(0);
}

static void wifi_direct_creds_handle_cb(uint16_t data_len, const uint8_t *data) {

        struct wifi_creds *wifi_creds_p = (struct wifi_creds*) data;
        wifi_config_t wifi_config;

        memset(&wifi_config, 0, sizeof(wifi_config));

        memcpy(wifi_config.sta.ssid, wifi_creds_p->ssid, wifi_creds_p->ssid_len);
        memcpy(wifi_config.sta.password, wifi_creds_p->password, wifi_creds_p->password_len);

        ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_STA) );
        ESP_ERROR_CHECK( esp_wifi_set_config(WIFI_IF_STA, &wifi_config) );

        ESP_LOGI(TAG, "connecting...");
        esp_wifi_connect();
}

