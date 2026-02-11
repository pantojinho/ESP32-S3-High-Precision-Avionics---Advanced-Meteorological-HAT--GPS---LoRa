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
  // TODO(v1): init display + LVGL pages.
}

void UiService::render(const SystemState& state) {
  // TODO(v1): render tracker / flight / meteo modes.
  (void)state;
}
