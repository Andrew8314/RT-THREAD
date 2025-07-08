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

#define DBG_TAG "yj_uart_servo"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>

/* Thread */
#define THREAD_PRIORITY         25
#define THREAD_STACK_SIZE       1024
#define THREAD_TIMESLICE        10

extern rt_mq_t mq_uart_rx_cx;
extern rt_mq_t mq_uart_rx_cy;

static rt_thread_t tid1 = RT_NULL;

// 舵机控制范围
#define LEVEL_SERVO_MIN 0
#define LEVEL_SERVO_MAX 180
#define LEVEL_SERVO_CENTER 90

#define VERTICAL_SERVO_MIN 0
#define VERTICAL_SERVO_MAX 180
#define VERTICAL_SERVO_CENTER 110

void level_servo_angle(int cx)
{
   // 根据偏移量调整舵机角度
    uint32_t servo_value = LEVEL_SERVO_CENTER;
    // 更新舵机控制值
    servo_value -= cx - 128;
    // 限制舵机角度范围
    if (servo_value < LEVEL_SERVO_MIN) servo_value = LEVEL_SERVO_MIN;
    if (servo_value > LEVEL_SERVO_MAX) servo_value = LEVEL_SERVO_MAX;
    // 控制舵机
    ServoCtrl(servo_value);
    // 打印调试信息
    rt_kprintf("level_servo_angle=%d, Servo Value=%u\n", cx,  servo_value);
}

void vertical_servo_angle(int cy)
{
   // 根据偏移量调整舵机角度
    uint32_t servo_value = VERTICAL_SERVO_CENTER;
    // 更新舵机控制值
    servo_value -= cy - 128;
    // 限制舵机角度范围
    if (servo_value < VERTICAL_SERVO_MIN) servo_value = VERTICAL_SERVO_MIN;
    if (servo_value > VERTICAL_SERVO_MAX) servo_value = VERTICAL_SERVO_MAX;
    // 控制舵机
    ServoCtrl1(servo_value);

    // 打印调试信息
    rt_kprintf("vertical_servo_angle=%d, Servo Value=%u\n", cy,  servo_value);
}

static void Uart1send_rx_entry(void *parameter)
{

    while (1)
    {
        int cx,cy;
        rt_mq_recv(mq_uart_rx_cx, &cx, sizeof(cx), RT_WAITING_NO);
        rt_mq_recv(mq_uart_rx_cy, &cy, sizeof(cy), RT_WAITING_NO);

        level_servo_angle(cx);
        vertical_servo_angle(cy);
//        if (rt_mq_recv(mq_uart_rx_cx, &cx, sizeof(cx), RT_WAITING_NO) == RT_EOK) //接收消息队列数据
//        {
//            // 根据目标的坐标直接调整舵机的角度
//                       adjust_servo_angle(cx);
//        }


        rt_thread_mdelay(200);
    }
}

void thread_Uart1send_rx(void)
{

    pwm_init();

    // 创建线程
    tid1 = rt_thread_create("Uart1send_rx_entry", Uart1send_rx_entry, RT_NULL, THREAD_STACK_SIZE, THREAD_PRIORITY, THREAD_TIMESLICE);
    if (tid1 != RT_NULL)
    {
        rt_thread_startup(tid1);
        rt_kprintf("thread_Uart1send_rx thread_startup\n");
    }
    else
    {
        LOG_E("thread_Uart1send_rx create failed");
    }
}

/* 导出到 msh 命令列表中 */
MSH_CMD_EXPORT(thread_Uart1send_rx, thread_Uart1send_rx);




