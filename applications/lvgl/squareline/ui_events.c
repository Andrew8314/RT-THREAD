// This file was generated by SquareLine Studio
// SquareLine Studio version: SquareLine Studio 1.4.0
// LVGL version: 8.3.11
// Project name: SquareLine_Project

#include "ui.h"

static char sensor_buf[32] = {0};
static char sensor_buf1[32] = {0};/////////
static char sensor_buf2[32] = {0};/////////
static char sensor_buf3[32] = {0};/////////

extern rt_mq_t mq_hum;
extern rt_mq_t mq_tem;
extern rt_mq_t mq_brightness;
extern rt_mq_t mq_ps_data2;

void show_callback(struct lv_timer_t *param)
{
    float brightness, humidity, temperature;
    rt_uint16_t ps_data;

    rt_mq_recv(mq_ps_data2, &ps_data, sizeof(ps_data), RT_WAITING_NO);
    rt_mq_recv(mq_hum, &humidity, sizeof(humidity), RT_WAITING_NO);
    rt_mq_recv(mq_tem, &temperature, sizeof(temperature), RT_WAITING_NO);
    rt_mq_recv(mq_brightness, &brightness, sizeof(brightness), RT_WAITING_NO);

    rt_sprintf(sensor_buf3, " %.u\n", ps_data);
    lv_label_set_text(ui_Label6, sensor_buf3);
    rt_sprintf(sensor_buf, "%.2f°\n", temperature);
    lv_label_set_text(ui_Label1, sensor_buf);
    rt_sprintf(sensor_buf1, "%.2f%%\n", humidity);
    lv_label_set_text(ui_Label4, sensor_buf1);
    rt_sprintf(sensor_buf2, "%.0fLux\n", brightness);
    lv_label_set_text(ui_Label5, sensor_buf2);

}

void show_ui_event(lv_event_t * e)
{
    // Your code here
    lv_timer_t * timer = lv_timer_create(show_callback, 1000, NULL);
    lv_timer_enable(true);

}
