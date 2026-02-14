#pragma once

#include <Arduino.h>
#include <Wire.h>

class TouchFt3168 {
public:
  TouchFt3168() = default;

  bool begin(TwoWire& wire, uint8_t sda, uint8_t scl, uint8_t address = 0x38);
  bool readPoint(uint16_t& x, uint16_t& y);
  bool isReady() const { return _ready; }

private:
  bool readRegs(uint8_t reg, uint8_t* data, size_t len);
  bool writeReg(uint8_t reg, uint8_t value);

  TwoWire* _wire = nullptr;
  uint8_t _address = 0x38;
  bool _ready = false;
};

