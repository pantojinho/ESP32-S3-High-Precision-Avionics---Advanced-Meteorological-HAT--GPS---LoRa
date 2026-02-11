#pragma once
#include <Arduino.h>

namespace BaseBoardPins {
  // Waveshare ESP32-S3 Touch AMOLED 1.64 baseline pinout
  constexpr int I2C_SDA = 47;
  constexpr int I2C_SCL = 48;
  constexpr int IMU_INT1 = 46;

  // UART exposed on side header
  constexpr int GPS_RX = 44;  // ESP32 RX <- GPS TX
  constexpr int GPS_TX = 43;  // ESP32 TX -> GPS RX

  // On-board microSD slot (kept untouched by HAT v1)
  constexpr int SD_CS   = 38;
  constexpr int SD_MOSI = 39;
  constexpr int SD_MISO = 40;
  constexpr int SD_SCK  = 41;
}

namespace HatPins {
  // Dedicated LoRa SPI on the HAT
  constexpr int LORA_SCK  = 7;
  constexpr int LORA_MOSI = 6;
  constexpr int LORA_MISO = 5;
  constexpr int LORA_NSS  = 15;
  constexpr int LORA_DIO1 = 17;
  constexpr int LORA_BUSY = 18;
  constexpr int LORA_RST  = 16;

  // External GPS connector (1x6 2.54 mm)
  // Pin order target: 3V3, GND, TXD, RXD, NC, NC
  constexpr int GPS_HEADER_TXD = BaseBoardPins::GPS_TX;
  constexpr int GPS_HEADER_RXD = BaseBoardPins::GPS_RX;
}

namespace Pins {
  // Compatibility aliases used by current firmware files.
  constexpr int I2C_SDA = BaseBoardPins::I2C_SDA;
  constexpr int I2C_SCL = BaseBoardPins::I2C_SCL;
  constexpr int IMU_INT1 = BaseBoardPins::IMU_INT1;
  constexpr int GPS_RX = BaseBoardPins::GPS_RX;
  constexpr int GPS_TX = BaseBoardPins::GPS_TX;

  constexpr int SD_CS = BaseBoardPins::SD_CS;
  constexpr int SD_MOSI = BaseBoardPins::SD_MOSI;
  constexpr int SD_MISO = BaseBoardPins::SD_MISO;
  constexpr int SD_SCK = BaseBoardPins::SD_SCK;

  constexpr int LORA_SCK = HatPins::LORA_SCK;
  constexpr int LORA_MOSI = HatPins::LORA_MOSI;
  constexpr int LORA_MISO = HatPins::LORA_MISO;
  constexpr int LORA_NSS = HatPins::LORA_NSS;
  constexpr int LORA_DIO1 = HatPins::LORA_DIO1;
  constexpr int LORA_BUSY = HatPins::LORA_BUSY;
  constexpr int LORA_RST = HatPins::LORA_RST;
}
