
#ifndef APP_INCLUDE_DRIVER_LED_H_
#define APP_INCLUDE_DRIVER_LED_H_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include "infra_types.h"


//移植其它单片机,请修改下面的程序
/*GPIO14 -- RST*/
#define GPIO_LED_IO     GPIO_NUM_2
#define GPIO_LED_OUTPUT_PIN_SEL  (1ULL<<GPIO_LED_IO)
//#define DS1302_RST_MUX PERIPHS_IO_MUX_MTMS_U
//#define DS1302_RST_GPIO 14
//#define DS1302_RST_FUNC FUNC_GPIO14


typedef struct Rhythm
{
	uint32_t *Rhythmbit;
	uint8_t RhythmNum;
	uint8_t RhythmPos;
} Rhythm_t;

//初始化引脚,移植其它单片机请替换内部函数
void led_gpio_init(Rhythm_t input[],uint8_t lenth);

//设置GPIO引脚输出高低电平,移植其它单片机请替换内部函数
void led_on(void);
void led_off(void);
void led_level(uint8_t   level);

void led_work_mode_get(uint8_t* n_working_mode);

void led_work_mode_set(uint8_t  n_working_mode);

#endif /* APP_INCLUDE_DRIVER_LED_H_ */
