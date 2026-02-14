#include "services.h"
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include "../config/pins.h"

#if __has_include(<RadioLib.h>)
#define HAS_RADIOLIB 1
#include <RadioLib.h>
#include <SPI.h>
#else
#define HAS_RADIOLIB 0
#endif

namespace {

constexpr uint32_t kGnssFixStaleMs = 5000;
constexpr uint32_t kLoraTxIntervalMs = 2000;
constexpr uint16_t kMeshBroadcast = 0xFFFF;
constexpr uint8_t kMeshDefaultTtl = 3;

struct MeshFrame {
  uint16_t src = 0;
  uint16_t dst = kMeshBroadcast;
  uint8_t ttl = kMeshDefaultTtl;
  uint32_t seq = 0;
  char payload[96] = {0};
};

bool buildMeshFrame(const MeshFrame& frame, char* out, size_t outLen) {
  if (!out || outLen == 0) {
    return false;
  }
  const int written = snprintf(
    out,
    outLen,
    "M1|%u|%u|%u|%lu|%s",
    static_cast<unsigned>(frame.src),
    static_cast<unsigned>(frame.dst),
    static_cast<unsigned>(frame.ttl),
    static_cast<unsigned long>(frame.seq),
    frame.payload);
  return written > 0 && static_cast<size_t>(written) < outLen;
}

bool parseMeshFrame(const char* text, MeshFrame& frame) {
  if (!text || !text[0]) {
    return false;
  }

  char copy[160] = {0};
  strncpy(copy, text, sizeof(copy) - 1);
  copy[sizeof(copy) - 1] = '\0';

  char* fields[6] = {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr};
  uint8_t fieldCount = 0;
  char* cursor = copy;
  while (cursor && fieldCount < 6) {
    fields[fieldCount++] = cursor;
    char* sep = strchr(cursor, '|');
    if (!sep) {
      break;
    }
    *sep = '\0';
    cursor = sep + 1;
  }

  if (fieldCount < 5 || !fields[0] || strcmp(fields[0], "M1") != 0) {
    return false;
  }

  frame.src = static_cast<uint16_t>(strtoul(fields[1], nullptr, 10));
  frame.dst = static_cast<uint16_t>(strtoul(fields[2], nullptr, 10));
  frame.ttl = static_cast<uint8_t>(strtoul(fields[3], nullptr, 10));
  frame.seq = static_cast<uint32_t>(strtoul(fields[4], nullptr, 10));
  if (fieldCount >= 6 && fields[5]) {
    strncpy(frame.payload, fields[5], sizeof(frame.payload) - 1);
    frame.payload[sizeof(frame.payload) - 1] = '\0';
  } else {
    frame.payload[0] = '\0';
  }
  return true;
}

#if HAS_RADIOLIB
#if defined(FSPI)
SPIClass g_loraSpi(FSPI);
#elif defined(HSPI)
SPIClass g_loraSpi(HSPI);
#else
SPIClass g_loraSpi;
#endif
SPISettings g_loraSpiSettings(8000000, MSBFIRST, SPI_MODE0);
SX1262 g_loraRadio = new Module(
  Pins::LORA_NSS,
  Pins::LORA_DIO1,
  Pins::LORA_RST,
  Pins::LORA_BUSY,
  g_loraSpi,
  g_loraSpiSettings);
#endif

}  // namespace

void FlightCoreService::begin() {
  // TODO(v1): init QMI8658 + BMP581
}

void FlightCoreService::update(SystemState& state) {
  // TODO(v1): replace placeholders with real sensor reads and fusion filter.
  const float t = state.timestamp_ms * 0.001f;
  state.flight.pitch_deg = 3.0f * sinf(t * 0.6f);
  state.flight.roll_deg = 5.0f * cosf(t * 0.4f);
  state.flight.yaw_deg = fmodf(t * 12.0f, 360.0f);
  state.flight.altitude_m = 112.0f + 2.5f * sinf(t * 0.08f);
  state.flight.vertical_speed_mps = 0.4f * cosf(t * 0.08f);
  state.flight.pressure_pa = 101325.0f - (state.flight.altitude_m * 12.0f);
  state.flight.ok = true;
  state.health.flags |= HEALTH_FLIGHT_OK;
}

void MeteoService::begin() {
  // TODO(v1): init BME688
}

void MeteoService::update(SystemState& state) {
  // TODO(v1): read BME688 values.
  const float t = state.timestamp_ms * 0.001f;
  state.meteo.temperature_c = 24.5f + 1.2f * sinf(t * 0.05f);
  state.meteo.humidity_rh = 52.0f + 2.0f * cosf(t * 0.035f);
  state.meteo.pressure_pa = state.flight.pressure_pa;
  state.meteo.gas_ohms = 9000.0f + 300.0f * sinf(t * 0.1f);
  state.meteo.ok = true;
  state.health.flags |= HEALTH_METEO_OK;
}

void GnssService::begin(HardwareSerial& serial) {
  _serial = &serial;
}

void GnssService::update(SystemState& state) {
  if (_serial) {
    while (_serial->available() > 0) {
      _gps.encode(static_cast<char>(_serial->read()));
    }
  }

  const uint32_t now = state.timestamp_ms;

  if (_gps.location.isValid()) {
    state.gnss.lat = _gps.location.lat();
    state.gnss.lon = _gps.location.lng();
  }
  if (_gps.altitude.isValid()) {
    state.gnss.altitude_m = static_cast<float>(_gps.altitude.meters());
  }
  if (_gps.speed.isValid()) {
    state.gnss.ground_speed_mps = static_cast<float>(_gps.speed.mps());
  }
  if (_gps.course.isValid()) {
    state.gnss.course_deg = static_cast<float>(_gps.course.deg());
  }
  if (_gps.hdop.isValid()) {
    state.gnss.hdop = static_cast<float>(_gps.hdop.hdop());
  }
  if (_gps.satellites.isValid()) {
    state.gnss.satellites = static_cast<uint8_t>(_gps.satellites.value());
  }

  if (_gps.location.isUpdated()) {
    _lastFixMs = now;
  }

  if (_lastFixMs == 0) {
    state.gnss.last_fix_age_ms = UINT32_MAX;
  } else {
    state.gnss.last_fix_age_ms = now - _lastFixMs;
  }

  state.gnss.ok = _gps.location.isValid() && state.gnss.last_fix_age_ms < kGnssFixStaleMs;
  if (state.gnss.ok) {
    state.gnss.fix_type = state.gnss.satellites >= 6 ? 3 : 2;
    state.health.flags |= HEALTH_GNSS_OK;
  } else {
    state.gnss.fix_type = 0;
    state.health.flags &= ~HEALTH_GNSS_OK;
    if ((now % 1000u) < 200u) {
      state.health.fault_count_gnss++;
    }
  }
}

void LoraService::begin() {
  _nodeId = static_cast<uint16_t>(ESP.getEfuseMac() & 0xFFFFu);
  if (_nodeId == 0) {
    _nodeId = 1;
  }

#if HAS_RADIOLIB
  g_loraSpi.begin(Pins::LORA_SCK, Pins::LORA_MISO, Pins::LORA_MOSI, Pins::LORA_NSS);

  int16_t radioState = g_loraRadio.begin(915.0);
  if (radioState == RADIOLIB_ERR_NONE) {
    g_loraRadio.setBandwidth(125.0);
    g_loraRadio.setSpreadingFactor(9);
    g_loraRadio.setCodingRate(7);
    g_loraRadio.setOutputPower(14);
    g_loraRadio.setCRC(true);
    g_loraRadio.startReceive();
    _radioReady = true;
    Serial.printf("LoRa mesh node started: %u\n", static_cast<unsigned>(_nodeId));
  } else {
    _radioReady = false;
    Serial.printf("LoRa init failed: %d\n", static_cast<int>(radioState));
  }
#else
  _radioReady = false;
  Serial.println("RadioLib missing, LoRa running in simulation mode");
#endif
}

void LoraService::update(SystemState& state) {
  state.radio.mesh_node_id = _nodeId;
  state.radio.mesh_enabled = _radioReady;

  char appPayload[96] = {0};
  const int payloadWritten = snprintf(
    appPayload,
    sizeof(appPayload),
    "t=%lu,lat=%.6f,lon=%.6f,alt=%.1f,var=%.2f,tmp=%.1f,hum=%.1f",
    static_cast<unsigned long>(state.timestamp_ms),
    state.gnss.lat,
    state.gnss.lon,
    state.flight.altitude_m,
    state.flight.vertical_speed_mps,
    state.meteo.temperature_c,
    state.meteo.humidity_rh);
  if (payloadWritten <= 0) {
    state.radio.ok = false;
    state.health.fault_count_lora++;
    return;
  }

  MeshFrame tx;
  tx.src = _nodeId;
  tx.dst = kMeshBroadcast;
  tx.ttl = kMeshDefaultTtl;
  tx.seq = ++_nextSeq;
  strncpy(tx.payload, appPayload, sizeof(tx.payload) - 1);
  tx.payload[sizeof(tx.payload) - 1] = '\0';

  char meshPacket[160] = {0};
  if (!buildMeshFrame(tx, meshPacket, sizeof(meshPacket))) {
    state.radio.ok = false;
    state.health.fault_count_lora++;
    return;
  }

  const bool txDue = (state.timestamp_ms - _lastTxMs) >= kLoraTxIntervalMs;
  if (txDue) {
    _lastTxMs = state.timestamp_ms;
#if HAS_RADIOLIB
    if (_radioReady) {
      int16_t txState = g_loraRadio.transmit(meshPacket);
      if (txState == RADIOLIB_ERR_NONE) {
        state.radio.packets_tx++;
        state.radio.ok = true;
        state.health.flags |= HEALTH_LORA_OK;
        strncpy(state.radio.last_payload, meshPacket, sizeof(state.radio.last_payload) - 1);
        state.radio.last_payload[sizeof(state.radio.last_payload) - 1] = '\0';
      } else {
        state.radio.ok = false;
        state.health.fault_count_lora++;
      }
      g_loraRadio.startReceive();
    } else {
      state.radio.ok = false;
      state.health.fault_count_lora++;
    }
#else
    state.radio.packets_tx++;
    state.radio.packets_rx = state.radio.packets_tx / 3;
    state.radio.ok = true;
    state.radio.rssi = -70;
    state.health.flags |= HEALTH_LORA_OK;
    strncpy(state.radio.last_payload, meshPacket, sizeof(state.radio.last_payload) - 1);
    state.radio.last_payload[sizeof(state.radio.last_payload) - 1] = '\0';
#endif
  }

#if HAS_RADIOLIB
  if (_radioReady) {
    String rxData;
    int16_t rxState = g_loraRadio.readData(rxData);
    if (rxState == RADIOLIB_ERR_NONE) {
      state.radio.packets_rx++;
      state.radio.rssi = static_cast<int16_t>(g_loraRadio.getRSSI());
      strncpy(state.radio.last_payload, rxData.c_str(), sizeof(state.radio.last_payload) - 1);
      state.radio.last_payload[sizeof(state.radio.last_payload) - 1] = '\0';

      MeshFrame rx;
      if (parseMeshFrame(rxData.c_str(), rx)) {
        state.radio.mesh_last_from = rx.src;
        state.radio.mesh_last_to = rx.dst;
        state.radio.mesh_last_hops = static_cast<uint8_t>(kMeshDefaultTtl - (rx.ttl > kMeshDefaultTtl ? kMeshDefaultTtl : rx.ttl));

        const bool shouldForward = (rx.src != _nodeId) && (rx.dst != _nodeId) && (rx.ttl > 1);
        if (shouldForward) {
          MeshFrame fwd = rx;
          fwd.ttl--;
          char fwdPacket[160] = {0};
          if (buildMeshFrame(fwd, fwdPacket, sizeof(fwdPacket))) {
            int16_t fwdState = g_loraRadio.transmit(fwdPacket);
            if (fwdState == RADIOLIB_ERR_NONE) {
              state.radio.mesh_packets_forwarded++;
            } else {
              state.radio.mesh_packets_dropped++;
            }
          } else {
            state.radio.mesh_packets_dropped++;
          }
          g_loraRadio.startReceive();
        }
      }
    }
  }
#endif
}

void LoggerService::begin() {
  // TODO(v1.1): open SD file and write CSV header.
}

void LoggerService::update(SystemState& state) {
  // TODO(v1.1): append a compact record to SD with buffered flush.
  state.connectivity.wifi_connected = true;
  state.connectivity.ble_connected = (state.timestamp_ms / 15000u) % 2u == 0u;
  state.connectivity.wifi_rssi_dbm = -62;
  state.connectivity.server_online = true;
  state.connectivity.server_clients = 1 + (state.radio.packets_tx % 2u);
  state.health.flags |= HEALTH_LOGGER_OK;
}

void UiService::begin() {
  _display.begin();
  if (_display.isReady()) {
    _miniOs.begin();
  }
}

void UiService::render(const SystemState& state) {
  if (!_display.isReady()) {
    return;
  }

  static uint32_t last_ms = 0;
  uint32_t now = millis();
  if (last_ms == 0) last_ms = now;
  
  uint32_t dt = now - last_ms;
  if (dt > 0) {
    lv_tick_inc(dt);
    last_ms = now;
  }

  _miniOs.render(state);
  _display.update();
}
