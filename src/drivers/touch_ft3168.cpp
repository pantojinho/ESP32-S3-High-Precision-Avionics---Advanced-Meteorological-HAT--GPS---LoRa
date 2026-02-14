#include "touch_ft3168.h"

namespace {
constexpr uint8_t kRegDeviceMode = 0x00;
constexpr uint8_t kRegTouchPoints = 0x02;
constexpr uint8_t kRegTouchData = 0x03;
constexpr uint16_t kMaxX = 279;
constexpr uint16_t kMaxY = 455;
}  // namespace

bool TouchFt3168::begin(TwoWire& wire, uint8_t sda, uint8_t scl, uint8_t address) {
  _wire = &wire;
  _address = address;

  _wire->begin(sda, scl);
  _wire->setClock(300000);

  _ready = writeReg(kRegDeviceMode, 0x00);
  return _ready;
}

bool TouchFt3168::readPoint(uint16_t& x, uint16_t& y) {
  if (!_ready) {
    return false;
  }

  uint8_t touchCount = 0;
  if (!readRegs(kRegTouchPoints, &touchCount, 1)) {
    return false;
  }
  if ((touchCount & 0x0F) == 0) {
    return false;
  }

  uint8_t buf[4] = {0};
  if (!readRegs(kRegTouchData, buf, sizeof(buf))) {
    return false;
  }

  x = static_cast<uint16_t>(((buf[0] & 0x0F) << 8) | buf[1]);
  y = static_cast<uint16_t>(((buf[2] & 0x0F) << 8) | buf[3]);

  if (x > kMaxX) {
    x = kMaxX;
  }
  if (y > kMaxY) {
    y = kMaxY;
  }
  return true;
}

bool TouchFt3168::readRegs(uint8_t reg, uint8_t* data, size_t len) {
  if (!_wire) {
    return false;
  }
  _wire->beginTransmission(_address);
  _wire->write(reg);
  if (_wire->endTransmission(false) != 0) {
    return false;
  }
  const size_t readCount = _wire->requestFrom(static_cast<int>(_address), static_cast<int>(len));
  if (readCount != len) {
    return false;
  }
  for (size_t i = 0; i < len; ++i) {
    data[i] = static_cast<uint8_t>(_wire->read());
  }
  return true;
}

bool TouchFt3168::writeReg(uint8_t reg, uint8_t value) {
  if (!_wire) {
    return false;
  }
  _wire->beginTransmission(_address);
  _wire->write(reg);
  _wire->write(value);
  return _wire->endTransmission() == 0;
}

