#include <string.h>

#include "unity.h"

#include "aws_iot_mqtt_task_common_sub_handler.h"
#include "aws_iot_mqtt_task_handler_procedures.h"
#include "mock_aws_iot_mqtt_task_payload_handlers.h"

#define CLIENT_ID "001122334455"
#define CLIENT_ID_LEN strlen("001122334455")

void test_command_open_correct_payload_should_return_success(void) {

        char topic_buf[64] = {0};
        size_t topic_len = 0;

        char payload[] = "{\'data\': [0,1,1,1,0,0,0,0]}";
        size_t payload_len = strlen(payload);

        sprintf(topic_buf, "%.*s/commands/open", (int) CLIENT_ID_LEN, CLIENT_ID);

        aws_iot_mqtt_task_payload_handlers_register(topic_buf, hdlr_open);

        hdlr_open_ExpectWithArrayAndReturn((const void*)payload, payload_len, payload_len, NULL, 0, 0);

        common_sub_handler(topic_buf, strlen(topic_buf), (void*) payload, payload_len, NULL);
}


void test_command_close_correct_payload_should_return_success(void) {

        char topic_buf[64] = {0};
        size_t topic_len = 0;

        char payload[] = "{\'data\': [0,1,1,1,0,0,0,0]}";
        size_t payload_len = strlen(payload);

        sprintf(topic_buf, "%.*s/commands/close", (int) CLIENT_ID_LEN, CLIENT_ID);

        aws_iot_mqtt_task_payload_handlers_register(topic_buf, hdlr_close);

        hdlr_close_ExpectWithArrayAndReturn((const void*)payload, payload_len, payload_len, NULL, 0, 0);

        common_sub_handler(topic_buf, strlen(topic_buf), (void*) payload, payload_len, NULL);
}


void test_command_close_incorrect_payload_should_fail(void) {

        char topic_buf[64] = {0};
        size_t topic_len = 0;

        /*
         * The payload format is wrong
         */
        char payload[] = "{\'data\': [0,1,1,1,0,0,0,0]";
        size_t payload_len = strlen(payload);

        sprintf(topic_buf, "%.*s/commands/close", (int) CLIENT_ID_LEN, CLIENT_ID);

        aws_iot_mqtt_task_payload_handlers_register(topic_buf, hdlr_close);

        hdlr_close_ExpectAndReturn((const void*)payload, payload_len, NULL, -1);

        common_sub_handler(topic_buf, strlen(topic_buf), (void*) payload, payload_len, NULL);
}

