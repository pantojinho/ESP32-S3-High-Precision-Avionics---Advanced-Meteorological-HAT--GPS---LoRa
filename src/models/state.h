#pragma once
#include <Arduino.h>

struct ImuState {
  float pitch_deg = 0.0f;
  float roll_deg = 0.0f;
  float yaw_deg = 0.0f;
  float accel_mps2[3] = {0.0f, 0.0f, 0.0f};
  float gyro_dps[3] = {0.0f, 0.0f, 0.0f};
  float mag_ut[3] = {0.0f, 0.0f, 0.0f};
  bool ok = false;
};

struct VarioState {
  float altitude_m = 0.0f;
  float vertical_speed_mps = 0.0f;
  float pressure_pa = 0.0f;
  bool ok = false;
};

struct MeteoState {
  float temperature_c = 0.0f;
  float humidity_rh = 0.0f;
  float pressure_pa = 0.0f;
  float gas_ohms = 0.0f;
  float uv_index = 0.0f;
  float lux = 0.0f;
  bool ok = false;
};

struct GnssState {
  double lat = 0.0;
  double lon = 0.0;
  float altitude_m = 0.0f;
  float ground_speed_mps = 0.0f;
  uint8_t fix_type = 0;
  bool ok = false;
};

struct LoraState {
  int16_t rssi = 0;
  uint32_t packets_sent = 0;
  bool ok = false;
};

struct SystemState {
  ImuState imu;
  VarioState vario;
  MeteoState meteo;
  GnssState gnss;
  LoraState lora;
};
