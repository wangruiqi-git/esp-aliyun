#ifndef _HAL_IR_H_
#define _HAL_IR_H_

#include "driver/gpio.h"


/*********************************************************************
* MACROS
*/
#define GPIO_IR_IO     GPIO_NUM_4
#define GPIO_IR_PIN_SEL  (1ULL<<GPIO_IR_IO)


//定义事件类型


/*********************************************************************
* CONSTANTS
*/


/*********************************************************************
* TYPEDEFS
*/


void ir_gpio_init(void);

void start_count(void);
uint32_t get_count(void);

#endif
