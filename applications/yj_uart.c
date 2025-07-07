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

#define SAMPLE_UART_NAME       "uart3"  //PB10 TX    PB11 RX

#define RXSIZE  256
char RxBuffer[RXSIZE],Uart1send_rx_buf[RXSIZE];
char readData;
uint8_t ReadCount = 0;
int flag=0;
int finish_flag=0;
int cx,cy;

/* 用于接收消息的信号量 */
static struct rt_semaphore rx_sem;
static rt_device_t serial;

/* 接收数据回调函数 */
static rt_err_t uart_input(rt_device_t dev, rt_size_t size)
{
    /* 串口接收到数据后产生中断，调用此回调函数，然后发送接收信号量 */
    rt_sem_release(&rx_sem);
    return RT_EOK;
}

void kz_entry(void *parameter)
{

    while (1)
    {
        //sscanf(Uart1send_rx_buf, "%d %d %f", &cx, &cy,&distance); //获取OpenMV发过来的X,Y位置信息
//
//        if (sscanf(Uart1send_rx_buf, "%d %d %d", &cx, &cy) == 3)
//        {
//          //  rt_kprintf("Parsed: cx=%d, cy=%d, distance=%.2f \n", cx, cy, distance);
//         //   rt_kprintf("Parsed:distance=%d \n",cx);
//            rt_thread_mdelay(50);
//
//        }
        sscanf(Uart1send_rx_buf, "%d %d %d", &cx, &cy);
        rt_thread_mdelay(50);


    }
}

static void serial_thread_entry(void *parameter)
{
    char ch;
    char str1[] = "hello RT-Thread!hh嘿嘿123\r\n";


    while (1)
    {
        /* 从串口读取一个字节的数据，没有读取到则等待接收信号量 */
        while (rt_device_read(serial, -1, (uint8_t *)&readData, 1) != 1)
        {
            /* 阻塞等待接收信号量，等到信号量后再次读取数据 */
            rt_sem_take(&rx_sem, RT_WAITING_FOREVER);
        }

        /* 读取到的数据 */
        RxBuffer[ReadCount] = readData;
        ReadCount++;
        if((RxBuffer[ReadCount-1] == 0xb3)&&(RxBuffer[ReadCount-2] == 0xb3)) flag=1;         //帧头判定
        else if((RxBuffer[ReadCount-2] == 0x0d)&&(RxBuffer[ReadCount-1] == 0x0a)) flag=2;  //帧尾判定
        else flag=0;

        switch (flag)
        {
        case 1:
            ReadCount = 0;
            memset(RxBuffer,0x00,sizeof(RxBuffer));
            //   rt_kprintf("Received char: %c  \n",RxBuffer);

            break;
        case 2:
            RxBuffer[ReadCount-1] = '\0';
            RxBuffer[ReadCount-2] = '\0';
            strcpy(Uart1send_rx_buf,RxBuffer);
            rt_kprintf("aa%s\r\n",Uart1send_rx_buf);
            ReadCount = 0;
            finish_flag=1;
            memset(RxBuffer,0x00,sizeof(RxBuffer));

            break;
        default:break;
        }


        // sscanf(Uart1send_rx_buf, "%d %d %f", &cx, &cy,&distance);


        // rt_device_write(serial, 0,(uint8_t *)&ch, 1);
    }
}

static int uart_sample(int argc, char *argv[])
{
    rt_err_t ret = RT_EOK;
    char uart_name[RT_NAME_MAX];
    char str[] = "hello RT-Thread!\r\n";

    if (argc == 2)
    {
        rt_strncpy(uart_name, argv[1], RT_NAME_MAX);
    }
    else
    {
        rt_strncpy(uart_name, SAMPLE_UART_NAME, RT_NAME_MAX);
    }

    /* 查找系统中的串口设备 */
    serial = rt_device_find(uart_name);
    if (!serial)
    {
        rt_kprintf("find %s failed!\n", uart_name);
        return RT_ERROR;
    }

    /* 初始化信号量 */
    rt_sem_init(&rx_sem, "rx_sem", 0, RT_IPC_FLAG_FIFO);
    /* 以中断接收及轮询发送模式打开串口设备 */
    rt_device_open(serial, RT_DEVICE_FLAG_INT_RX);
    /* 设置接收回调函数 */
    rt_device_set_rx_indicate(serial, uart_input);
    /* 发送字符串 */
    rt_device_write(serial, 0, str, (sizeof(str) - 1));


    /* 创建 serial 线程 */
    rt_thread_t thread = rt_thread_create("serial", serial_thread_entry, RT_NULL, 1024, 25, 10);
    rt_thread_t thread1111 = rt_thread_create("seria1111", kz_entry, RT_NULL, 4096, 25, 5);

    /* 创建成功则启动线程 */
    if (thread != RT_NULL)
    {
        rt_thread_startup(thread);
    }
    else
    {
        ret = RT_ERROR;
    }
    /* 创建成功则启动线程 */
    if (thread1111 != RT_NULL)
    {
        rt_thread_startup(thread1111);
    }
    else
    {
        ret = RT_ERROR;
    }
    return ret;
}
/* 导出到 msh 命令列表中 */
MSH_CMD_EXPORT(uart_sample, uart device sample);

