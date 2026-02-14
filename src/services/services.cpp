#include "services.h"
#include <stdio.h>
#include <string.h>

void FlightCoreService::begin() {
  // TODO(v1): init QMI8658 + BMP581
}

void FlightCoreService::update(SystemState& state) {
  // TODO(v1): replace placeholders with real sensor reads and fusion filter.
  state.flight.pitch_deg = 0.0f;
  state.flight.roll_deg = 0.0f;
  state.flight.yaw_deg = 0.0f;
  state.flight.altitude_m = 0.0f;
  state.flight.vertical_speed_mps = 0.0f;
  state.flight.pressure_pa = 101325.0f;
  state.flight.ok = true;
  state.health.flags |= HEALTH_FLIGHT_OK;
}

void MeteoService::begin() {
  // TODO(v1): init BME688
}

void MeteoService::update(SystemState& state) {
  // TODO(v1): read BME688 values.
  state.meteo.temperature_c = 25.0f;
  state.meteo.humidity_rh = 50.0f;
  state.meteo.pressure_pa = 101325.0f;
  state.meteo.gas_ohms = 10000.0f;
  state.meteo.ok = true;
  state.health.flags |= HEALTH_METEO_OK;
}

void GnssService::begin(HardwareSerial& serial) {
  // TODO(v1): init TinyGPS++ parser.
  (void)serial;
}

void GnssService::update(SystemState& state) {
  // TODO(v1): parse NMEA and update fix type.
  state.gnss.ok = false;
  state.gnss.fix_type = 0;
  state.health.flags &= ~HEALTH_GNSS_OK;
}

void LoraService::begin() {
  // TODO(v1): init SX1262/CC68 at 915 MHz.
}

void LoraService::update(SystemState& state) {
  const int written = snprintf(
    state.radio.last_payload,
    sizeof(state.radio.last_payload),
    "t=%lu,lat=%.6f,lon=%.6f,alt=%.1f,var=%.2f,tmp=%.1f,hum=%.1f,prs=%.1f,bat=%.1f,h=%lu",
    static_cast<unsigned long>(state.timestamp_ms),
    state.gnss.lat,
    state.gnss.lon,
    state.flight.altitude_m,
    state.flight.vertical_speed_mps,
    state.meteo.temperature_c,
    state.meteo.humidity_rh,
    state.meteo.pressure_pa,
    state.power.battery_percent,
    static_cast<unsigned long>(state.health.flags));
  if (written > 0) {
    state.radio.packets_tx++;
    state.radio.ok = true;
    state.radio.rssi = -70;
    state.health.flags |= HEALTH_LORA_OK;
  } else {
    state.radio.ok = false;
    state.health.fault_count_lora++;
  }
}

void LoggerService::begin() {
  // TODO(v1.1): open SD file and write CSV header.
}

void LoggerService::update(SystemState& state) {
  // TODO(v1.1): append a compact record to SD with buffered flush.
  (void)state;
  state.health.flags |= HEALTH_LOGGER_OK;
}

void UiService::begin() {
  _display.begin();
  
  // TODO(v1): Initialize UI pages/screens here
  // For now, let's create a simple label to verify it works
  lv_obj_t *label = lv_label_create(lv_screen_active());
  lv_label_set_text(label, "Antigravity Avionics\nInitializing...");
  lv_obj_center(label);
}

void UiService::render(const SystemState& state) {
  // Update UI Elements with state data if needed
  // ...
  
  // Handle LVGL tasks
  // Note: We need to handle ticks. For simplicity in this task, 
  // we assume the loop time is roughly constant or we use system time.
  // Ideally, use a dedicated timer or esp_timer for lv_tick_inc.
  static uint32_t last_ms = 0;
  uint32_t now = millis();
  if (last_ms == 0) last_ms = now;
  
  uint32_t dt = now - last_ms;
  if (dt > 0) {
      lv_tick_inc(dt);
      last_ms = now;
  }
  
  _display.update(); // Calls lv_timer_handler
}
