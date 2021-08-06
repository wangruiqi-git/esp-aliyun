
/*********************************************************************
* INCLUDES
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
	

#include "esp_system.h"

#include "hal_ir.h"

uint32_t g_ir_times=0;

/*********************************************************************
* TYPEDEFS
*/
//中断处理
static void ir_isr_handler(void *arg)
{
	uint32_t gpio_status = GPIO_REG_READ(GPIO_STATUS_ADDRESS);
	
	GPIO_REG_WRITE(GPIO_STATUS_W1TC_ADDRESS, gpio_status);
	uint32_t pin_num=(uint32_t)arg;
	g_ir_times++;
}

void start_count(void)
{
	g_ir_times=0;
}
uint32_t get_count(void)
{
	return g_ir_times;
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
void ir_gpio_init(void)
{
	gpio_config_t io_conf;
	
	memset(&io_conf,0,sizeof(gpio_config_t));

	//interrupt of falling edge
	io_conf.intr_type = GPIO_INTR_NEGEDGE;
	//bit mask of the pins
	io_conf.pin_bit_mask = GPIO_IR_PIN_SEL;
	//set as input mode
	io_conf.mode = GPIO_MODE_INPUT;
	//enable pull-up mode
	io_conf.pull_up_en = GPIO_PULLUP_ENABLE;
	//io_conf.pull_down_en = GPIO_PULLDOWN_ENABLE;
	gpio_config(&io_conf);
	
	//install gpio isr service
    gpio_install_isr_service(0);
    //hook isr handler for specific gpio pin
    gpio_isr_handler_add(GPIO_IR_IO, ir_isr_handler, (void *) GPIO_IR_IO);
}
