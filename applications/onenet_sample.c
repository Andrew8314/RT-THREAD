#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <onenet.h>
#include "aht10.h"
#include <stdio.h>


#define DBG_ENABLE
#define DBG_COLOR
#define DBG_SECTION_NAME    "onenet.sample"
#if ONENET_DEBUG
#define DBG_LEVEL           DBG_LOG
#else
#define DBG_LEVEL           DBG_INFO
#endif /* ONENET_DEBUG */

#include <rtdbg.h>

#define PIN_LED_B              GET_PIN(F, 11)      // PF11 :  LED_B        --> LED
#define PIN_LED_R              GET_PIN(F, 12)      // PF12 :  LED_R        --> LED

#ifdef FINSH_USING_MSH
#include <finsh.h>


const char *i2c_bus_name = "i2c3";
static float humidity = 0, temperature = 0;/////////////////////
static aht10_device_t dev;

extern rt_mq_t mq_brightness;
extern rt_mq_t mq_ps_data;
/* upload random value to temperature*/
static void onenet_upload_entry(void *parameter)
{

    while(rt_wlan_is_connected() == RT_FALSE)
    {
        rt_thread_mdelay(1000);
    }

    dev = aht10_init(i2c_bus_name);


    while (1)
    {
        rt_uint16_t ps_data;
        float brightness;

        humidity = aht10_read_humidity(dev); /* 读取湿度 */
        temperature = aht10_read_temperature(dev);   /* 读取温度 */

        rt_mq_recv(mq_brightness, &brightness, sizeof(brightness), RT_WAITING_NO);
        rt_mq_recv(mq_ps_data, &ps_data, sizeof(ps_data), RT_WAITING_NO);
        //LOG_D("current ps data: %d.", ps_data);
        //LOG_D("current brightness: %d.%d(lux).", (int)brightness, ((int)(10 * brightness) % 10));


        onenet_mqtt_upload_digit("brightness", (int)brightness);
        rt_thread_mdelay(10);
        onenet_mqtt_upload_digit("ps_data", (int)ps_data);
        rt_thread_mdelay(10);
        onenet_mqtt_upload_digit("temp_value", (int)temperature);
        rt_thread_mdelay(10);
        onenet_mqtt_upload_digit("humidity_value", (int)humidity);


        rt_thread_delay(rt_tick_from_millisecond(5 * 1000));
    }
}


int onenet_upload_cycle(void)
{
    rt_thread_t tid;

    tid = rt_thread_create("onenet_send",
            onenet_upload_entry,
            RT_NULL,
            2 * 1024,
            RT_THREAD_PRIORITY_MAX / 3 - 1,
            5);
    if (tid)
    {
        rt_thread_startup(tid);
    }

    return 0;
}
MSH_CMD_EXPORT(onenet_upload_cycle, send data to OneNET cloud cycle);


int onenet_publish_digit(int argc, char **argv)
{
    if (argc != 3)
    {
        LOG_E("onenet_publish [datastream_id]  [value]  - mqtt pulish digit data to OneNET.");
        return -1;
    }

    if (onenet_mqtt_upload_digit(argv[1], atoi(argv[2])) < 0)
    {
        LOG_E("upload digit data has an error!\n");
    }

    return 0;
}
MSH_CMD_EXPORT_ALIAS(onenet_publish_digit, onenet_mqtt_publish_digit, send digit data to onenet cloud);

int onenet_publish_string(int argc, char **argv)
{
    if (argc != 3)
    {
        LOG_E("onenet_publish [datastream_id]  [string]  - mqtt pulish string data to OneNET.");
        return -1;
    }

    if (onenet_mqtt_upload_string(argv[1], argv[2]) < 0)
    {
        LOG_E("upload string has an error!\n");
    }

    return 0;
}
MSH_CMD_EXPORT_ALIAS(onenet_publish_string, onenet_mqtt_publish_string, send string data to onenet cloud);

/* onenet mqtt command response callback function */
static void onenet_cmd_rsp_cb(uint8_t *recv_data, size_t recv_size, uint8_t **resp_data, size_t *resp_size)
{
    char res_buf[] = { "cmd is received!\n" };

    LOG_D("recv data is %x:%x\n", recv_data[0], recv_data[1]);

    if(recv_data[0] == 0x00)
    {
        rt_pin_write(PIN_LED_B, recv_data[1] > 0 ? PIN_HIGH : PIN_LOW);
        LOG_D("blue light %d", recv_data[1]);
    }else if(recv_data[0] == 0x01)
    {
        rt_pin_write(PIN_LED_R, recv_data[1] > 0 ? PIN_HIGH : PIN_LOW);
        LOG_D("red light %d", recv_data[1]);
    }

    /* user have to malloc memory for response data */
    *resp_data = (uint8_t *) ONENET_MALLOC(strlen(res_buf));

    strncpy((char *)*resp_data, res_buf, strlen(res_buf));

    *resp_size = strlen(res_buf);
}

/* set the onenet mqtt command response callback function */
int onenet_set_cmd_rsp(int argc, char **argv)
{
    rt_pin_mode(PIN_LED_B,PIN_MODE_OUTPUT);
    rt_pin_mode(PIN_LED_R,PIN_MODE_OUTPUT);
    onenet_set_cmd_rsp_cb(onenet_cmd_rsp_cb);
    return 0;
}
MSH_CMD_EXPORT(onenet_set_cmd_rsp, set cmd response function);

#endif /* FINSH_USING_MSH */
