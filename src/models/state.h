#pragma once
#include <Arduino.h>

enum HealthFlags : uint32_t {
  HEALTH_FLIGHT_OK = (1u << 0),
  HEALTH_METEO_OK  = (1u << 1),
  HEALTH_GNSS_OK   = (1u << 2),
  HEALTH_LORA_OK   = (1u << 3),
  HEALTH_LOGGER_OK = (1u << 4),
};

struct FlightState {
  float pitch_deg = 0.0f;
  float roll_deg = 0.0f;
  float yaw_deg = 0.0f;
  float altitude_m = 0.0f;
  float vertical_speed_mps = 0.0f;
  float pressure_pa = 0.0f;
  float accel_mps2[3] = {0.0f, 0.0f, 0.0f};
  float gyro_dps[3] = {0.0f, 0.0f, 0.0f};
  bool ok = false;
};

struct MeteoState {
  float temperature_c = 0.0f;
  float humidity_rh = 0.0f;
  float pressure_pa = 0.0f;
  float gas_ohms = 0.0f;
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
  uint32_t packets_tx = 0;
  uint32_t packets_rx = 0;
  char last_payload[160] = {0};
  bool ok = false;
};

struct PowerState {
  float battery_percent = 0.0f;
  float battery_voltage = 0.0f;
  bool low_batt_flag = false;
  bool ok = false;
};

struct HealthState {
  uint32_t flags = 0;
  uint32_t fault_count_flight = 0;
  uint32_t fault_count_meteo = 0;
  uint32_t fault_count_gnss = 0;
  uint32_t fault_count_lora = 0;
  uint32_t fault_count_logger = 0;
};

struct SystemState {
  uint32_t timestamp_ms = 0;
  FlightState flight;
  MeteoState meteo;
  GnssState gnss;
  LoraState radio;
  PowerState power;
  HealthState health;
};
