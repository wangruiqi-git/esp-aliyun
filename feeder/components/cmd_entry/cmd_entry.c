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
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "esp_log.h"
#include "esp_system.h"
#include "transport_uart.h"
#include "transport_data.h"
#include "conn_mgr.h"
#include "dm_wrapper.h"
#include "wifi_provision_api.h"
#include "cmd_entry.h"
#include "hal_motor.h"
#include "hal_led.h"


static const char *TAG = "cmd_entry";

#define AWSS_SC_NAME                 "active_awss"
#define AWSS_SOFTAP_NAME             "dev_ap"
#define AWSS_RESET_NAME              "reset"
#define AWSS_CONN_NAME               "netmgr connect"
#define AWSS_CONFIG_NAME             "linkkey"
#define AWSS_REBOOT_NAME             "reboot"
#define AWSS_KV_ERASE_DY_SECRET_NAME "kv_clear"
#define DYNAMIC_REG_KV_PREFIX       "DYNAMIC_REG_"
#define DYNAMIC_REG_KV_PREFIX_LEN   12
#define SET_LOG_LEVEL             "setlog"
#define MOTOR             "motor"
#define LEDTEST             "led"

static bool s_conn_mgr_exist = false;
#if 0
int app_check_config_pk(void)
{
    char PRODUCT_KEY[IOTX_PRODUCT_KEY_LEN + 1] = {0};

    int ret = HAL_GetProductKey(PRODUCT_KEY);

    if (!ret || !strlen(PRODUCT_KEY)) {
        ESP_LOGE(TAG, "Please first input four config");
        return 0;
    }

    return 1;
}
#endif
static void app_get_config_input_len(const char *param, uint32_t *len)
{
    uint32_t i = 0;
    for (i = 0; i < strlen(param); i ++) {
        if (param[i] == ' ' || param[i] == '\r' || param[i] == '\n') {
            break;
        }
    }
    *len = i;
}
#if 0
void start_conn_mgr(void)
{
    s_conn_mgr_exist = true;
    conn_mgr_start();
    s_conn_mgr_exist = false;
    vTaskDelete(NULL);
}
#endif
void app_get_input_param(char *param, size_t param_len)
{
    if (!param) {
        ESP_LOGE(TAG, "Input error");
        return;
    }

    if (strstr(param, SET_LOG_LEVEL)) {
        uint32_t len = 0;
        char buf[64 + 1] = {0};

        char *input = param + strlen(SET_LOG_LEVEL) + 1;
        app_get_config_input_len(input, &len);
        strncpy(buf, input, len);
        ESP_LOGI(TAG, "LEVEL: %s", buf);
		int level = atoi(buf);
		if(level>IOT_LOG_DEBUG||level<IOT_LOG_NONE)
			return;
		IOT_SetLogLevel(level);
        return;
    }
	else if(strstr(param, MOTOR)){
		uint32_t len = 0;
        char buf[64 + 1] = {0};

        char *input = param + strlen(MOTOR) + 1;
        app_get_config_input_len(input, &len);
        strncpy(buf, input, len);
        ESP_LOGI(TAG, "MOTOR: %s", buf);
		int speed = atoi(buf);
		motorControl(speed);
        return;
	}
	else if(strstr(param, LEDTEST)){
		uint32_t len = 0;
        char buf[64 + 1] = {0};

        char *input = param + strlen(LEDTEST) + 1;
        app_get_config_input_len(input, &len);
        strncpy(buf, input, len);
        ESP_LOGI(TAG, "LEDTEST: %s", buf);
		int mode = atoi(buf);
		led_work_mode_set(mode);

        return;
	}
	else {
        ESP_LOGE(TAG, "Can't recongize cmd");
        return;
    }
#if 0
    xTaskCreate(start_conn_mgr, "conn_mgr", CONN_MGR_TASK_SIZE, NULL, 5, NULL);
#endif
}
