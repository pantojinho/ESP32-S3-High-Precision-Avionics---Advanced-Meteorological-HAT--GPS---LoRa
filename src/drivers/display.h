#pragma once

#include <Arduino.h>
#include <Arduino_GFX_Library.h>
#include <lvgl.h>
#include "touch_ft3168.h"
#include "../config/pins.h"

class DisplayDriver {
public:
    DisplayDriver();
    void begin();
    void update(); // Call this periodically in loop/task
    bool isReady() const { return _lv_display != nullptr; }
    bool isTouchReady() const { return _touch.isReady(); }
    void setFrameIntervalMs(uint16_t interval_ms) { _frameIntervalMs = interval_ms; }
    float getUiFps() const { return _fps; }

    // Access to raw GFX if needed (avoid if using LVGL)
    Arduino_GFX* getGfx() { return _gfx; }

private:
    Arduino_DataBus *_bus;
    Arduino_GFX *_gfx;

    // LVGL Buffer
    static const uint32_t SCREEN_WIDTH = 280;
    static const uint32_t SCREEN_HEIGHT = 456;
    static const uint32_t BUF_SIZE = SCREEN_WIDTH * SCREEN_HEIGHT / 10; // 10% buffer
    
    uint8_t *_draw_buf_a;
    uint8_t *_draw_buf_b;
    lv_display_t *_lv_display;
    lv_indev_t *_touch_indev;
    TouchFt3168 _touch;
    uint32_t _lastFrameMs;
    uint16_t _frameIntervalMs;
    float _fps;

    static void flush_cb(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map);
    static void touch_read_cb(lv_indev_t *indev, lv_indev_data_t *data);
};
