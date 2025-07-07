#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "paho_mqtt.h"
#include <wlan_mgnt.h>
#include "aht10.h"
#include <onenet.h>

int main(void)
{

    rt_wlan_connect("禹锦华", "yjh666666");
    thread_ap3216();
    thread_aht21();
    thread_onenet_upload();
    show_ui_event();
    thread_rgb1();

    rt_thread_delay(300);  // 延迟秒

    return 0;
}




