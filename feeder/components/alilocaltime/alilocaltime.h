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
#ifndef _ALI_LOCAL_TIME_H__
#define _ALI_LOCAL_TIME_H__
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "infra_types.h"
#include "infra_defs.h"
#include <stdbool.h>

#include "product_config.h"

//#include "infra_compat.h"

/* cron 格式时间表示 */
typedef struct
{
    uint8_t     min;    // minute,          0xFFFFFFFF表示*
    uint8_t     hour;   // hour，           0xFFFFFFFF表示*
    uint8_t     wday;   // day of week，    0xFFFFFFFF表示*，bit[0~6]  表示周一到周日,如周一到周五每天响铃，则0x1F
    uint8_t     mday;   // day of month，   0xFFFFFFFF表示*, bit[0~31] 表示1~31日,每月1,3,5号响铃，则0x15
    int     mon;    // month，          0xFFFFFFFF表示*，bit[0~11] 表示1~12月
} cron_tm_t;

typedef struct
{
	cron_tm_t cron_tm;
	int       action;     // 响应操作，对于灯控产品来说，action可以表示开关、颜色、场景等等
} cron_task_t;

/* 闹钟任务 */
typedef struct
{
	int             id;         // 任务ID
    cron_task_t       cron_task;    // cron格式时间
    //TimerHandle_t   xTimer;     // 定时器句柄
} alarm_task_t;

#define LOCALTIME_NUM 5
//LOCALTIME_NUM*sizeof(cron_task_t)
extern char g_localtime_config[IOTX_LOCALTIME_CONF_LEN + 1];
int HAL_GetLocalTimeConf(char LocalTimeConf[IOTX_LOCALTIME_CONF_LEN + 1]);
int HAL_SetLocalTimeConf(char LocalTimeConf[IOTX_LOCALTIME_CONF_LEN + 1]);

void ali_localtime_set(void);

void ali_localtime_init(void);

#endif
