/*
 * ESPRSSIF MIT License
 *
 * Copyright (c) 2019 <ESPRESSIF SYSTEMS (SHANGHAI) PTE LTD>
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
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp8266/rom_functions.h"

#include "esp_err.h"
#include "esp_event.h"
#include "esp_log.h"

#include "infra_compat.h"

#include "linkkit_solo.h"
#include "factory_restore.h"
#include "hal_motor.h"

#include "conn_mgr.h"

#include "hal_key.h"
#include "transport_uart.h"

#include "hal_DS1302.h"
#include "freertos/timers.h"

#include "hal_led.h"
#include "hal_ir.h"

#include "product_config.h"

#include "iot_export_timer.h"


static const char *TAG = "app main";

static bool linkkit_started = false;

#ifdef CONFIG_AOS_TIMER_SERVICE
#define NUM_OF_PROPERTYS 1 /* <=30 dont add timer property */
const char *control_targets_list[NUM_OF_PROPERTYS] = {"manualFeeding"};
static int num_of_tsl_type[NUM_OF_TSL_TYPES] = {1, 0, 0}; /* 1:int/enum/bool; 2:float/double; 3:text/date */
#define NUM_OF_COUNTDOWN_LIST_TARGET 1  /* <=10 */
const char *countdownlist_target_list[NUM_OF_COUNTDOWN_LIST_TARGET] = {"manualFeeding"};
#define NUM_OF_LOCAL_TIMER_TARGET 1  /* <=5 */
const char *localtimer_target_list[NUM_OF_LOCAL_TIMER_TARGET] = {"manualFeeding"};
#endif

#define  LEDTYPE_NUM 4
#define  NORMAL_MODE 0
#define  AWSS_START 1
#define  OFFLINE_MODE 2
#define  OFFLINE_ERR_MODE 3
uint32_t Rhythmbit1[4]={200,200,200,9400};        /*工作模式*/
uint32_t Rhythmbit2[2]={1200,200};                 /*配网*/
uint32_t Rhythmbit3[4]={200,9800};                /*离线工作*/
uint32_t Rhythmbit4[2]={200,200};                 /*离线工作无配置*/
Rhythm_t led_config[LEDTYPE_NUM]={{Rhythmbit1, 4, 0},
                                  {Rhythmbit2, 2, 0},
                                  {Rhythmbit3, 4, 0},
                                  {Rhythmbit4, 2, 0}};

void user_led_work_mode_set(uint8_t new_working_mode);

static esp_err_t wifi_event_handle(void *ctx, system_event_t *event)
{
    switch (event->event_id) {
        case SYSTEM_EVENT_STA_GOT_IP:
            if (linkkit_started == false) {
                xTaskCreate((void (*)(void *))linkkit_main, "feeder", 10240, NULL, 5, NULL);
                linkkit_started = true;
            }
            break;

        default:
            break;
    }

    return ESP_OK;
}

static void linkkit_event_monitor(int event)
{
    switch (event) {
        case IOTX_AWSS_START: // AWSS start without enbale, just supports device discover
            // operate led to indicate user
            ESP_LOGI(TAG, "IOTX_AWSS_START");
			user_led_work_mode_set(AWSS_START);
            break;

        case IOTX_AWSS_ENABLE: // AWSS enable, AWSS doesn't parse awss packet until AWSS is enabled.
            ESP_LOGI(TAG, "IOTX_AWSS_ENABLE");
            // operate led to indicate user
            break;

        case IOTX_AWSS_LOCK_CHAN: // AWSS lock channel(Got AWSS sync packet)
            ESP_LOGI(TAG, "IOTX_AWSS_LOCK_CHAN");
            // operate led to indicate user
            break;

        case IOTX_AWSS_PASSWD_ERR: // AWSS decrypt passwd error
            ESP_LOGE(TAG, "IOTX_AWSS_PASSWD_ERR");
            // operate led to indicate user
            break;

        case IOTX_AWSS_GOT_SSID_PASSWD:
            ESP_LOGI(TAG, "IOTX_AWSS_GOT_SSID_PASSWD");
            // operate led to indicate user
            break;

        case IOTX_AWSS_CONNECT_ADHA: // AWSS try to connnect adha (device
            // discover, router solution)
            ESP_LOGI(TAG, "IOTX_AWSS_CONNECT_ADHA");
            // operate led to indicate user
            break;

        case IOTX_AWSS_CONNECT_ADHA_FAIL: // AWSS fails to connect adha
            ESP_LOGE(TAG, "IOTX_AWSS_CONNECT_ADHA_FAIL");
            // operate led to indicate user
            break;

        case IOTX_AWSS_CONNECT_AHA: // AWSS try to connect aha (AP solution)
            ESP_LOGI(TAG, "IOTX_AWSS_CONNECT_AHA");
            // operate led to indicate user
            break;

        case IOTX_AWSS_CONNECT_AHA_FAIL: // AWSS fails to connect aha
            ESP_LOGE(TAG, "IOTX_AWSS_CONNECT_AHA_FAIL");
            // operate led to indicate user
            break;

        case IOTX_AWSS_SETUP_NOTIFY: // AWSS sends out device setup information
            // (AP and router solution)
            ESP_LOGI(TAG, "IOTX_AWSS_SETUP_NOTIFY");
            // operate led to indicate user
            break;

        case IOTX_AWSS_CONNECT_ROUTER: // AWSS try to connect destination router
            ESP_LOGI(TAG, "IOTX_AWSS_CONNECT_ROUTER");
            // operate led to indicate user
            break;

        case IOTX_AWSS_CONNECT_ROUTER_FAIL: // AWSS fails to connect destination
            // router.
            ESP_LOGE(TAG, "IOTX_AWSS_CONNECT_ROUTER_FAIL");
            // operate led to indicate user
            break;

        case IOTX_AWSS_GOT_IP: // AWSS connects destination successfully and got
            // ip address
            ESP_LOGI(TAG, "IOTX_AWSS_GOT_IP");
            // operate led to indicate user
            break;

        case IOTX_AWSS_SUC_NOTIFY: // AWSS sends out success notify (AWSS
            // sucess)
            ESP_LOGI(TAG, "IOTX_AWSS_SUC_NOTIFY");
            // operate led to indicate user
            break;

        case IOTX_AWSS_BIND_NOTIFY: // AWSS sends out bind notify information to
            // support bind between user and device
            ESP_LOGI(TAG, "IOTX_AWSS_BIND_NOTIFY");
            // operate led to indicate user
            break;

        case IOTX_AWSS_ENABLE_TIMEOUT: // AWSS enable timeout
            // user needs to enable awss again to support get ssid & passwd of router
            ESP_LOGW(TAG, "IOTX_AWSS_ENALBE_TIMEOUT");
            // operate led to indicate user
            break;

        case IOTX_CONN_CLOUD: // Device try to connect cloud
            ESP_LOGI(TAG, "IOTX_CONN_CLOUD");
            // operate led to indicate user
            break;

        case IOTX_CONN_CLOUD_FAIL: // Device fails to connect cloud, refer to
            // net_sockets.h for error code
            ESP_LOGE(TAG, "IOTX_CONN_CLOUD_FAIL");
            // operate led to indicate user
            break;

        case IOTX_CONN_CLOUD_SUC: // Device connects cloud successfully
            ESP_LOGI(TAG, "IOTX_CONN_CLOUD_SUC");
            // operate led to indicate user
            user_led_work_mode_set(NORMAL_MODE);
            break;

        case IOTX_RESET: // Linkkit reset success (just got reset response from
            // cloud without any other operation)
            ESP_LOGI(TAG, "IOTX_RESET");
            // operate led to indicate user
            break;

        default:
            break;
    }
}
void user_key_handle(uint8_t event)
{
	ESP_LOGI(TAG, "user_key_handle %d",event);
	if(KEY_GPIO_L_PRESS_EVT == event)
	{
		factory_restore_handle_butten();
		rom_software_reboot();
	}
}

#ifdef CONFIG_AOS_TIMER_SERVICE
static void timer_service_cb(const char *report_data, const char *property_name, int i_value,
                             double d_value, const char * s_value, int prop_idx)
{
    if (prop_idx >= NUM_OF_CONTROL_TARGETS){
        ESP_LOGI(TAG, "ERROR: prop_idx=%d is out of limit=%d", prop_idx, NUM_OF_CONTROL_TARGETS);
    }
    if (property_name != NULL){    /* set value to device */
        ESP_LOGI(TAG, "timer event callback: property_name=%s prop_idx=%d, i_value=%d",
					property_name, prop_idx, i_value);
		motorControl_timer(5, i_value);
    }
    return;
}
#endif

static int user_ntp_event_callback(int readlen)
{
	if(readlen == 0)
		user_led_work_mode_set(OFFLINE_ERR_MODE);
    return 0;
}


void printtest( TimerHandle_t xTimer )
{

}

void user_led_work_mode_set(uint8_t new_working_mode)
{
	uint8_t old_working_mode;
	led_work_mode_get(&old_working_mode);
	ESP_LOGI(TAG, "old:%d,new:%d", old_working_mode,new_working_mode);
	if (new_working_mode == AWSS_START)
	{
		led_work_mode_set(new_working_mode);
		return;
	}
	if(old_working_mode==OFFLINE_ERR_MODE )
	{
		return;
	}
	led_work_mode_set(new_working_mode);
}

void app_main()
{
    //factory_restore_init();
	bool ret=false;

    motorInit();
	key_gpio_init(user_key_handle);
	transport_uart_handle_init();

    conn_mgr_init();
    conn_mgr_register_wifi_event(wifi_event_handle);

    iotx_event_regist_cb(linkkit_event_monitor);    // awss callback

    IOT_SetLogLevel(IOT_LOG_INFO);
	
	led_gpio_init(led_config,LEDTYPE_NUM);
	user_led_work_mode_set(OFFLINE_MODE);

	ds1302_gpio_init();
	ret = ds1302_syn_systime(1);
    if(ret == false)
		user_led_work_mode_set(OFFLINE_ERR_MODE);
	
	ir_gpio_init();

    conn_mgr_start();
	

	TimerHandle_t  testtimmer = xTimerCreate(
						/* 定时器的名称 */
						"OneShot",
						/* 定时周期 */
						1000/portTICK_RATE_MS,
						/* 设置为pdFALSE表示一次性定时器 */
						pdTRUE,
						/* ID初始为0 */
						0,
						/* 回调函数 */
						printtest);
    xTimerStart(testtimmer,pdMS_TO_TICKS(1000));

#ifdef CONFIG_AOS_TIMER_SERVICE
	int tm_ret = timer_service_init( control_targets_list, NUM_OF_PROPERTYS,
						countdownlist_target_list,	NUM_OF_COUNTDOWN_LIST_TARGET,
						localtimer_target_list,NUM_OF_LOCAL_TIMER_TARGET,
						timer_service_cb, num_of_tsl_type, user_ntp_event_callback
						);
#endif

}
