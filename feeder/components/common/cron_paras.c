
/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */
#include "cron_paras.h"
#include "ctype.h"


#include "esp_log.h"

#ifndef ARRAY_SIZE
# define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))
#endif

static const char *TAG = "cron_paras";

static const char DowAry[]  =
	"sun""mon""tue""wed""thu""fri""sat"
;

static const char MonAry[]  =
	"jan""feb""mar""apr""may""jun""jul""aug""sep""oct""nov""dec"
;

static bool cron_str_seperate(char *str, char *pp[CRON_ATTR_NUM], char *delims)
{
	if(str==NULL||pp==NULL||delims==NULL)
		return false;
	
	char *p = strtok(str, delims);
	int i_Timer_val=0;
	while(p)
	{
		pp[i_Timer_val]=p;
		p = strtok(NULL, delims);
		i_Timer_val++;
	}
	if(i_Timer_val < CRON_ATTR_NUM-1)
		return false;
	return true;
}


static void ParseField(char *ary, int modvalue, int off,
				const char *names, char *ptr)
/* 'names' is a pointer to a set of 3-char abbreviations */
{
	if(ptr==NULL||ary==NULL)
		return;
	
	char *base = ptr;
	int n1 = -1;
	int n2 = -1;

	// this can't happen due to config_read()
	/*if (base == NULL)
		return;*/

	while (1) {
		int skip = 0;

		/* Handle numeric digit or symbol or '*' */
		if (*ptr == '*') {
			n1 = 0;  /* everything will be filled */
			n2 = modvalue - 1;
			skip = 1;
			++ptr;
		} else if (isdigit((int)*ptr)) {
			char *endp;
			if (n1 < 0) {
				n1 = strtol(ptr, &endp, 10) + off;
			} else {
				n2 = strtol(ptr, &endp, 10) + off;
			}
			ptr = endp; /* gcc likes temp var for &endp */
			skip = 1;
		} else if (names) {
			int i;

			for (i = 0; names[i]; i += 3) {
				/* was using strncmp before... */
				if (strncasecmp(ptr, &names[i], 3) == 0) {
					ptr += 3;
					if (n1 < 0) {
						n1 = i / 3;
					} else {
						n2 = i / 3;
					}
					skip = 1;
					break;
				}
			}
		}

		/* handle optional range '-' */
		if (skip == 0) {
			goto err;
		}
		if (*ptr == '-' && n2 < 0) {
			++ptr;
			continue;
		}

		/*
		 * collapse single-value ranges, handle skipmark, and fill
		 * in the character array appropriately.
		 */
		if (n2 < 0) {
			n2 = n1;
		}
		if (*ptr == '/') {
			char *endp;
			skip = strtol(ptr + 1, &endp, 10);
			ptr = endp; /* gcc likes temp var for &endp */
		}

		/*
		 * fill array, using a failsafe is the easiest way to prevent
		 * an endless loop
		 */
		{
			int s0 = 1;
			int failsafe = 1024;

			--n1;
			do {
				n1 = (n1 + 1) % modvalue;

				if (--s0 == 0) {
					ary[n1 % modvalue] = 1;
					s0 = skip;
				}
				if (--failsafe == 0) {
					goto err;
				}
			} while (n1 != n2);
		}
		if (*ptr != ',') {
			break;
		}
		++ptr;
		n1 = -1;
		n2 = -1;
	}

	if (*ptr) {
 err:
//log
		return;
	}
#if 0
	int i;
	for (i = 0; i < modvalue; ++i)
	{
		ESP_LOGI(TAG, "ParseField bool:%d  no:%d",(unsigned char)ary[i],i);
	}
#endif
	return;
}

static void FixDayDow(CronLine *line)
{
	unsigned i;
	int weekUsed = 0;
	int daysUsed = 0;

	for (i = 0; i < ARRAY_SIZE(line->cl_Dow); ++i) {
		if (line->cl_Dow[i] == 0) {
			weekUsed = 1;
			break;
		}
	}
	for (i = 0; i < ARRAY_SIZE(line->cl_Days); ++i) {
		if (line->cl_Days[i] == 0) {
			daysUsed = 1;
			break;
		}
	}
	if (weekUsed != daysUsed) {
		if (weekUsed)
			memset(line->cl_Days, 0, sizeof(line->cl_Days));
		else /* daysUsed */
			memset(line->cl_Dow, 0, sizeof(line->cl_Dow));
	}
}


bool cron_str_to_struct(CronLine *cron_item, char *ptr ,char *delims)
{
	if(cron_item==NULL||ptr==NULL||delims==NULL)
		return false;
	
	char *pp[CRON_ATTR_NUM];
	bool res = cron_str_seperate(ptr, pp, delims);
	if(res==false)
		return false;

	ParseField(cron_item->cl_Mins, 60, 0, NULL, pp[0]);
	ParseField(cron_item->cl_Hrs, 24, 0, NULL, pp[1]);
	ParseField(cron_item->cl_Days, 32, 0, NULL, pp[2]);
	ParseField(cron_item->cl_Mons, 12, -1, MonAry, pp[3]);
	ParseField(cron_item->cl_Dow, 7, 0, DowAry, pp[4]);

	FixDayDow(cron_item);
	return true;
}
//cron_str_to_time_t

#if 0


struct_to_time_t
#endif

