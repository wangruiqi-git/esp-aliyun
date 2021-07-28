#include "driver/gpio.h"


/*********************************************************************
* MACROS
*/
#define GPIO_INPUT_IO_0     GPIO_NUM_0
#define GPIO_INPUT_PIN0_SEL  (1ULL<<GPIO_INPUT_IO_0)
#define GPIO_INPUT_IO_14     GPIO_NUM_14
#define GPIO_INPUT_PIN14_SEL  (1ULL<<GPIO_INPUT_IO_14)

//定义事件类型
#define KEY_GPIO_LL_PRESS_EVT		0x01	//超长按事件
#define KEY_GPIO_L_PRESS_EVT		0x02	//长按事件
#define KEY_GPIO_S_PRESS_EVT		0x03	//短按事件

/*********************************************************************
* CONSTANTS
*/


/*********************************************************************
* TYPEDEFS
*/

typedef void (*key_click_cb_t)(uint8_t event);

void key_gpio_init(key_click_cb_t cb);


