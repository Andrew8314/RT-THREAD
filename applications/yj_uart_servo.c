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

extern rt_mq_t mq_uart_rx;
static rt_thread_t tid1 = RT_NULL;


// 摄像头图像宽度
#define IMAGE_WIDTH 1000
// 舵机控制范围
#define SERVO_MIN 0
#define SERVO_MAX 180
#define SERVO_CENTER 90

// 每次调整的最大步长
//#define MAX_ADJUSTMENT_STEP 3500000  // 减小调整步长
// 根据目标的坐标直接调整舵机的角度
void adjust_servo_angle(int cx)
{
//    // 计算目标相对于图像中心的偏移量
    int offset = (IMAGE_WIDTH / 2) - cx;
//    // 根据偏移量调整舵机角度
    uint32_t servo_value = SERVO_CENTER;
//    // 计算调整量
//    int adjustment = 0;
//    if (offset < 0) {
//        // 目标在右侧，舵机向右调整
//        adjustment = (offset * (SERVO_CENTER - SERVO_MIN)) / (IMAGE_WIDTH / 2);
//    } else if (offset > 0) {
//        // 目标在左侧，舵机向左调整
//        adjustment = (offset * (SERVO_MAX - SERVO_CENTER)) / (IMAGE_WIDTH / 2);
//    }

    // 限制调整量的最大值
  //  if (adjustment > MAX_ADJUSTMENT_STEP) adjustment = MAX_ADJUSTMENT_STEP;
  //  if (adjustment < -MAX_ADJUSTMENT_STEP) adjustment = -MAX_ADJUSTMENT_STEP;

    // 更新舵机控制值
    servo_value -= cx - 128;

    // 限制舵机角度范围
    if (servo_value < SERVO_MIN) servo_value = SERVO_MIN;
    if (servo_value > SERVO_MAX) servo_value = SERVO_MAX;
    // 控制舵机

    ServoCtrl(servo_value);

    // 打印调试信息
    rt_kprintf("Parsed: distance=%d, Offset=%d, Servo Value=%u\n", cx, offset, servo_value);
}

static void Uart1send_rx_entry(void *parameter)
{

    while (1)
    {
        int cx;
        if (rt_mq_recv(mq_uart_rx, &cx, sizeof(cx), RT_WAITING_NO) == RT_EOK)
        {
            // 根据目标的坐标直接调整舵机的角度
                       adjust_servo_angle(cx);
        }

       // rt_kprintf("cccccccccccParsed:distance=%d \n",cx);

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




