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
    thread_ap3216();
    rt_wlan_connect("禹锦华", "yjh666666");
    onenet_mqtt_init();
    onenet_upload_cycle();
    onenet_set_cmd_rsp();
    show_ui_event();

    rt_thread_delay(300);  // 延迟秒

    return 0;
}




