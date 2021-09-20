/*
 * ESPRESSIF MIT License
 *
 * Copyright (c) 2019 <ESPRESSIF SYSTEMS (SHANGHAI) PTE LTD>
 *
 * Permission is hereby granted for use on all ESPRESSIF SYSTEMS products, in which case,
 * it is free of charge, to any person obtaining a copy of this software and associated
 * documentation files (the "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the Software is furnished
 * to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#include <stdio.h>
#include <string.h>

#include "infra_defs.h"

#include "esp_err.h"
#include "esp_log.h"

#include "nvs_flash.h"
#include "nvs.h"


#include "product_config.h"

#define CFG_PARTITION_NAME "product_cfg"
#define NVS_PRODUCT_CFG "user-cfg"

static const char *TAG = "product_config";

static bool s_part_init_flag;

static esp_err_t HAL_ProductCfgParam_init(void)
{
    esp_err_t ret = ESP_OK;

    do {
        if (s_part_init_flag == false) {
            if ((ret = nvs_flash_init_partition(CFG_PARTITION_NAME)) != ESP_OK) {
                ESP_LOGE(TAG, "NVS Flash init %s failed, Please check that you have flashed fctry partition!!!", CFG_PARTITION_NAME);
                break;
            }

            s_part_init_flag = true;
        }
    } while (0);

    return ret;
}

static int HAL_GetProductCfgParam(char *param_name, const char *param_name_str)
{
    esp_err_t ret;
    size_t read_len = 0;
    nvs_handle handle;

    do {
        if (HAL_ProductCfgParam_init() != ESP_OK) {
            break;
        }

        if (param_name == NULL) {
            ESP_LOGE(TAG, "%s param %s NULL", __func__, param_name);
            break;
        }

        ret = nvs_open_from_partition(CFG_PARTITION_NAME, NVS_PRODUCT_CFG, NVS_READONLY, &handle);

        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "%s nvs_open failed with %x", __func__, ret);
            break;
        }

        ret = nvs_get_str(handle, param_name_str, NULL, (size_t *)&read_len);

        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "%s nvs_get_str get %s failed with %x", __func__, param_name_str, ret);
            break;
        }

        ret = nvs_get_str(handle, param_name_str, param_name, (size_t *)&read_len);

        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "%s nvs_get_str get %s failed with %x", __func__, param_name_str, ret);
        } else {
            ESP_LOGV(TAG, "%s %s %s", __func__, param_name_str, param_name);
        }

        nvs_close(handle);
    } while (0);

    return read_len;
}


static int HAL_SetProductCfgParam(char *param_name, const char *param_name_str)
{
    esp_err_t ret;
    size_t write_len = 0;
    nvs_handle handle;

    do {
        if (HAL_ProductCfgParam_init() != ESP_OK) {
            break;
        }

        if (param_name == NULL) {
            ESP_LOGE(TAG, "%s param %s NULL", __func__, param_name);
            break;
        }

        ret = nvs_open_from_partition(CFG_PARTITION_NAME, NVS_PRODUCT_CFG, NVS_READWRITE, &handle);

        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "%s nvs_open failed with %x", __func__, ret);
            break;
        }

        ret = nvs_set_str(handle, param_name_str, param_name);

        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "%s nvs_set_str set %s failed with %x", __func__, param_name_str, ret);
        } else {
            write_len = strlen(param_name);
            ESP_LOGV(TAG, "%s %s %s", __func__, param_name_str, param_name);
        }

        nvs_close(handle);
    } while (0);

    return write_len;
}



///////////config
/**
 * @brief Get product key from user's system persistent storage
 *
 * @param [ou] product_key: array to store product key, max length is IOTX_PRODUCT_KEY_LEN
 * @return  the actual length of product key
 */

int HAL_GetLocalTimerConf(char LocalTimeConf[IOTX_LOCALTIME_CONF_LEN + 1])
{
	size_t write_len = HAL_GetProductCfgParam(LocalTimeConf, "LocalTimer");
	//ESP_LOGE(TAG, "HAL_GetLocalTimerConf:%s", LocalTimeConf);
    return write_len;
}

int HAL_SetLocalTimerConf(char *LocalTimeConf)
{
    return HAL_SetProductCfgParam(LocalTimeConf, "LocalTimer");
}



