#pragma once
#include <Arduino.h>
#include <TinyGPS++.h>
#include "../models/state.h"
#include "../drivers/display.h"
#include "../ui/mini_os.h"

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

private:
  HardwareSerial* _serial = nullptr;
  TinyGPSPlus _gps;
  uint32_t _lastFixMs = 0;
};

class LoraService {
public:
  void begin();
  void update(SystemState& state);

private:
  uint16_t _nodeId = 1;
  uint32_t _nextSeq = 0;
  uint32_t _lastTxMs = 0;
  bool _radioReady = false;
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
  MiniOsUi _miniOs;
};
