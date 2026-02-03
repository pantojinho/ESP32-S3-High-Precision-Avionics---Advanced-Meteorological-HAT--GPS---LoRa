#include "services.h"

void SensorService::begin() {
  // TODO: init QMI8658 + BMM350 + BMP581
}

void SensorService::update(SystemState& state) {
  // TODO: read sensors and run filter (Madgwick/Mahony)
  state.imu.ok = false;
  state.vario.ok = false;
}

void MeteoService::begin() {
  // TODO: init BME688 + LTR-390
}

void MeteoService::update(SystemState& state) {
  // TODO: update meteo state
  state.meteo.ok = false;
}

void GnssService::begin(HardwareSerial& serial) {
  // TODO: init serial + GNSS parser
  (void)serial;
}

void GnssService::update(SystemState& state) {
  // TODO: parse NMEA and update state
  state.gnss.ok = false;
}

void LoraService::begin() {
  // TODO: init LoRa radio
}

void LoraService::update(SystemState& state) {
  // TODO: send packet
  (void)state;
}

void UiService::begin() {
  // TODO: init display + LVGL
}

void UiService::render(const SystemState& state) {
  // TODO: render UI from state
  (void)state;
}
