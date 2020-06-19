/* 
 * File:   display.h
 * Author: Adam Martini
 *
 * Created on May 10, 2020, 11:32 PM
 */

#ifndef DISPLAY_H
#define DISPLAY_H

#include "Arduino.h"
#include <lvgl.h>
#include <EncoderStepCounter.h>

#include "tft.h"
#include "touch.h"

namespace display
{
#define BUFFER_LINE_COUNT 10

#define ENCODER_PIN1 11
#define ENCODER_INT1 digitalPinToInterrupt(ENCODER_PIN1)
#define ENCODER_PIN2 12
#define ENCODER_INT2 digitalPinToInterrupt(ENCODER_PIN2)

#define ROTARY_WIDTH lv_obj_get_width(pg) / 3 - 10
#define ROTARY_HEIGHT 24

#define ROTARY_MIN -2048.0f
#define ROTARY_MAX 2047.0f


class Display
{
    lv_disp_buf_t disp_buf;
    lv_color_t buf_1[LV_HOR_RES_MAX * BUFFER_LINE_COUNT];
    lv_color_t buf_2[LV_HOR_RES_MAX * BUFFER_LINE_COUNT];

    lv_disp_drv_t disp_drv;
    lv_disp_t *disp;

    lv_indev_drv_t indev_drv_ts;
    lv_indev_drv_t indev_drv_en;

public:
    Display();
    void begin();
    void render();
};

extern Display DC;

} // namespace display

#endif /* DISPLAY_H */