/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2025-07-06     Y8314       the first version
 */
#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>
#include "aht10.h"

#define DBG_TAG "ath10"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>

/* Thread */
#define THREAD_PRIORITY         25
#define THREAD_STACK_SIZE       1024
#define THREAD_TIMESLICE        10

static float humidity, temperature;
static aht10_device_t dev;
/* 总线名称 */
static const char *i2c_bus_name = "i2c3";

//新建一个线程专门用于读取传感器温湿度数据并将数据分别放入两个消息队列中
static rt_thread_t tid1 = RT_NULL;
rt_mq_t mq_hum = RT_NULL;
rt_mq_t mq_tem = RT_NULL;

/* 定义一个线程入口 */
static void aht10_entry(void *parameter)
{
    while (1)
    {
        /* 读取湿度 */
        humidity = aht10_read_humidity(dev);
       // LOG_D("humidity   : %d.%d %%", (int)humidity, (int)(humidity * 10) % 10);
        rt_mq_urgent(mq_hum, &humidity, sizeof(humidity));

        /* 读取温度 */
        temperature = aht10_read_temperature(dev);
      // LOG_D("temperature: %d.%d", (int)temperature, (int)(temperature * 10) % 10);
        rt_mq_urgent(mq_tem, &temperature, sizeof(temperature));
        rt_thread_mdelay(200);
    }
}

void thread_aht21(void)
{

    /* 初始化 aht10 */
    dev = aht10_init(i2c_bus_name);

    // 初始化消息队列
    mq_hum = rt_mq_create("mq_hum", 10, sizeof(humidity), RT_IPC_FLAG_FIFO);
    mq_tem = rt_mq_create("mq_tem", 10, sizeof(temperature), RT_IPC_FLAG_FIFO);

    // 创建线程
    tid1 = rt_thread_create("thread_aht10", aht10_entry, RT_NULL, THREAD_STACK_SIZE, THREAD_PRIORITY, THREAD_TIMESLICE);
    if (tid1 != RT_NULL)
    {
        rt_thread_startup(tid1);
        rt_kprintf("aht21 thread_startup\n");
    }
    else
    {
        LOG_E("ath21 thread create failed");
    }
}

MSH_CMD_EXPORT(thread_aht21, thread_aht21);


