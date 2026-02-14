#pragma once
#include <Arduino.h>
#include "../models/state.h"
#include "../drivers/display.h"

class FlightCoreService {
public:
  void begin();
  void update(SystemState& state);
};

class MeteoService {
public:
  void begin();
  void update(SystemState& state);
};

class GnssService {
public:
  void begin(HardwareSerial& serial);
  void update(SystemState& state);
};

class LoraService {
public:
  void begin();
  void update(SystemState& state);
};

class LoggerService {
public:
  void begin();
  void update(SystemState& state);
};

class UiService {
public:
  void begin();
  void render(const SystemState& state);

private:
  DisplayDriver _display;
};
