#include <Arduino.h>
#include "config/pins.h"
#include "config/system_config.h"
#include "models/state.h"
#include "services/services.h"

static SystemState g_state;
static SemaphoreHandle_t g_stateMutex;

static SensorService g_sensor;
static MeteoService g_meteo;
static GnssService g_gnss;
static LoraService g_lora;
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

static void TaskSensorCore(void* param) {
  (void)param;
  const TickType_t delayTicks = pdMS_TO_TICKS(1000 / Config::SENSOR_RATE_HZ);
  g_sensor.begin();
  for (;;) {
    lockState();
    g_sensor.update(g_state);
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
    g_lora.update(g_state);
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

  Wire.begin(Pins::I2C_SDA, Pins::I2C_SCL);
  GNSS_SERIAL.begin(9600, SERIAL_8N1, Pins::GPS_RX, Pins::GPS_TX);

  g_stateMutex = xSemaphoreCreateMutex();

  xTaskCreatePinnedToCore(TaskSensorCore, "SensorCore", Config::STACK_SENSOR, nullptr, 3, nullptr, Config::CORE_SENSOR);
  xTaskCreatePinnedToCore(TaskMeteo, "Meteo", Config::STACK_METEO, nullptr, 2, nullptr, Config::CORE_SENSOR);
  xTaskCreatePinnedToCore(TaskGnss, "GNSS", Config::STACK_GNSS, nullptr, 2, nullptr, Config::CORE_SENSOR);
  xTaskCreatePinnedToCore(TaskLora, "LoRa", Config::STACK_LORA, nullptr, 1, nullptr, Config::CORE_SENSOR);
  xTaskCreatePinnedToCore(TaskUi, "UI", Config::STACK_UI, nullptr, 2, nullptr, Config::CORE_UI);
}

void loop() {
  // Empty. Work is done in tasks.
  vTaskDelay(pdMS_TO_TICKS(1000));
}
