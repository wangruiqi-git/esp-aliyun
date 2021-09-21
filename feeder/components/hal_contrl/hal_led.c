
#define LED_C_

#include "hal_led.h"
#include "esp8266/eagle_soc.h"
//#include "osapi.h"
#include "driver/gpio.h"
#include "rom/ets_sys.h"

#include "FreeRTOS.h"
#include "freertos/timers.h"
#include "esp_log.h"

#define level_on  0
#define level_off 1


static Rhythm_t *pri_led_config=NULL;
static uint8_t  num_working_mode=0;

static uint8_t  working_mode=0;
static TimerHandle_t  led_Timer;

static const char *TAG = "hal_led";

static void led_timer_handler( TimerHandle_t xTimer );


//初始化引脚,移植其它单片机请替换内部函数
void led_gpio_init(Rhythm_t input[],uint8_t lenth)
{
	num_working_mode=lenth;
	pri_led_config=input;

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
	uint8_t pos=0;
	uint8_t num =0;
	uint32_t Rhythmbit=0;
	uint8_t led_lvl=0;

	xTimerStop(xTimer,0);
	pos = pri_led_config[working_mode].RhythmPos;
	num = pri_led_config[working_mode].RhythmNum;
	Rhythmbit = pri_led_config[working_mode].Rhythmbit[pos];
	led_lvl = (pos)%2;
	//ESP_LOGI(TAG, "pos:%d,num:%d,Rhythmbit:%d,led_lvl:%d",pos,num,Rhythmbit,led_lvl);
	
	pri_led_config[working_mode].RhythmPos = (pos+1)%num;
	
	led_level(led_lvl);
	
	xTimerChangePeriod(xTimer,
						pdMS_TO_TICKS(Rhythmbit),
						pdMS_TO_TICKS(50));
	xTimerStart(xTimer,pdMS_TO_TICKS(50));
}

void led_work_mode_get(uint8_t* n_working_mode)
{
	*n_working_mode = working_mode;
}

void led_work_mode_set(uint8_t      n_working_mode)
{
	if( n_working_mode >= num_working_mode||n_working_mode<0)
	{
		ESP_LOGI(TAG, "n_working_mode:%d",n_working_mode);
		return;
	}
	working_mode = n_working_mode;
	pri_led_config[working_mode].RhythmPos=0;
	led_timer_handler(led_Timer);
}


