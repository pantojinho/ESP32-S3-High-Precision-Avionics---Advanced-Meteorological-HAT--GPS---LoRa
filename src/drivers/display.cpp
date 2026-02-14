#include "display.h"
#include <Wire.h>
#include <esp_heap_caps.h>

DisplayDriver::DisplayDriver()
    : _bus(nullptr),
      _gfx(nullptr),
      _draw_buf_a(nullptr),
      _draw_buf_b(nullptr),
      _lv_display(nullptr),
      _touch_indev(nullptr),
      _lastFrameMs(0),
      _frameIntervalMs(16),
      _fps(0.0f) {}

void DisplayDriver::begin() {
    // 1. Init Data Bus (QSPI)
    _bus = new Arduino_ESP32QSPI(
        DisplayPins::LCD_CS,
        DisplayPins::LCD_SCK,
        DisplayPins::LCD_D0,
        DisplayPins::LCD_D1,
        DisplayPins::LCD_D2,
        DisplayPins::LCD_D3
    );

    // 2. Init Display Driver (RM67162)
    // Resolution: 280x456 (or 456x280 depending on orientation)
    // Rotation: 0 - might need adjustment
    _gfx = new Arduino_RM67162(_bus, DisplayPins::LCD_RST, 0 /* rotation */);
    
    if (!_gfx->begin()) {
        Serial.println("Display Init Failed!");
        return;
    }
    
    _gfx->fillScreen(BLACK);
    
    // 3. Init LVGL
    lv_init();

    // 4. Create Draw Buffer
    // Try internal DMA memory first, then fallback to generic DMA memory.
    _draw_buf_a = static_cast<uint8_t *>(heap_caps_malloc(BUF_SIZE * 2, MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL));
    _draw_buf_b = static_cast<uint8_t *>(heap_caps_malloc(BUF_SIZE * 2, MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL));
    if (!_draw_buf_a || !_draw_buf_b) {
        if (_draw_buf_a) {
            heap_caps_free(_draw_buf_a);
            _draw_buf_a = nullptr;
        }
        if (_draw_buf_b) {
            heap_caps_free(_draw_buf_b);
            _draw_buf_b = nullptr;
        }
        _draw_buf_a = static_cast<uint8_t *>(heap_caps_malloc(BUF_SIZE * 2, MALLOC_CAP_DMA));
        _draw_buf_b = static_cast<uint8_t *>(heap_caps_malloc(BUF_SIZE * 2, MALLOC_CAP_DMA));
    }

    if (!_draw_buf_a || !_draw_buf_b) {
        Serial.println("LVGL Buffer Alloc Failed!");
        return;
    }

    // 5. Create Display
    _lv_display = lv_display_create(SCREEN_WIDTH, SCREEN_HEIGHT);
    lv_display_set_flush_cb(_lv_display, flush_cb);
    lv_display_set_buffers(_lv_display, _draw_buf_a, _draw_buf_b, BUF_SIZE * 2, LV_DISPLAY_RENDER_MODE_PARTIAL);
    lv_display_set_user_data(_lv_display, this);

    // 6. Init touch controller (FT3168 on 0x38)
    if (_touch.begin(Wire, DisplayPins::TOUCH_SDA, DisplayPins::TOUCH_SCL, 0x38)) {
        _touch_indev = lv_indev_create();
        lv_indev_set_type(_touch_indev, LV_INDEV_TYPE_POINTER);
        lv_indev_set_display(_touch_indev, _lv_display);
        lv_indev_set_read_cb(_touch_indev, touch_read_cb);
        lv_indev_set_user_data(_touch_indev, this);
        Serial.println("Touch FT3168 initialized");
    } else {
        Serial.println("Touch FT3168 not detected");
    }
    
    Serial.println("Display & LVGL Initialized");
}

void DisplayDriver::update() {
    if (!_lv_display) {
        return;
    }
    const uint32_t now = millis();
    if (_lastFrameMs != 0 && (now - _lastFrameMs) < _frameIntervalMs) {
        return;
    }
    const uint32_t dt = (_lastFrameMs == 0) ? _frameIntervalMs : (now - _lastFrameMs);
    _lastFrameMs = now;
    if (dt > 0) {
        _fps = 1000.0f / static_cast<float>(dt);
    }
    lv_timer_handler();
}

void DisplayDriver::flush_cb(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map) {
    DisplayDriver *driver = (DisplayDriver *)lv_display_get_user_data(disp);
    Arduino_GFX *gfx = driver->getGfx();

    uint32_t w = (area->x2 - area->x1 + 1);
    uint32_t h = (area->y2 - area->y1 + 1);

    gfx->draw16bitBeRGBBitmap(area->x1, area->y1, (uint16_t *)px_map, w, h);

    lv_display_flush_ready(disp);
}

void DisplayDriver::touch_read_cb(lv_indev_t *indev, lv_indev_data_t *data) {
    DisplayDriver *driver = static_cast<DisplayDriver *>(lv_indev_get_user_data(indev));
    if (!driver) {
        data->state = LV_INDEV_STATE_RELEASED;
        return;
    }

    uint16_t x = 0;
    uint16_t y = 0;
    if (driver->_touch.readPoint(x, y)) {
        data->point.x = static_cast<lv_coord_t>(x);
        data->point.y = static_cast<lv_coord_t>(y);
        data->state = LV_INDEV_STATE_PRESSED;
    } else {
        data->state = LV_INDEV_STATE_RELEASED;
    }
}
