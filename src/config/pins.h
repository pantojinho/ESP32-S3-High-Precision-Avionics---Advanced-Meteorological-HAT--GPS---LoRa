#pragma once
#include <Arduino.h>

namespace Pins {
  // I2C shared bus for HAT sensors
  constexpr int I2C_SDA = 47;
  constexpr int I2C_SCL = 48;

  // On-board IMU interrupt
  constexpr int IMU_INT1 = 46;

  // GNSS UART (GPS TX -> ESP RX, GPS RX -> ESP TX)
  constexpr int GPS_RX = 44;
  constexpr int GPS_TX = 43;

  // LoRa SPI (verify with schematic)
  constexpr int LORA_SCK  = 7;
  constexpr int LORA_MOSI = 6;
  constexpr int LORA_MISO = 5;
  constexpr int LORA_NSS  = 15;
  constexpr int LORA_DIO1 = 17;
  constexpr int LORA_BUSY = 18;
  constexpr int LORA_RST  = 16; // TODO: confirm exact pin
}
