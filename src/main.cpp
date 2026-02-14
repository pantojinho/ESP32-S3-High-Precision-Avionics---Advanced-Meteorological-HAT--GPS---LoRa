#include <Arduino.h>
#include <Wire.h>
#include "config/pins.h"
#include "config/system_config.h"
#include "models/state.h"
#include "services/services.h"

static SystemState g_state;
static SemaphoreHandle_t g_stateMutex;

static FlightCoreService g_flight;
static MeteoService g_meteo;
static GnssService g_gnss;
static LoraService g_lora;
static LoggerService g_logger;
static UiService g_ui;

static HardwareSerial GNSS_SERIAL(1);

static inline void lockState() {
  if (g_stateMutex) {
    xSemaphoreTake(g_stateMutex, portMAX_DELAY);
  }
}

static inline void unlockState() {
  if (g_stateMutex) {
    xSemaphoreGive(g_stateMutex);
  }
}

static void TaskFlightCore(void* param) {
  (void)param;
  const TickType_t delayTicks = pdMS_TO_TICKS(1000 / Config::SENSOR_RATE_HZ);
  g_flight.begin();
  for (;;) {
    lockState();
    g_state.timestamp_ms = millis();
    g_flight.update(g_state);
    unlockState();
    vTaskDelay(delayTicks);
  }
}

static void TaskMeteo(void* param) {
  (void)param;
  const TickType_t delayTicks = pdMS_TO_TICKS(1000 / Config::METEO_RATE_HZ);
  g_meteo.begin();
  for (;;) {
    lockState();
    g_state.timestamp_ms = millis();
    g_meteo.update(g_state);
    unlockState();
    vTaskDelay(delayTicks);
  }
}

static void TaskGnss(void* param) {
  (void)param;
  const TickType_t delayTicks = pdMS_TO_TICKS(1000 / Config::GNSS_RATE_HZ);
  g_gnss.begin(GNSS_SERIAL);
  for (;;) {
    lockState();
    g_state.timestamp_ms = millis();
    g_gnss.update(g_state);
    unlockState();
    vTaskDelay(delayTicks);
  }
}

static void TaskLora(void* param) {
  (void)param;
  const TickType_t delayTicks = pdMS_TO_TICKS(1000 / Config::LORA_RATE_HZ);
  g_lora.begin();
  for (;;) {
    lockState();
    g_state.timestamp_ms = millis();
    g_lora.update(g_state);
    unlockState();
    vTaskDelay(delayTicks);
  }
}

static void TaskLogger(void* param) {
  (void)param;
  const TickType_t delayTicks = pdMS_TO_TICKS(1000 / Config::LOGGER_RATE_HZ);
  g_logger.begin();
  for (;;) {
    lockState();
    g_state.timestamp_ms = millis();
    g_logger.update(g_state);
    unlockState();
    vTaskDelay(delayTicks);
  }
}

static void TaskUi(void* param) {
  (void)param;
  const TickType_t delayTicks = pdMS_TO_TICKS(1000 / Config::UI_RATE_HZ);
  g_ui.begin();
  for (;;) {
    lockState();
    SystemState snapshot = g_state;
    unlockState();
    g_ui.render(snapshot);
    vTaskDelay(delayTicks);
  }
}

void setup() {
  Serial.begin(115200);
  delay(200);

  g_state.power.battery_percent = 100.0f;
  g_state.power.battery_voltage = 4.10f;
  g_state.power.low_batt_flag = false;
  g_state.power.ok = true;
  g_state.connectivity.wifi_enabled = true;
  g_state.connectivity.ble_enabled = true;
  g_state.connectivity.server_enabled = true;
  g_state.connectivity.server_port = 8080;
  g_state.ui.requested_mode = MissionMode::GPS;
  g_state.ui.auto_cycle = true;

  Wire.begin(Pins::I2C_SDA, Pins::I2C_SCL);
  GNSS_SERIAL.begin(9600, SERIAL_8N1, Pins::GPS_RX, Pins::GPS_TX);

  g_stateMutex = xSemaphoreCreateMutex();

  xTaskCreatePinnedToCore(TaskFlightCore, "FlightCore", Config::STACK_SENSOR, nullptr, 3, nullptr, Config::CORE_SENSOR);
  xTaskCreatePinnedToCore(TaskMeteo, "Meteo", Config::STACK_METEO, nullptr, 2, nullptr, Config::CORE_SENSOR);
  xTaskCreatePinnedToCore(TaskGnss, "GNSS", Config::STACK_GNSS, nullptr, 2, nullptr, Config::CORE_SENSOR);
  xTaskCreatePinnedToCore(TaskLora, "LoRa", Config::STACK_LORA, nullptr, 1, nullptr, Config::CORE_SENSOR);
  xTaskCreatePinnedToCore(TaskLogger, "Logger", Config::STACK_LOGGER, nullptr, 1, nullptr, Config::CORE_LOGGER);
  xTaskCreatePinnedToCore(TaskUi, "UI", Config::STACK_UI, nullptr, 2, nullptr, Config::CORE_UI);
}

void loop() {
  // Empty. Work is done in tasks.
  vTaskDelay(pdMS_TO_TICKS(1000));
}
