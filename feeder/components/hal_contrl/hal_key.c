
/*********************************************************************
* INCLUDES
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
	
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
	

	
#include "esp_log.h"
#include "esp_system.h"

#include "hal_key.h"

/*********************************************************************
* TYPEDEFS
*/
typedef void (*post_delay_call_t)(void *arg);


/*********************************************************************
* GLOBAL VARIABLES
*/
       
/*********************************************************************
* LOCAL VARIABLES
*/
static uint64_t  elink_time = 0;
static key_click_cb_t pfnKeyClickCallback = NULL;
static xQueueHandle key_evt_queue = NULL;


/*********************************************************************
* LOCAL FUNCTIONS
*/
//软件定时器实现定时调用指定函数
static void post_delayed_action(int ms, post_delay_call_t action, void *parg)
{
	static os_timer_t os_timer;
	os_timer_disarm(&os_timer);
	os_timer_setfn(&os_timer, action, parg);
	os_timer_arm(&os_timer, ms,0);
}

static void key_poll_func(void *arg)
{
	uint64_t diff;
	uint8_t events = 0;
	//获取引脚状态
	uint32_t pin_num = (uint32_t)arg;
	uint32_t level = gpio_get_level(pin_num);
	
	 //按键未松开
	if (level == 0) {
		post_delayed_action(10, key_poll_func, (void *)pin_num);
	}
	//按键已经松开 
	else {
		//获取按下到松开的间隔时间
		extern uint32_t esp_get_time(void);
		diff = esp_get_time() - elink_time;
		if (diff > 6000000) { /*long long press */
			elink_time = 0;
			events = KEY_GPIO_LL_PRESS_EVT;
		} else if (diff > 2000000) { /* long press */
			elink_time = 0;
			events = KEY_GPIO_L_PRESS_EVT;
		} else if (diff > 40000) { /* short press */
			elink_time = 0;
			events = KEY_GPIO_S_PRESS_EVT;
		} else {
			post_delayed_action(10, key_poll_func, (void *)pin_num);
		}
	}
	//发送消息给task
	if(events)
	{
		xQueueSendFromISR(key_evt_queue, &events, NULL);
		events = 0;
	}
}

static void handle_elink_key(uint32_t pin_num)
{
    
    uint32_t level = gpio_get_level(pin_num);
    if ((level == 0) && (elink_time == 0)) {
		extern uint32_t esp_get_time(void);
        elink_time = esp_get_time();
        key_poll_func((void *)pin_num);
    }
}

//中断处理
static void gpio_isr_handler(void *arg)
{
	uint32_t gpio_status = GPIO_REG_READ(GPIO_STATUS_ADDRESS);
	
	GPIO_REG_WRITE(GPIO_STATUS_W1TC_ADDRESS, gpio_status);

    //if (gpio_status & GPIO_INPUT_PIN0_SEL)
	{
        handle_elink_key((uint32_t)arg);
	}
	
}

static void key_task_handler(void *arg)
{
    uint8_t event;
	ESP_LOGI("KEY","gpio_task_example");
    for (;;) {
        if (xQueueReceive(key_evt_queue, &event, portMAX_DELAY)) {
            	//接收到消息，调用回调函数,在回调函数中对不同的按键类型进行处理
			if(pfnKeyClickCallback != NULL)
				pfnKeyClickCallback(event);	
        }
    }
}

/*********************************************************************
* GLOBAL FUNCTIONS
*/

/*********************************************************************
* @fn          funtion_name
*
* @brief       Add device to descovery list.
*
* @param       pSimpleDescRsp - SimpleDescRsp containing epInfo of new EP.
*
* @return      index of device or 0xFF if no room in list
*/
void key_gpio_init(key_click_cb_t cb)
{
	gpio_config_t io_conf;

	pfnKeyClickCallback = cb;
	
	memset(&io_conf,0,sizeof(gpio_config_t));

	//interrupt of falling edge
	io_conf.intr_type = GPIO_INTR_NEGEDGE;
	//bit mask of the pins
	io_conf.pin_bit_mask = GPIO_INPUT_PIN0_SEL;
	//set as input mode
	io_conf.mode = GPIO_MODE_INPUT;
	//enable pull-up mode
	io_conf.pull_up_en = GPIO_PULLUP_ENABLE;
	gpio_config(&io_conf);
	

	key_evt_queue = xQueueCreate(10, sizeof(uint8_t));
	xTaskCreate(key_task_handler, "key_task_handle", 2048, NULL, 10, NULL);
	
	//install gpio isr service
    gpio_install_isr_service(0);
    //hook isr handler for specific gpio pin
    gpio_isr_handler_add(GPIO_INPUT_IO_0, gpio_isr_handler, (void *) GPIO_INPUT_IO_0);
}
