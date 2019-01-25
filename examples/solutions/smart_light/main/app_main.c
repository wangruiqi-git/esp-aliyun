/*
 * ESPRSSIF MIT License
 *
 * Copyright (c) 2018 <ESPRESSIF SYSTEMS (SHANGHAI) PTE LTD>
 *
 * Permission is hereby granted for use on ESPRESSIF SYSTEMS ESP32 only, in which case,
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
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event_loop.h"
#include "esp_log.h"
#include "nvs_flash.h"

#include "app_entry.h"

#include "iot_export.h"
#include "iot_import.h"
#include "awss.h"

/* FreeRTOS event group to signal when we are connected & ready to make a request */
EventGroupHandle_t wifi_event_group;

/* The event group allows multiple bits for each event,
   but we only care about one event - are we connected
   to the AP with an IP? */
const int CONNECTED_BIT = BIT0;
static const char* TAG = "app main";

static esp_err_t event_handler(void *ctx, system_event_t *event)
{
    switch (event->event_id) {
        case SYSTEM_EVENT_STA_START:
            esp_wifi_connect();
            break;

        case SYSTEM_EVENT_STA_GOT_IP:
            break;

        case SYSTEM_EVENT_STA_DISCONNECTED:
            /* This is a workaround as ESP8266 WiFi libs don't currently
               auto-reassociate. */
            esp_wifi_connect();
            xEventGroupClearBits(wifi_event_group, CONNECTED_BIT);
            break;

        default:
            break;
    }

    return ESP_OK;
}

static void initialise_wifi(void)
{
    tcpip_adapter_init();
    wifi_event_group = xEventGroupCreate();
    ESP_ERROR_CHECK(esp_event_loop_init(event_handler, NULL));
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
}

void smart_light_example(void* parameter)
{
    while(1) {
        ESP_LOGI(TAG, "wait wifi connect...");
        xEventGroupWaitBits(wifi_event_group, CONNECTED_BIT, false, true, portMAX_DELAY);

        app_main_paras_t paras;
        const char* argv[] = {"main", "loop"};
        paras.argc = 2;
        paras.argv = argv; 
    
        ESP_LOGI(TAG, "entry linkkit main...");
        linkkit_main((void *)&paras);
    }
}


static void linkkit_event_monitor(int event)
{
    switch (event) {
        case IOTX_AWSS_START: // AWSS start without enbale, just supports device discover
            // operate led to indicate user
            ESP_LOGW(TAG, "IOTX_AWSS_START");
            break;
        case IOTX_AWSS_ENABLE: // AWSS enable, AWSS doesn't parse awss packet until AWSS is enabled.
            ESP_LOGW(TAG, "IOTX_AWSS_ENABLE");
            // operate led to indicate user
            break;
        case IOTX_AWSS_LOCK_CHAN: // AWSS lock channel(Got AWSS sync packet)
            ESP_LOGW(TAG, "IOTX_AWSS_LOCK_CHAN");
            // operate led to indicate user
            break;
        case IOTX_AWSS_PASSWD_ERR: // AWSS decrypt passwd error
            ESP_LOGW(TAG, "IOTX_AWSS_PASSWD_ERR");
            // operate led to indicate user
            break;
        case IOTX_AWSS_GOT_SSID_PASSWD:
            ESP_LOGW(TAG, "IOTX_AWSS_GOT_SSID_PASSWD");
            // awss_stop();
            // operate led to indicate user
            break;
        case IOTX_AWSS_CONNECT_ADHA: // AWSS try to connnect adha (device
                                     // discover, router solution)
            ESP_LOGW(TAG, "IOTX_AWSS_CONNECT_ADHA");
            // operate led to indicate user
            break;
        case IOTX_AWSS_CONNECT_ADHA_FAIL: // AWSS fails to connect adha
            ESP_LOGW(TAG, "IOTX_AWSS_CONNECT_ADHA_FAIL");
            // operate led to indicate user
            break;
        case IOTX_AWSS_CONNECT_AHA: // AWSS try to connect aha (AP solution)
            ESP_LOGW(TAG, "IOTX_AWSS_CONNECT_AHA");
            // operate led to indicate user
            break;
        case IOTX_AWSS_CONNECT_AHA_FAIL: // AWSS fails to connect aha
            ESP_LOGW(TAG, "IOTX_AWSS_CONNECT_AHA_FAIL");
            // operate led to indicate user
            break;
        case IOTX_AWSS_SETUP_NOTIFY: // AWSS sends out device setup information
                                     // (AP and router solution)
            ESP_LOGW(TAG, "IOTX_AWSS_SETUP_NOTIFY");
            // operate led to indicate user
            break;
        case IOTX_AWSS_CONNECT_ROUTER: // AWSS try to connect destination router
            ESP_LOGW(TAG, "IOTX_AWSS_CONNECT_ROUTER");
            // operate led to indicate user
            break;
        case IOTX_AWSS_CONNECT_ROUTER_FAIL: // AWSS fails to connect destination
                                            // router.
            ESP_LOGW(TAG, "IOTX_AWSS_CONNECT_ROUTER_FAIL");
            // operate led to indicate user
            break;
        case IOTX_AWSS_GOT_IP: // AWSS connects destination successfully and got
                               // ip address
            ESP_LOGW(TAG, "IOTX_AWSS_GOT_IP");

            // awss_report_cloud();
            // operate led to indicate user
            break;
        case IOTX_AWSS_SUC_NOTIFY: // AWSS sends out success notify (AWSS
                                   // sucess)
            ESP_LOGW(TAG, "IOTX_AWSS_SUC_NOTIFY");
            // operate led to indicate user
            break;
        case IOTX_AWSS_BIND_NOTIFY: // AWSS sends out bind notify information to
                                    // support bind between user and device
            ESP_LOGW(TAG, "IOTX_AWSS_BIND_NOTIFY");
            awss_stop();
            // operate led to indicate user
            break;
        case IOTX_AWSS_ENABLE_TIMEOUT: // AWSS enable timeout
                                       // user needs to enable awss again to support get ssid & passwd of router
            ESP_LOGW(TAG, "IOTX_AWSS_ENALBE_TIMEOUT");
            // operate led to indicate user
            break;
        case IOTX_CONN_CLOUD: // Device try to connect cloud
            ESP_LOGW(TAG, "IOTX_CONN_CLOUD");
            // operate led to indicate user
            break;
        case IOTX_CONN_CLOUD_FAIL: // Device fails to connect cloud, refer to
                                   // net_sockets.h for error code
            ESP_LOGW(TAG, "IOTX_CONN_CLOUD_FAIL");
            // operate led to indicate user
            break;
        case IOTX_CONN_CLOUD_SUC: // Device connects cloud successfully
            ESP_LOGW(TAG, "IOTX_CONN_CLOUD_SUC");
            // operate led to indicate user
            break;
        case IOTX_RESET: // Linkkit reset success (just got reset response from
                         // cloud without any other operation)
            ESP_LOGW(TAG, "IOTX_RESET");
            // operate led to indicate user
            break;
        default:
            break;
    }
}

void app_main()
{
    //Initialize NVS
    esp_err_t ret = nvs_flash_init();

    if (ret == ESP_ERR_NVS_NO_FREE_PAGES) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }

    void LITE_set_loglevel(int pri);
    LITE_set_loglevel(6);

    ESP_ERROR_CHECK(ret);

    initialise_wifi();

    awss_set_config_press(1);

    iotx_event_regist_cb(linkkit_event_monitor);
    
    void set_iotx_info();

    set_iotx_info();

    esp_timer_init();

    xTaskCreate(smart_light_example, "smart_light_example", 10240, NULL, 5, NULL);

    awss_start();
}

