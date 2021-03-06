#include "display.h"

using namespace tft;
using namespace touch;

namespace display
{

namespace
{
TFT tft = TFT(TFT_CS, TFT_DC);
TouchScreen ts = TouchScreen(TS_CS);
EncoderStepCounter encoder(ENCODER_PIN1, ENCODER_PIN2, HALF_STEP);

lv_indev_t *indev_ts;
lv_indev_t *indev_en;

lv_coord_t last_x = 0;
lv_coord_t last_y = 0;

lv_group_t *grp_en;

lv_obj_t *pg;

lv_style_t rotary_style;
} // namespace

void flush(lv_disp_drv_t *disp_drv, const lv_area_t *area, lv_color_t *color_p)
{
    int16_t x1 = (int16_t)area->x1;
    int16_t y1 = (int16_t)area->y1;
    int16_t x2 = (int16_t)area->x2;
    int16_t y2 = (int16_t)area->y2;
    int16_t w = x2 - x1 + 1;
    int16_t h = y2 - y1 + 1;

    tft.startWrite();
    tft.setAddrWindow(x1, y1, w, h);
    tft.writePixels((uint16_t *)color_p, w * h, true, true);
    tft.endWrite();

    // tft.writePixelsDMA(x1, y1, w, h, (uint16_t *)color_p, w * h);

    lv_disp_flush_ready(disp_drv); // inform the graphics library flushing complete
}

bool input_ts(lv_indev_drv_t *indev_driver, lv_indev_data_t *data)
{
    TouchPoint p;

    // save the state and save the pressed coordinate
    data->state = ts.touched() ? LV_INDEV_STATE_PR : LV_INDEV_STATE_REL;
    if (data->state == LV_INDEV_STATE_PR)
    {
        p = ts.getPoint();
        last_y = map(p.x, TS_MINX, TS_MAXX, 0, tft.height());
        last_x = map(p.y, TS_MINY, TS_MAXY, 0, tft.width());
    }

    // set the coordinates (if released use the last pressed coordinates)
    data->point.x = last_x;
    data->point.y = last_y;

    return !ts.bufferEmpty();
}

void encoder_isr() {
  encoder.tick();
}
bool input_en(lv_indev_drv_t *indev_driver, lv_indev_data_t *data)
{
    data->enc_diff = encoder.getPosition();
    data->state = false ? LV_INDEV_STATE_PR : LV_INDEV_STATE_REL; // add encoder button read here if available
    
    encoder.reset();

    return false;
}

void rotary_event_cb(lv_obj_t *obj, lv_event_t event)
{
    lv_rotary_ext_t *ext = (lv_rotary_ext_t *)lv_obj_get_ext_attr(obj);
    lv_event_t e = event;
    Serial.println(ext->threshold);
    if (event == LV_EVENT_VALUE_CHANGED)
    {
        Serial.println(ext->cur_value);
        Serial.println(lv_rotary_get_angle_end(obj));
    }
    if (event == LV_EVENT_CLICKED)
    {
        Serial.println("clicked");
    }
    if (event == LV_EVENT_SHORT_CLICKED)
    {
        Serial.println("short clicked");
    }  

}

Display::Display() {}

void Display::begin()
{
    ts.begin();

    tft.begin();
    tft.setRotation(1);

    encoder.begin(); //init the encoder
    attachInterrupt(ENCODER_INT1, encoder_isr, CHANGE); // causes interrupt if pin 11 or 12 change state
    attachInterrupt(ENCODER_INT2, encoder_isr, CHANGE);

    lv_init();

    // display
    lv_disp_buf_init(&disp_buf, buf_1, buf_2, LV_HOR_RES_MAX * BUFFER_LINE_COUNT);
    lv_disp_drv_init(&disp_drv);
    disp_drv.buffer = &disp_buf;
    disp_drv.flush_cb = flush;
    disp = lv_disp_drv_register(&disp_drv);

    // input
    lv_indev_drv_init(&indev_drv_ts);
    indev_drv_ts.type = LV_INDEV_TYPE_POINTER;
    indev_drv_ts.read_cb = input_ts;
    indev_ts = lv_indev_drv_register(&indev_drv_ts);

    lv_indev_drv_init(&indev_drv_en);
    indev_drv_en.type = LV_INDEV_TYPE_ENCODER;
    indev_drv_en.read_cb = input_en;
    indev_en = lv_indev_drv_register(&indev_drv_en);

    // obj groups
    grp_en = lv_group_create();
    lv_indev_set_group(indev_en, grp_en);
}

void Display::render()
{
    // Styles
    lv_style_init(&rotary_style);
    lv_style_set_pad_inner(&rotary_style, LV_STATE_DEFAULT, 10);

    // Home
    pg = lv_page_create(NULL, NULL);

    lv_obj_t *rotary = lv_rotary_create(pg, NULL);
    lv_obj_set_size(rotary, 90, 90);
    lv_obj_align(rotary, pg, LV_ALIGN_IN_TOP_LEFT, 50, 50);
    lv_rotary_set_range(rotary, -100, 100);
    lv_rotary_set_value(rotary, 100, LV_ANIM_OFF);
    lv_obj_set_event_cb(rotary, rotary_event_cb);
    lv_group_add_obj(grp_en, rotary);
    lv_rotary_set_sensitivity(rotary, 1);
    lv_rotary_set_threshold(rotary, 10);

    lv_obj_t *rotary2 = lv_rotary_create(pg, NULL);
    lv_obj_set_size(rotary2, 90, 90);
    lv_obj_align(rotary2, rotary, LV_ALIGN_OUT_RIGHT_TOP, 10, 0);
    lv_rotary_set_range(rotary2, -100, 100);
    lv_rotary_set_value(rotary2, 100, LV_ANIM_OFF);
    lv_obj_set_event_cb(rotary2, rotary_event_cb);
    lv_rotary_set_sensitivity(rotary2, 1);
    lv_rotary_set_threshold(rotary2, 180);
    lv_group_add_obj(grp_en, rotary2);
    lv_rotary_set_type(rotary2, LV_ROTARY_TYPE_SYMMETRIC);
    // lv_rotary_set_state(rotary2, LV_ROTARY_STATE_CHECKED_DISABLED);

    lv_obj_t *rotary3 = lv_rotary_create(pg, NULL);
    lv_obj_set_size(rotary3, 120, 120);
    lv_obj_align(rotary3, rotary, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 10); 
    lv_rotary_set_range(rotary3, ROTARY_MIN, ROTARY_MAX);
    lv_rotary_set_value(rotary3, 1000, LV_ANIM_OFF);
    lv_obj_set_event_cb(rotary3, rotary_event_cb);
    lv_group_add_obj(grp_en, rotary3);
    lv_rotary_set_sensitivity(rotary3, 10);
    lv_rotary_set_threshold(rotary3, 500);
    lv_rotary_set_type(rotary3, LV_ROTARY_TYPE_REVERSE);  
    lv_obj_add_style(rotary3, LV_ROTARY_PART_KNOB, &rotary_style);

    // lv_obj_t *btn = lv_btn_create(pg, NULL);
    // lv_obj_set_size(btn, 70, 70);
    // lv_obj_align(btn, rotary3, LV_ALIGN_OUT_BOTTOM_RIGHT, 10, 10);
    // lv_group_add_obj(grp_en, btn);
    // lv_btn_set_checkable(btn, false);

    lv_scr_load(pg);
}

Display DC = Display();

} // namespace display