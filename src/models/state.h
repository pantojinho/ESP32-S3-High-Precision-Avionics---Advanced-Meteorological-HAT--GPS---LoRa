#pragma once
#include <Arduino.h>

enum HealthFlags : uint32_t {
  HEALTH_FLIGHT_OK = (1u << 0),
  HEALTH_METEO_OK  = (1u << 1),
  HEALTH_GNSS_OK   = (1u << 2),
  HEALTH_LORA_OK   = (1u << 3),
  HEALTH_LOGGER_OK = (1u << 4),
};

enum class MissionMode : uint8_t {
  BOOT = 0,
  AVIONICS = 1,
  METEO = 2,
  TRACKER = 3,
  GPS = 4,
  BLACK_BOX = 5,
  CONFIG = 6,
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
  float course_deg = 0.0f;
  float hdop = 99.9f;
  uint8_t satellites = 0;
  uint8_t fix_type = 0;
  uint32_t last_fix_age_ms = 0;
  bool ok = false;
};

struct LoraState {
  int16_t rssi = 0;
  uint32_t packets_tx = 0;
  uint32_t packets_rx = 0;
  uint32_t mesh_packets_forwarded = 0;
  uint32_t mesh_packets_dropped = 0;
  uint16_t mesh_node_id = 0;
  uint16_t mesh_last_from = 0;
  uint16_t mesh_last_to = 0;
  uint8_t mesh_last_hops = 0;
  bool mesh_enabled = false;
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

struct ConnectivityState {
  bool wifi_enabled = true;
  bool wifi_connected = false;
  int16_t wifi_rssi_dbm = -120;
  bool ble_enabled = true;
  bool ble_connected = false;
  bool server_enabled = true;
  bool server_online = false;
  uint16_t server_port = 8080;
  uint8_t server_clients = 0;
};

struct UiState {
  MissionMode requested_mode = MissionMode::BOOT;
  bool auto_cycle = true;
};

struct SystemState {
  uint32_t timestamp_ms = 0;
  FlightState flight;
  MeteoState meteo;
  GnssState gnss;
  LoraState radio;
  PowerState power;
  HealthState health;
  ConnectivityState connectivity;
  UiState ui;
};
