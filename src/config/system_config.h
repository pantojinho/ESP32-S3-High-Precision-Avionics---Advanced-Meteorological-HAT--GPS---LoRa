#pragma once
#include <Arduino.h>

namespace Config {
  // Task rates (Hz)
  constexpr uint32_t SENSOR_RATE_HZ = 100;
  constexpr uint32_t METEO_RATE_HZ  = 2;
  constexpr uint32_t GNSS_RATE_HZ   = 5;
  constexpr uint32_t LORA_RATE_HZ   = 2;
  constexpr uint32_t UI_RATE_HZ     = 30;

  // Stack sizes (words, not bytes)
  constexpr uint32_t STACK_SENSOR = 4096;
  constexpr uint32_t STACK_METEO  = 4096;
  constexpr uint32_t STACK_GNSS   = 4096;
  constexpr uint32_t STACK_LORA   = 4096;
  constexpr uint32_t STACK_UI     = 6144;

  // Core affinity (ESP32-S3 has 2 cores)
  constexpr int CORE_SENSOR = 0;
  constexpr int CORE_UI     = 1;
}
