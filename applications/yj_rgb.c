/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2025-07-07     Y8314       the first version
 */
#include <rtthread.h>
#include <rtdevice.h>
#include <drv_neo_pixel.h>

#define DBG_TAG "yj_rgb"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>
/* Thread */
#define THREAD_PRIORITY         27
#define THREAD_STACK_SIZE       1024
#define THREAD_TIMESLICE        5

#define LED_NUMS       (19 + 30)    /* LED 的数目 */
#define LED_BRIGHTNESS 150   /* LED 的最大亮度 */
#define BREATHING_RATE 60    /* 呼吸灯变化速率，数值越小变化越快 */

neo_pixel_ops_t *neo_ops;
uint8_t rightness = 0;  /* 当前亮度 */
uint8_t direction = 1;   /* 亮度变化方向，1 表示增加，0 表示减少 */

static rt_thread_t tid1 = RT_NULL;

void rgb_entry(void *parameter)
{
    while (1)
    {
        /* 设置所有LED的亮度 */
        neo_ops->setBrightness(rightness);

        /* 设置所有LED为白色 */
        neo_ops->fill(0x00FF00, 0, LED_NUMS);
        /*      0xFF0000,  红色
                0x00FF00, 绿色
                0x0000FF, 蓝色
                0xFFFF00, 黄色
                0x00FFFF, 青色
                0xFF00FF   品红     */
        /* 显示当前亮度的LED状态 */
        neo_ops->show();

        /* 根据方向改变亮度 */
        if (direction)
        {
            rightness += 5; /* 逐渐增加亮度 */
            if (rightness >= LED_BRIGHTNESS)
            {
                rightness = LED_BRIGHTNESS;
                direction = 0; /* 达到最大亮度，开始减少 */
            }
        }
        else
        {
            rightness -= 5; /* 逐渐减少亮度 */
            if (rightness <= 0)
            {
                rightness = 0;
                direction = 1; /* 达到最小亮度，开始增加 */
            }
        }

        /* 控制呼吸灯变化速率 */
        rt_thread_mdelay(BREATHING_RATE);
    }

}

int thread_rgb(void)
{
    /* 初始化 neo_pixel 库 */
    neo_pixel_init(&neo_ops, LED_NUMS);
    /* 等待驱动初始化完成 */
    rt_thread_mdelay(10);

    tid1 = rt_thread_create("thread_rgb", rgb_entry, RT_NULL, THREAD_STACK_SIZE, THREAD_PRIORITY, THREAD_TIMESLICE);
    if (tid1 != RT_NULL)
        rt_thread_startup(tid1);
    return 0;
}
MSH_CMD_EXPORT(thread_rgb, thread_rgb);


