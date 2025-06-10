/*
 * Copyright (c) 2006-2022, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-10-18     Meco Man     The first version
 */
#include <lvgl.h>
#include <stdbool.h>
#include <rtdevice.h>
#include <board.h>

#define PIN_KEY_LEFT        GET_PIN(C, 0)   // PC0:  KEY0         --> KEY
#define PIN_KEY_DOWN        GET_PIN(C, 1)   // PC1 : KEY1         --> KEY
#define PIN_KEY_RIGHT       GET_PIN(C, 4)   // PC4 : KEY2         --> KEY
#define PIN_KEY_UP          GET_PIN(C, 5)   // PC5:  WK_UP        --> KEY

static uint8_t button_read(void)
{
    if (rt_pin_read(PIN_KEY_LEFT) == PIN_LOW)
    {
        return 1;
    }
    if (rt_pin_read(PIN_KEY_RIGHT) == PIN_LOW)
    {
        return 2;
    }
    if (rt_pin_read(PIN_KEY_UP) == PIN_LOW)
    {
        return 3;
    }
    if (rt_pin_read(PIN_KEY_DOWN) == PIN_LOW)
    {
        return 4;
    }
    return 0;
}

static void keypad_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data)
{
    static uint32_t last_key = 0;
    uint32_t act_key = button_read();
    if (act_key != 0)
    {
        data->state = LV_INDEV_STATE_PR;
        switch (act_key)
        {
            case 1:
                act_key = LV_KEY_PREV;
                break;
            case 2:
                act_key = LV_KEY_NEXT;
                break;
            case 3:
                act_key = LV_KEY_ESC;
                break;
            case 4:
                act_key = LV_KEY_ENTER;
            default:
                break;
        }
        last_key = act_key;
    }
    else
    {
        data->state = LV_INDEV_STATE_REL;
    }
    data->key = last_key;
}

static void Btns_Init(void)
{
    rt_pin_mode(PIN_KEY_LEFT, PIN_MODE_INPUT_PULLUP);
    rt_pin_mode(PIN_KEY_RIGHT, PIN_MODE_INPUT_PULLUP);
    rt_pin_mode(PIN_KEY_DOWN, PIN_MODE_INPUT_PULLUP);
    rt_pin_mode(PIN_KEY_UP, PIN_MODE_INPUT_PULLUP);
}

void lv_port_indev_init(void)
{
    static lv_indev_drv_t keypad_drv;
    lv_indev_t *indev_keypad;

    Btns_Init();
    lv_indev_drv_init(&keypad_drv);
    keypad_drv.type = LV_INDEV_TYPE_KEYPAD;
    keypad_drv.read_cb = keypad_read;
    indev_keypad = lv_indev_drv_register(&keypad_drv);

    lv_group_t * group = lv_group_create();
    lv_group_set_default(group);
    lv_indev_set_group(indev_keypad, group);
}
