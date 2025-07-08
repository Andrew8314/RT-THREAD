/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2025-07-07     Y8314       the first version
 */
#include <drv_matrix_led.h>

#define DBG_TAG "rgb_ap3216"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>
/* define LED  */
enum{
    EXTERN_LED_0,
    EXTERN_LED_1,
    EXTERN_LED_2,
    EXTERN_LED_3,
    EXTERN_LED_4,
    EXTERN_LED_5,
    EXTERN_LED_6,
    EXTERN_LED_7,
    EXTERN_LED_8,
    EXTERN_LED_9,
    EXTERN_LED_10,
    EXTERN_LED_11,
    EXTERN_LED_12,
    EXTERN_LED_13,
    EXTERN_LED_14,
    EXTERN_LED_15,
    EXTERN_LED_16,
    EXTERN_LED_17,
    EXTERN_LED_18,
};
extern rt_mq_t mq_ps_data1;
rt_thread_t led_matrix_thread;
static rt_thread_t tid1 = RT_NULL;

void rgb1_entry(void *parameter)
{
    pixel_rgb_t Light   = WHITE;

    led_matrix_clear();
    //等待AP3216初始化完成
    rt_thread_mdelay(2000);

    while (1)
    {
        rt_uint16_t ps_data;

        if (rt_mq_recv(mq_ps_data1, &ps_data, sizeof(ps_data), RT_WAITING_NO) == RT_EOK)
        {
            //rt_kprintf("current ps data: %d.", ps_data);

            if (ps_data < 800) {
                Light = DARK;      //关
            }
            else if(ps_data < 900){
                Light = WHITE;
            }
            else if (ps_data < 1000){
                Light = BLUE;
            }
            else {
                Light = RED;
            }
            for (int i = EXTERN_LED_0; i <= EXTERN_LED_18; i++)
            {
                led_matrix_set_color(i, Light);
            }
            led_matrix_reflash();
        }
        rt_thread_mdelay(20);
    }
}
int thread_rgb1(void)
{

    tid1 = rt_thread_create("rgb1_entry", rgb1_entry, RT_NULL, 1024, 25, 10);
    if (tid1 != RT_NULL)
    {
        rt_thread_startup(tid1);
        rt_kprintf("rgb1 thread_startup\n");
    }
    else
    {
        LOG_E("rgb1 thread create failed");
    }

    return 0;
}
MSH_CMD_EXPORT(thread_rgb1, thread_rgb1_ap3216);


