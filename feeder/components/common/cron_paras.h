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
#ifndef _CRON_PARAS_H__
#define _CRON_PARAS_H__
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct CronLine {
	char cl_Dow[7];                 /* 0-6, beginning sunday */
	char cl_Mons[12];               /* 0-11 */
	char cl_Hrs[24];                /* 0-23 */
	char cl_Days[32];               /* 1-31 */
	char cl_Mins[60];               /* 0-59 */
} CronLine;


#define CRON_ATTR_NUM 6


bool cron_str_to_struct(CronLine *cron_item, char *ptr ,char *delims);


#if 0

char str[]="3 1 * * *";
CronLine cron_item={0};
cron_str_to_struct(&cron_item, str," ");
CronLine cron_item2={0};
char str2[]="3 1 * * 1,2,3,4,5,6,7";
cron_str_to_struct(&cron_item2, str2," ");


cron_str_to_time_t
struct_to_time_t
#endif

#endif
