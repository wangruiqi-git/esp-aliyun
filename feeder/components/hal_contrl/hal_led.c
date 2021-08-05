
#define LED_C_

#include "hal_led.h"
#include "esp8266/eagle_soc.h"
//#include "osapi.h"
#include "driver/gpio.h"
#include "rom/ets_sys.h"

#include "FreeRTOS.h"
#include "freertos/timers.h"

#define level_on  0
#define level_off 1


static Rhythm_t led_config[3]={ {{50,3000},0},
							   {{500,500},1},
							   {{50,200},2}};
static uint8_t  working_mode=0;
static TimerHandle_t  led_Timer;
static uint8_t  current_pos=0;

static void led_timer_handler( TimerHandle_t xTimer );


//初始化引脚,移植其它单片机请替换内部函数
void led_gpio_init(Rhythm_t input[],uint8_t lenth)
{
    gpio_config_t io_conf;
	io_conf.intr_type = GPIO_INTR_DISABLE;
	//设置为输出模式
	io_conf.mode = GPIO_MODE_OUTPUT;
	//管脚的位
	io_conf.pin_bit_mask = GPIO_LED_OUTPUT_PIN_SEL;
	io_conf.pull_down_en = 1;
	io_conf.pull_up_en = 1;
	gpio_config(&io_conf);
	
	led_off();

	led_Timer = xTimerCreate(
						/* 定时器的名称 */
						"led_timer",
						/* 定时周期 */
						1000,
						/* 设置为pdFALSE表示一次性定时器 */
						pdFALSE,
						/* ID初始为0 */
						0,
						/* 回调函数 */
						led_timer_handler);
	
	xTimerStart(led_Timer,pdMS_TO_TICKS(50));
}


//设置引脚输出高低电平,移植其它单片机请替换内部函数
void led_on(void)
{
    gpio_set_level(GPIO_LED_IO, level_on);
}
//设置引脚输出高低电平,移植其它单片机请替换内部函数
void led_off(void)
{
    gpio_set_level(GPIO_LED_IO, level_off);
}

//设置引脚输出高低电平,移植其它单片机请替换内部函数
void led_level(uint8_t   level)
{
    gpio_set_level(GPIO_LED_IO, level);
}



void led_timer_handler( TimerHandle_t xTimer )
{
	xTimerStop(xTimer,0);

	led_level(current_pos);

	xTimerChangePeriod(xTimer,
						pdMS_TO_TICKS(led_config[working_mode].Rhythmbit[current_pos]),
						pdMS_TO_TICKS(50));
	xTimerStart(xTimer,pdMS_TO_TICKS(50));
	
	if(current_pos)
		current_pos=0;
	else
		current_pos=1;
}

void led_work_mode_set(uint8_t      n_working_mode)
{
	working_mode = n_working_mode;
	led_timer_handler(led_Timer);

}


