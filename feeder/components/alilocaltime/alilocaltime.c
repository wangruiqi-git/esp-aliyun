#include "alilocaltime.h"
#include "hal_motor.h"

#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"

#include "esp_log.h"
#include "product_config.h"
char g_localtime_config[IOTX_LOCALTIME_CONF_LEN+1];

//static TimerHandle_t *localtime_timer;

static const char *TAG = "alilocaltime";
#if 0
/* 距离闹钟下一次激发的时间（min） */
int get_expiry_time(ali_localtime_config_t	 *alarm_task)
{
   /* 首先获取当前时间 */
   
   time_t    t;
   struct tm now;

   time(&t);
   localtime_r(&t, &now);

   int ret = 0;

   if(alarm_task->cron_tm.min != 0xFFFFFFFF)
   {
        ret += cron_tm.min - now.tm_min;
   }

   if(alarm_task->cron_tm.hour != 0xFFFFFFFF)
   {
        int flag = 0;

        for(int i=0; i<24; i++)
        {
            if(i == alarm_task->cron_tm.hour){
                flag=1;
                break;
            }
        }

        if(flag){
            ret += 60*(i-now.tm_hour);
        }
   }

   /* 找到wday或者mday中距离今天最近的一天 */
   int t = ret<0?1:0;
   
   int d_wday=0;
   int d_mday=0;

   for(int i=0; i<7; i++)
   {
        int wday = now.tm_wday + t +i;
        if(1<<(wday%7) & alarm_task->cron_tm.wday){
            d_wday=i+t;
            break;
        }
   }

   for(int i=0; i<30; i++)
   {
        int mday = now.tm_mday + t +i;

        if(1<<(mday%30) & alarm_task->cron_tm_.mday){
            d_mday = i+t;
            break;
        }
   }

   int d = d_mday<d_wday?d_mday:d_wday;

   ret += d*24*60;
  
   return ret;
}

int set_timer(void)
{
	ali_localtime_config_t	 *alarm_task;

    int t = get_expiry_time(alarm_task);
    if(t <= 0) return -1; // 不需要设置下一次闹钟，不重复的闹钟，且时间已过

    alarm_task->xTimer = xTimerCreate( "timer",
                                        t / portTICK_RATE_MS,
                                        1,
                                        (void*)alarm_task,
                                        timer_task_callback );

    if(alarm_task->xTimer == NULL)
    {
        printf("!!! timer created failed\n");
        return -1;
    }
    else
    {
        xTimerStart(alarm_task->xTimer, 0);
    }

    return 0;
}
#endif
void ali_localtime_set(void)
{
	HAL_SetLocalTimeConf((char*)g_localtime_config);
	//start_timer();
	return;
}


void ali_localtime_init(void)
{
	//memset(g_localtime_config, 0, 5*sizeof(ali_localtime_config_t));
	
	HAL_GetLocalTimeConf((char*)g_localtime_config);
	
	//ali_localtime_config_t *p_l_c=(ali_localtime_config_t *)g_localtime_config;
	//ESP_LOGI(TAG, "ali_localtime_init %d",p_l_c[0].time_attr[0]);


	//start_timer();
	return;
}


