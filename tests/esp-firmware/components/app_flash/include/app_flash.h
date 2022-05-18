#ifndef APP_FLASH_H
#define APP_FLASH_H

#define ECDH_KEY_LEN 32

#include "esp_err.h"

esp_err_t app_flash_init(void);

esp_err_t app_flash_erase_item(const char key[32]);
esp_err_t app_flash_load_item(const char key[32], void *buf, size_t *sz);
esp_err_t app_flash_save_item(const char key[32], const void *buf, size_t sz);

esp_err_t app_flash_read_data_nvs_partition(const char *partition_name, void *dst, size_t size);

#endif /* APP_FLASH_H */
