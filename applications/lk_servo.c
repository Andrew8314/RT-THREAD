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

/* Thread */
#define THREAD_PRIORITY         25
#define THREAD_STACK_SIZE       512
#define THREAD_TIMESLICE        5

#define PWM_DEV_NAME        "pwm1"  /* PWM设备名称 */
#define PWM_DEV_CHANNEL     4       /* PWM通道 PE14 上*/
#define PWM_DEV_CHANNEL1     3       /* PWM通道  PE13 下*/


#define Servo_Center_Mid 2900000      //舵机直行中值，
#define Servo_Left_Max   4700000   //(Servo_Center_Mid+Servo_Delta) 220     //舵机左转极限值//限制幅值80 0000舵机最大  //1100000云台上最大
#define Servo_Right_Min  1100000  //(Servo_Center_Mid-Servo_Delta)      //舵机右转极限值，此值跟舵机放置方式有关，立式//限制幅值520 0000舵机最大   //4700000云台上最大

static rt_thread_t tid1 = RT_NULL;
struct rt_device_pwm *pwm_dev1;      /* PWM设备句柄 */

rt_uint32_t period1 = 20000000;    /* 周期为20ms ，单位为纳秒ns    20ms*/
rt_uint32_t dir1 = 1;            /* PWM脉冲宽度值的增减方向 */
rt_uint32_t pulse1 = 3000000;          /* PWM脉冲宽度值，单位为纳秒ns */


void ServoCtrl (uint32_t duty)
{
    if (duty >= Servo_Left_Max)                  //限制幅值520 0000
        duty = Servo_Left_Max;
    else if (duty <= Servo_Right_Min)            //限制幅值80 0000
        duty = Servo_Right_Min;

    rt_pwm_set(pwm_dev, PWM_DEV_CHANNEL, period, duty);
}

static void lk_servo_entry(void *parameter)
{

    while (1)
    {
        rt_thread_mdelay(50);
        if (dir1)
        {
            pulse1 += 100000;      /* 从0值开始每次增加5000ns */
        }
        else
        {
            pulse1 -= 100000;      /* 从最大值开始每次减少5000ns */
        }
        if (pulse1 >= Servo_Left_Max)
        {
            dir1 = 0;
        }
        if (pulse1 == Servo_Right_Min)
        {
            dir1 = 1;
        }
        if (pulse1 >= Servo_Left_Max)                  //限制幅值520 0000
            pulse1 = Servo_Left_Max;
        else if (pulse1 <= Servo_Right_Min)            //限制幅值80 0000
            pulse1 = Servo_Right_Min;
        /* 设置PWM周期和脉冲宽度 */
        rt_pwm_set(pwm_dev1, PWM_DEV_CHANNEL1, period1, pulse1);
    }
}

int thread_servo(void)
{
    /* 查找设备 */
    pwm_dev1 = (struct rt_device_pwm *)rt_device_find(PWM_DEV_NAME);
    if (pwm_dev1 == RT_NULL)
    {
        rt_kprintf("pwm sample run failed! can't find %s device!\n", PWM_DEV_NAME);
        return RT_ERROR;
    }
    /* 设置PWM周期和脉冲宽度默认值 */
    rt_pwm_set(pwm_dev1, PWM_DEV_CHANNEL1, period1, pulse1);
    /* 使能设备 */
    rt_pwm_enable(pwm_dev1, PWM_DEV_CHANNEL1);

    tid1 = rt_thread_create("tid_servo", lk_servo_entry, RT_NULL, THREAD_STACK_SIZE, THREAD_PRIORITY, THREAD_TIMESLICE);
    if (tid1 != RT_NULL)
        rt_thread_startup(tid1);
    return 0;
}
MSH_CMD_EXPORT(thread_servo, thread_servo);

