#include <string.h>

#include "esp_err.h"
#include "esp_log.h"
#include "esp_partition.h"

#include "nvs_flash.h"
#include "nvs.h"

#include "app_flash.h"

#define SECTION_NAME "storage"

static const char *TAG = "app_flash";

esp_err_t app_flash_init() {
	return nvs_flash_init();
}

esp_err_t app_flash_erase_item(const char key[32]) {
	nvs_handle my_handle;
	esp_err_t err = ESP_OK;

	err = nvs_open(SECTION_NAME, NVS_READWRITE, &my_handle);
	if (err != ESP_OK) {
		ESP_LOGE(TAG, "%s: can't open nvs", __FUNCTION__);
		return err;
	}

	err = nvs_erase_key(my_handle, key);
	if (err != ESP_OK) {
		ESP_LOGE(TAG, "%s: can't erase nvs, err = 0x%04X", __FUNCTION__, err);
	}

	return err;
}

esp_err_t app_flash_load_item(const char key[32], void *buf, size_t *sz) {
	nvs_handle my_handle;
	esp_err_t err;
	size_t required_size = 0;

	err = nvs_open(SECTION_NAME, NVS_READWRITE, &my_handle);
	if (err != ESP_OK) {
		ESP_LOGE(TAG, "%s: can't open nvs", __FUNCTION__);
		return ESP_FAIL;
	}

	err = nvs_get_blob(my_handle, key, NULL, &required_size);
	if (err != ESP_OK && err != ESP_ERR_NVS_NOT_FOUND) {
		ESP_LOGE(TAG, "%s: Can't get item size\n", __FUNCTION__);
		return ESP_FAIL;
	}

        *sz = required_size;

	if (required_size > 0) {
                err = nvs_get_blob(my_handle, key, buf, &required_size);
		if (err != ESP_OK) {
			ESP_LOGE(TAG, "%s: Can't get data, size=%d\n", __FUNCTION__, required_size);
			return ESP_FAIL;
		}
        }
	return ESP_OK;
}

esp_err_t app_flash_save_item(const char key[32], const void *buf, size_t sz) {

	nvs_handle my_handle;
	esp_err_t err;

	err = nvs_open(SECTION_NAME, NVS_READWRITE, &my_handle);

	if (err != ESP_OK) {
		ESP_LOGE(TAG, "nvs_open fail\n");
		return err;
	}

	err = nvs_set_blob(my_handle, key, buf, sz);
	if (err != ESP_OK) {
		ESP_LOGE(TAG, "nvs_set_blob fail, err = %x\n", err);
		return err;
	}

	err = nvs_commit(my_handle);
	if (err != ESP_OK)
		return err;

	nvs_close(my_handle);
	return ESP_OK;
}

esp_err_t app_flash_read_data_nvs_partition(const char *partition_name, void *dst, size_t size) {
        esp_err_t err;

        const esp_partition_t *prt = esp_partition_find_first( 
                        ESP_PARTITION_TYPE_DATA,
                        ESP_PARTITION_SUBTYPE_ANY,
                        partition_name);

        if (prt == NULL) {
                return ESP_ERR_NOT_FOUND;
        }

        err = esp_partition_read(prt, 0, dst, size);
        
	return err;
}
