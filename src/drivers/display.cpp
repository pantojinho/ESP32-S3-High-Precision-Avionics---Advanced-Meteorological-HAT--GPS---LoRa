#include "display.h"

DisplayDriver::DisplayDriver() : _bus(nullptr), _gfx(nullptr), _draw_buf(nullptr), _lv_display(nullptr) {}

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
    // LVGL 9 handles buffers differently than v8.
    // We allocate a simple buffer.
    _draw_buf = (uint8_t *)heap_caps_malloc(BUF_SIZE * 2, MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL); // *2 for 16-bit color
    if (!_draw_buf) {
        Serial.println("LVGL Buffer Alloc Failed!");
        return;
    }

    // 5. Create Display
    _lv_display = lv_display_create(SCREEN_WIDTH, SCREEN_HEIGHT);
    lv_display_set_flush_cb(_lv_display, flush_cb);
    lv_display_set_buffers(_lv_display, _draw_buf, nullptr, BUF_SIZE * 2, LV_DISPLAY_RENDER_MODE_PARTIAL);
    lv_display_set_user_data(_lv_display, this);
    
    Serial.println("Display & LVGL Initialized");
}

void DisplayDriver::update() {
    lv_task_handler();
}

void DisplayDriver::flush_cb(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map) {
    DisplayDriver *driver = (DisplayDriver *)lv_display_get_user_data(disp);
    Arduino_GFX *gfx = driver->getGfx();

    uint32_t w = (area->x2 - area->x1 + 1);
    uint32_t h = (area->y2 - area->y1 + 1);

    gfx->draw16bitBeRGBBitmap(area->x1, area->y1, (uint16_t *)px_map, w, h);

    lv_display_flush_ready(disp);
}
