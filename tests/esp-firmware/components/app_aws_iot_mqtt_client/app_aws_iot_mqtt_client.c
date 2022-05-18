#include <string.h>

#include "aws_iot_error.h"
#include "aws_iot_mqtt_client_interface.h"

#include "esp_log.h"

#include "app_aws_iot_mqtt_client.h"

#define TAG "mqtt_client"

static AWS_IoT_Client client;

void disconnectCallbackHandler(AWS_IoT_Client *pClient, void *data) {
	ESP_LOGW(TAG, "MQTT Disconnect");
	IoT_Error_t rc = FAILURE;

	if(NULL == pClient) {
		return;
	}

	if(aws_iot_is_autoreconnect_enabled(pClient)) {
		ESP_LOGI(TAG, "Auto Reconnect is enabled, Reconnecting attempt will start now");
	} else {
		ESP_LOGW(TAG, "Auto Reconnect not enabled. Starting manual reconnect...");
		rc = aws_iot_mqtt_attempt_reconnect(pClient);
		if(NETWORK_RECONNECTED == rc) {
			ESP_LOGW(TAG, "Manual Reconnect Successful");
		} else {
			ESP_LOGW(TAG, "Manual Reconnect Failed - %d", rc);
		}
	}
}

IoT_Error_t app_aws_iot_mqtt_client_init(const char *host, uint16_t port, 
                const char *aws_rootCA, const char *client_cert, const char *client_key) {

	IoT_Client_Init_Params mqttInitParams = iotClientInitParamsDefault;

	mqttInitParams.enableAutoReconnect = false; // We enable this later below
	mqttInitParams.pHostURL = (char*) host;
	mqttInitParams.port = port;

	mqttInitParams.pRootCALocation = aws_rootCA;
	mqttInitParams.pDeviceCertLocation = client_cert;
	mqttInitParams.pDevicePrivateKeyLocation = client_key;

	mqttInitParams.mqttCommandTimeout_ms = 20000;
	mqttInitParams.tlsHandshakeTimeout_ms = 5000;
	mqttInitParams.isSSLHostnameVerify = true;
	mqttInitParams.disconnectHandler = disconnectCallbackHandler;
	mqttInitParams.disconnectHandlerData = NULL;

	return aws_iot_mqtt_init(&client, &mqttInitParams);
}

IoT_Error_t app_aws_iot_mqtt_client_connect(const char *client_id, uint16_t client_id_len) {

        IoT_Client_Connect_Params connectParams = iotClientConnectParamsDefault;

	connectParams.keepAliveIntervalInSec = 10;
	connectParams.isCleanSession = true;
	connectParams.MQTTVersion = MQTT_3_1_1;
	/* Client ID is set in the menuconfig of the example */
	connectParams.pClientID = client_id;
	connectParams.clientIDLen = client_id_len;
	connectParams.isWillMsgPresent = false;

        return aws_iot_mqtt_connect(&client, &connectParams);
}

IoT_Error_t app_aws_iot_mqtt_client_autoreconnect_set_status(bool status) {
	return aws_iot_mqtt_autoreconnect_set_status(&client, status);
}

IoT_Error_t app_aws_iot_mqtt_client_subscribe(const char *topic_name, size_t topic_name_len,
                QoS qos, pApplicationHandler_t p_handler, void *user_data) {

	return aws_iot_mqtt_subscribe(&client, topic_name, topic_name_len, qos, p_handler, user_data);
}

IoT_Error_t app_aws_iot_mqtt_client_publish(const char *topic_name, uint16_t topic_name_len,
                QoS qos, void *payload) {
        IoT_Publish_Message_Params pub_params;
	pub_params.qos = qos;
	pub_params.payload = payload;
	pub_params.isRetained = 0;
        pub_params.payloadLen = strlen(payload);

        return aws_iot_mqtt_publish(&client, topic_name, topic_name_len, &pub_params);
}

IoT_Error_t app_aws_iot_mqtt_client_yield(uint32_t timeout_ms) {
        return aws_iot_mqtt_yield(&client, timeout_ms);
}

AWS_IoT_Client *app_aws_iot_mqtt_client_get_client(void) {
        return &client;
}
