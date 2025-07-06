/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2025-07-05     Y8314       the first version
 */
#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>
#include "ap3216c.h"

#define DBG_TAG "ap3216"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>

/* Thread */
#define THREAD_PRIORITY         25
#define THREAD_STACK_SIZE       1024
#define THREAD_TIMESLICE        10

static float brightness, distance;
static ap3216c_device_t dev;
static rt_thread_t tid1 = RT_NULL;

rt_mq_t mq_brightness = RT_NULL;
rt_mq_t mq_ps_data = RT_NULL;

static void ap3216_entry(void *parameter)
{
    while (1)
    {
        rt_uint16_t ps_data;
        float brightness;

        ps_data = ap3216c_read_ps_data(dev);
        // LOG_D("current ps data: %d.", ps_data);
        rt_mq_urgent(mq_ps_data, &ps_data, sizeof(ps_data));

        brightness = ap3216c_read_ambient_light(dev);
        // LOG_D("current brightness: %d.%d(lux).", (int)brightness, ((int)(10 * brightness) % 10));
        rt_mq_urgent(mq_brightness, &brightness, sizeof(brightness));
        rt_thread_mdelay(200);
    }
}

void thread_ap3216(void)
{
    const char* i2c_bus_name = "i2c2";
    dev = ap3216c_init(i2c_bus_name);

    // 初始化消息队列
    mq_brightness = rt_mq_create("mq_bright", 10, sizeof(brightness), RT_IPC_FLAG_FIFO);
    mq_ps_data = rt_mq_create("mq_distance", 10, sizeof(distance), RT_IPC_FLAG_FIFO);

    // 创建线程
    tid1 = rt_thread_create("thread_ap3216", ap3216_entry, RT_NULL, THREAD_STACK_SIZE, THREAD_PRIORITY, THREAD_TIMESLICE);
    if (tid1 != RT_NULL)
    {
        rt_thread_startup(tid1);
        rt_kprintf("creating ap3216 rt_thread_startup\n");
    }
    else
    {
        LOG_E("app2_ap3216_example thread create failed");
    }
}

/* 导出到 msh 命令列表中 */
MSH_CMD_EXPORT(thread_ap3216, thread_ap3216);

//int main(void)
//{
//    ap3216c_device_t dev;
//    const char *i2c_bus_name = "i2c2";
//    int count = 0;
//
//    /* 初始化 ap3216c */
//    dev = ap3216c_init(i2c_bus_name);
//    if (dev == RT_NULL)
//    {
//        LOG_E("The sensor initializes failure.");
//        return 0;
//    }
//
//    while (count++ < 100)
//    {
//        rt_uint16_t ps_data;
//        float brightness;
//
//        /* 读接近感应值 */
//        ps_data = ap3216c_read_ps_data(dev);
//        if (ps_data == 0)
//        {
//            LOG_D("object is not proximity of sensor.");
//        }
//        else
//        {
//            LOG_D("current ps data   : %d.", ps_data);
//        }
//
//        /* 读光照强度值 */
//        brightness = ap3216c_read_ambient_light(dev);
//        LOG_D("current brightness: %d.%d(lux).", (int)brightness, ((int)(10 * brightness) % 10));
//
//        rt_thread_mdelay(1000);
//    }
//    return 0;
//}


