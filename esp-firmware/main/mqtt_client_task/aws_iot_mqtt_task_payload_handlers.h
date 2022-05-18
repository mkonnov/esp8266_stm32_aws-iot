#ifndef AWS_IOT_MQTT_TASK_PAYLOAD_HANDLERS_H
#define AWS_IOT_MQTT_TASK_PAYLOAD_HANDLERS_H

#include <stdint.h>
#include <stdlib.h>

int hdlr_open(const void *payload, size_t payload_len, void *p_user_data);
int hdlr_close(const void *payload, size_t payload_len, void *p_user_data);
int hdlr_clear(const void *payload, size_t payload_len, void *p_user_data);
int hdlr_report(const void *payload, size_t payload_len, void *p_user_data);
int hdlr_set_clock(const void *payload, size_t payload_len, void *p_user_data);
int hdlr_set_system_params(const void *payload, size_t payload_len, void *p_user_data);
int hdlr_write_flash(const void *payload, size_t payload_len, void *p_user_data);
int hdlr_read_flash(const void *payload, size_t payload_len, void *p_user_data);
int hdlr_jump_boot(const void *payload, size_t payload_len, void *p_user_data);





#endif /* AWS_IOT_MQTT_TASK_PAYLOAD_HANDLERS_H */
