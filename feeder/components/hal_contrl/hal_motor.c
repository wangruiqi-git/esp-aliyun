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
#include <stdio.h>
#include <sdkconfig.h>

#include "FreeRTOS.h"
#include "freertos/timers.h"
#ifdef CONFIG_IDF_TARGET_ESP8266
#include "driver/pwm.h"
#endif
#include "esp_log.h"
#include "hal_motor.h"
#include "stdlib.h"

static const char *TAG = "hal_motor";

struct pwm_param {
    uint32_t period;
    uint32_t freq;
    uint32_t  duty[PWM_CHANNEL_NUM_MAX];  //PWM_CHANNEL<=8
};

struct pwm_param motorParam;

#ifdef CONFIG_IDF_TARGET_ESP8266
#define PWM_PERIOD        1000
//#define MOTOR_MAX_DUTY      (PWM_PERIOD * 1000 / 45)  //1KHz(1000 us) PWM， duty 范围是： 0 ~ 22222
#define MOTOR_MIN_DUTY      0

#define MCU_D6 12
#define MCU_D7 13

#define MOTO_IO_1 MCU_D6
#define MOTO_IO_2 MCU_D7

#define PWM_IO_NUM 2
#define CHANNEL_0 0
#define CHANNEL_1 1
const uint32_t motor_pin_num[PWM_IO_NUM] = {
    MOTO_IO_1,
    MOTO_IO_2
};

#define MOTOR_MAX_INPUT       20
#define MOTOR_MIN_INPUT        -20
#define MOTOR_SFCT_STA      20

#endif

static TimerHandle_t  xOneShotTimer;


/******************************************************************************
 * FunctionName : user_light_set_duty
 * Description  : set each channel's duty params
 * Parameters   : uint8 duty    : duty 的范围随 PWM 周期改变， 
 *                              最大值为： Period * 1000 /45 。
 *                              例如， 1KHz PWM， duty 范围是： 0 ~ 22222
 *                uint8 channel : MOTOR_A/MOTOR_B
 * Returns      : NONE
*******************************************************************************/
void motorPwmControl(uint8_t m0, uint8_t m1)
{
    uint32_t temp_m0 = 0;
    uint32_t temp_m1 = 0;

    temp_m0 = (uint32_t)((m0 * 1.0 / (MOTOR_SFCT_STA )) * (PWM_PERIOD));
    temp_m1 = (uint32_t)((m1 * 1.0 / (MOTOR_SFCT_STA)) * (PWM_PERIOD));
	
	pwm_stop(0x00);

    if(temp_m0 != motorParam.duty[CHANNEL_0])
    {
        pwm_set_duty(CHANNEL_0, temp_m0);
        pwm_get_duty(CHANNEL_0,& motorParam.duty[CHANNEL_0]);
    }

    if(temp_m1 != motorParam.duty[CHANNEL_1])
    {
        pwm_set_duty(CHANNEL_1, temp_m1);
        pwm_get_duty(CHANNEL_1 , &motorParam.duty[CHANNEL_1]);
    }

    pwm_start();
}

void  motorControl(int8_t status)
{
	if((MOTOR_MIN_INPUT > status) || (MOTOR_MAX_INPUT < status))
    {
        return;
    }

    if(status == 0)
    {
        motorPwmControl(0, 0);
    }
    else if (status > 0)
    {
        motorPwmControl(0, (uint8_t)status);
    }
    else if (status < 0)
    {
        motorPwmControl(abs(status), (uint8_t)0);
    }

}

void  motorControl_timer(int8_t status,int8_t run_second)
{
	motorControl(status);
	xTimerStop(xOneShotTimer,0);
	xTimerChangePeriod(xOneShotTimer,
						pdMS_TO_TICKS(run_second*1000),
						pdMS_TO_TICKS(1000) );
	xTimerStart(xOneShotTimer,pdMS_TO_TICKS(1000));
}


void motorStop( TimerHandle_t xTimer )
{
	motorControl(0);
	xTimerStop(xTimer,0);
}

// phase table, (phase/180)*depth
int16_t phase[PWM_CHANNEL] = {0, 0};
void  motorInit(void)
{
    /* Migrate your driver code */
    
    motorParam.period = PWM_PERIOD;

    uint32_t pwm_duty_init[PWM_CHANNEL] = { 0 };

    /*PIN FUNCTION INIT FOR PWM OUTPUT*/
    pwm_init(motorParam.period, pwm_duty_init, PWM_CHANNEL, motor_pin_num);
	pwm_set_phases(phase);

	xOneShotTimer = xTimerCreate(
						/* 定时器的名称 */
						"OneShot",
						/* 定时周期 */
						3333,
						/* 设置为pdFALSE表示一次性定时器 */
						pdFALSE,
						/* ID初始为0 */
						0,
						/* 回调函数 */
						motorStop);

    return;
}


