/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author        Notes
 * 2021-10-17     Meco Man      first version
 */
#include <lvgl.h>

void lv_user_gui_init(void)
{
    #ifdef PKG_USING_LVGL_SQUARELINE
    extern void ui_init(void);
    ui_init();
    #else
    extern lv_demo_widgets();
    lv_demo_widgets();
    #endif
}
