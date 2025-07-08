SET(SOURCES components/ui_comp_image1.c
    components/ui_comp.c
    screens/ui_Screen1.c
    ui.c
    components/ui_comp_hook.c
    ui_helpers.c
    images/ui_img_1843987647.c
    images/ui_img_1481188968.c
    images/ui_img_1823885662.c
    images/ui_img_brightness_png.c
    images/ui_img_ps_data_png.c)

add_library(ui ${SOURCES})
