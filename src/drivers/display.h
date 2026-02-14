#pragma once

#include <Arduino.h>
#include <Arduino_GFX_Library.h>
#include <lvgl.h>
#include "../config/pins.h"

class DisplayDriver {
public:
    DisplayDriver();
    void begin();
    void update(); // Call this periodically in loop/task

    // Access to raw GFX if needed (avoid if using LVGL)
    Arduino_GFX* getGfx() { return _gfx; }

private:
    Arduino_DataBus *_bus;
    Arduino_GFX *_gfx;

    // LVGL Buffer
    static const uint32_t SCREEN_WIDTH = 280;
    static const uint32_t SCREEN_HEIGHT = 456;
    static const uint32_t BUF_SIZE = SCREEN_WIDTH * SCREEN_HEIGHT / 10; // 10% buffer
    
    uint8_t *_draw_buf;
    lv_display_t *_lv_display;

    static void flush_cb(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map);
};
