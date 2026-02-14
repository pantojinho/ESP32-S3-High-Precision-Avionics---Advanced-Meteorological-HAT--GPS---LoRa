#include "mini_os.h"

#include <cstdio>
#include <cstring>

namespace {

constexpr int32_t kScreenHeight = 456;
constexpr int32_t kStatusBarHeight = 38;
constexpr int32_t kNavBarHeight = 52;
constexpr int32_t kBodyHeight = kScreenHeight - kStatusBarHeight - kNavBarHeight;

constexpr uint32_t kUiRefreshMs = 200;
constexpr uint32_t kAutoCycleMs = 9000;
constexpr uint32_t kAutoCycleIdleMs = 15000;

constexpr uint32_t kColorText = 0xEAF6FF;
constexpr uint32_t kColorSubtle = 0x8CB8D9;
constexpr uint32_t kColorCard = 0x0A1D2E;
constexpr uint32_t kColorCardAlt = 0x122A3D;

const char* batteryIcon(float percent) {
  if (percent > 80.0f) {
    return LV_SYMBOL_BATTERY_FULL;
  }
  if (percent > 60.0f) {
    return LV_SYMBOL_BATTERY_3;
  }
  if (percent > 35.0f) {
    return LV_SYMBOL_BATTERY_2;
  }
  if (percent > 15.0f) {
    return LV_SYMBOL_BATTERY_1;
  }
  return LV_SYMBOL_BATTERY_EMPTY;
}

void setSwitchState(lv_obj_t* sw, bool enabled) {
  if (!sw) {
    return;
  }
  const bool checked = lv_obj_has_state(sw, LV_STATE_CHECKED);
  if (enabled && !checked) {
    lv_obj_add_state(sw, LV_STATE_CHECKED);
  } else if (!enabled && checked) {
    lv_obj_clear_state(sw, LV_STATE_CHECKED);
  }
}

lv_obj_t* createPageCard(lv_obj_t* parent, int32_t y, int32_t h, const char* title, uint32_t accent, lv_obj_t** outValueLabel) {
  lv_obj_t* card = lv_obj_create(parent);
  lv_obj_set_size(card, LV_PCT(100), h);
  lv_obj_align(card, LV_ALIGN_TOP_LEFT, 0, y);
  lv_obj_set_style_radius(card, 14, LV_PART_MAIN);
  lv_obj_set_style_bg_color(card, lv_color_hex(kColorCard), LV_PART_MAIN);
  lv_obj_set_style_bg_opa(card, LV_OPA_80, LV_PART_MAIN);
  lv_obj_set_style_border_width(card, 1, LV_PART_MAIN);
  lv_obj_set_style_border_color(card, lv_color_hex(accent), LV_PART_MAIN);
  lv_obj_set_style_pad_all(card, 8, LV_PART_MAIN);
  lv_obj_set_style_shadow_width(card, 0, LV_PART_MAIN);

  lv_obj_t* titleLabel = lv_label_create(card);
  lv_label_set_text(titleLabel, title);
  lv_obj_set_style_text_color(titleLabel, lv_color_hex(accent), LV_PART_MAIN);
  lv_obj_align(titleLabel, LV_ALIGN_TOP_LEFT, 0, 0);

  lv_obj_t* valueLabel = lv_label_create(card);
  lv_label_set_text(valueLabel, "-");
  lv_obj_set_width(valueLabel, LV_PCT(100));
  lv_label_set_long_mode(valueLabel, LV_LABEL_LONG_WRAP);
  lv_obj_set_style_text_color(valueLabel, lv_color_hex(kColorText), LV_PART_MAIN);
  lv_obj_set_style_text_align(valueLabel, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN);
  lv_obj_align(valueLabel, LV_ALIGN_TOP_LEFT, 0, 24);

  if (outValueLabel) {
    *outValueLabel = valueLabel;
  }
  return card;
}

lv_obj_t* createSwitchRow(lv_obj_t* parent, const char* labelText, lv_obj_t** outSwitch) {
  lv_obj_t* row = lv_obj_create(parent);
  lv_obj_set_width(row, LV_PCT(100));
  lv_obj_set_height(row, 30);
  lv_obj_set_style_bg_opa(row, LV_OPA_TRANSP, LV_PART_MAIN);
  lv_obj_set_style_border_width(row, 0, LV_PART_MAIN);
  lv_obj_set_style_pad_all(row, 0, LV_PART_MAIN);
  lv_obj_set_layout(row, LV_LAYOUT_FLEX);
  lv_obj_set_flex_flow(row, LV_FLEX_FLOW_ROW);
  lv_obj_set_flex_align(row, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

  lv_obj_t* label = lv_label_create(row);
  lv_label_set_text(label, labelText);
  lv_obj_set_style_text_color(label, lv_color_hex(kColorText), LV_PART_MAIN);

  lv_obj_t* sw = lv_switch_create(row);
  lv_obj_set_size(sw, 46, 24);
  if (outSwitch) {
    *outSwitch = sw;
  }
  return row;
}

}  // namespace

void MiniOsUi::begin() {
  lv_obj_t* screen = lv_screen_active();
  if (!screen) {
    return;
  }

  lv_obj_clean(screen);

  _root = lv_obj_create(screen);
  lv_obj_remove_style_all(_root);
  lv_obj_set_size(_root, LV_PCT(100), LV_PCT(100));
  lv_obj_align(_root, LV_ALIGN_TOP_LEFT, 0, 0);

  buildTheme();
  createStatusBar();
  createBody();
  createNavBar();

  setActivePage(0, false);

  const uint32_t now = millis();
  _lastDataRefreshMs = now;
  _lastAutoSwitchMs = now;
  _lastInteractionMs = now;
  _ready = true;
}

void MiniOsUi::render(const SystemState& state) {
  if (!_ready) {
    return;
  }

  const uint32_t now = millis();

  if (state.ui.requested_mode != _lastRequestedMode) {
    _lastRequestedMode = state.ui.requested_mode;
    if (state.ui.requested_mode != MissionMode::BOOT) {
      const uint8_t requested = modeToPage(state.ui.requested_mode);
      if (requested != _activePage) {
        setActivePage(requested, false);
        _lastAutoSwitchMs = now;
      }
    }
  }

  if (state.ui.auto_cycle && (now - _lastInteractionMs) > kAutoCycleIdleMs && (now - _lastAutoSwitchMs) > kAutoCycleMs) {
    setActivePage(static_cast<uint8_t>((_activePage + 1u) % kPageCount), false);
    _lastAutoSwitchMs = now;
  }

  if ((now - _lastDataRefreshMs) < kUiRefreshMs) {
    return;
  }
  _lastDataRefreshMs = now;

  refreshStatusBar(state);
  refreshPages(state);
}

void MiniOsUi::buildTheme() {
  lv_obj_t* screen = lv_screen_active();
  lv_obj_set_style_bg_color(screen, lv_color_hex(0x04121F), LV_PART_MAIN);
  lv_obj_set_style_bg_grad_color(screen, lv_color_hex(0x0A2538), LV_PART_MAIN);
  lv_obj_set_style_bg_grad_dir(screen, LV_GRAD_DIR_VER, LV_PART_MAIN);
  lv_obj_set_style_bg_opa(screen, LV_OPA_COVER, LV_PART_MAIN);

  lv_obj_set_style_bg_opa(_root, LV_OPA_TRANSP, LV_PART_MAIN);
  lv_obj_set_style_border_width(_root, 0, LV_PART_MAIN);
  lv_obj_set_style_radius(_root, 0, LV_PART_MAIN);
  lv_obj_set_style_pad_all(_root, 0, LV_PART_MAIN);
}

void MiniOsUi::createStatusBar() {
  _statusBar = lv_obj_create(_root);
  lv_obj_set_size(_statusBar, LV_PCT(100), kStatusBarHeight);
  lv_obj_align(_statusBar, LV_ALIGN_TOP_MID, 0, 0);
  lv_obj_set_style_radius(_statusBar, 0, LV_PART_MAIN);
  lv_obj_set_style_bg_color(_statusBar, lv_color_hex(0x081726), LV_PART_MAIN);
  lv_obj_set_style_bg_opa(_statusBar, LV_OPA_90, LV_PART_MAIN);
  lv_obj_set_style_border_width(_statusBar, 0, LV_PART_MAIN);
  lv_obj_set_style_pad_left(_statusBar, 8, LV_PART_MAIN);
  lv_obj_set_style_pad_right(_statusBar, 8, LV_PART_MAIN);
  lv_obj_set_style_pad_top(_statusBar, 6, LV_PART_MAIN);
  lv_obj_set_style_pad_bottom(_statusBar, 6, LV_PART_MAIN);
  lv_obj_set_layout(_statusBar, LV_LAYOUT_FLEX);
  lv_obj_set_flex_flow(_statusBar, LV_FLEX_FLOW_ROW);
  lv_obj_set_flex_align(_statusBar, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

  _lblStatusTime = lv_label_create(_statusBar);
  lv_obj_set_style_text_color(_lblStatusTime, lv_color_hex(kColorText), LV_PART_MAIN);
  lv_label_set_text(_lblStatusTime, "T+00:00");

  _lblStatusLinks = lv_label_create(_statusBar);
  lv_obj_set_style_text_color(_lblStatusLinks, lv_color_hex(kColorSubtle), LV_PART_MAIN);
  lv_label_set_text(_lblStatusLinks, "W- B- L- G-");

  _lblStatusBattery = lv_label_create(_statusBar);
  lv_obj_set_style_text_color(_lblStatusBattery, lv_color_hex(kColorText), LV_PART_MAIN);
  lv_label_set_text(_lblStatusBattery, LV_SYMBOL_BATTERY_EMPTY " 0%");
}

void MiniOsUi::createBody() {
  _body = lv_obj_create(_root);
  lv_obj_set_size(_body, LV_PCT(100), kBodyHeight);
  lv_obj_align(_body, LV_ALIGN_TOP_MID, 0, kStatusBarHeight);
  lv_obj_set_style_radius(_body, 0, LV_PART_MAIN);
  lv_obj_set_style_bg_opa(_body, LV_OPA_TRANSP, LV_PART_MAIN);
  lv_obj_set_style_border_width(_body, 0, LV_PART_MAIN);
  lv_obj_set_style_pad_all(_body, 6, LV_PART_MAIN);

  for (uint8_t i = 0; i < kPageCount; ++i) {
    _pages[i] = lv_obj_create(_body);
    lv_obj_set_size(_pages[i], LV_PCT(100), LV_PCT(100));
    lv_obj_align(_pages[i], LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_radius(_pages[i], 0, LV_PART_MAIN);
    lv_obj_set_style_bg_opa(_pages[i], LV_OPA_TRANSP, LV_PART_MAIN);
    lv_obj_set_style_border_width(_pages[i], 0, LV_PART_MAIN);
    lv_obj_set_style_pad_all(_pages[i], 0, LV_PART_MAIN);
    lv_obj_clear_flag(_pages[i], LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_flag(_pages[i], LV_OBJ_FLAG_HIDDEN);
  }

  createHomePage(_pages[0]);
  createAvionicsPage(_pages[1]);
  createMeteoPage(_pages[2]);
  createTrackerPage(_pages[3]);
  createGpsPage(_pages[4]);
  createBlackBoxPage(_pages[5]);
  createConfigPage(_pages[6]);
}

void MiniOsUi::createNavBar() {
  _navBar = lv_obj_create(_root);
  lv_obj_set_size(_navBar, LV_PCT(100), kNavBarHeight);
  lv_obj_align(_navBar, LV_ALIGN_BOTTOM_MID, 0, 0);
  lv_obj_set_style_radius(_navBar, 0, LV_PART_MAIN);
  lv_obj_set_style_bg_color(_navBar, lv_color_hex(0x081726), LV_PART_MAIN);
  lv_obj_set_style_bg_opa(_navBar, LV_OPA_95, LV_PART_MAIN);
  lv_obj_set_style_border_width(_navBar, 0, LV_PART_MAIN);
  lv_obj_set_style_pad_left(_navBar, 8, LV_PART_MAIN);
  lv_obj_set_style_pad_right(_navBar, 8, LV_PART_MAIN);
  lv_obj_set_style_pad_top(_navBar, 8, LV_PART_MAIN);
  lv_obj_set_style_pad_bottom(_navBar, 8, LV_PART_MAIN);
  lv_obj_set_layout(_navBar, LV_LAYOUT_FLEX);
  lv_obj_set_flex_flow(_navBar, LV_FLEX_FLOW_ROW);
  lv_obj_set_flex_align(_navBar, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

  _btnPrev = lv_button_create(_navBar);
  lv_obj_set_size(_btnPrev, 74, 34);
  lv_obj_set_style_bg_color(_btnPrev, lv_color_hex(kColorCardAlt), LV_PART_MAIN);
  lv_obj_set_style_border_width(_btnPrev, 1, LV_PART_MAIN);
  lv_obj_set_style_border_color(_btnPrev, lv_color_hex(0x1D5578), LV_PART_MAIN);
  lv_obj_add_event_cb(_btnPrev, onPrevClicked, LV_EVENT_CLICKED, this);
  lv_obj_t* prevLabel = lv_label_create(_btnPrev);
  lv_label_set_text(prevLabel, LV_SYMBOL_LEFT " Prev");
  lv_obj_center(prevLabel);

  _lblNavMode = lv_label_create(_navBar);
  lv_obj_set_style_text_color(_lblNavMode, lv_color_hex(kColorText), LV_PART_MAIN);
  lv_obj_set_style_text_align(_lblNavMode, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
  lv_label_set_text(_lblNavMode, LV_SYMBOL_HOME " Home");

  _btnNext = lv_button_create(_navBar);
  lv_obj_set_size(_btnNext, 74, 34);
  lv_obj_set_style_bg_color(_btnNext, lv_color_hex(kColorCardAlt), LV_PART_MAIN);
  lv_obj_set_style_border_width(_btnNext, 1, LV_PART_MAIN);
  lv_obj_set_style_border_color(_btnNext, lv_color_hex(0x1D5578), LV_PART_MAIN);
  lv_obj_add_event_cb(_btnNext, onNextClicked, LV_EVENT_CLICKED, this);
  lv_obj_t* nextLabel = lv_label_create(_btnNext);
  lv_label_set_text(nextLabel, "Next " LV_SYMBOL_RIGHT);
  lv_obj_center(nextLabel);
}

void MiniOsUi::createHomePage(lv_obj_t* page) {
  lv_obj_t* title = lv_label_create(page);
  lv_label_set_text(title, LV_SYMBOL_HOME " MINI OS");
  lv_obj_set_style_text_color(title, lv_color_hex(0x9ED5FF), LV_PART_MAIN);
  lv_obj_align(title, LV_ALIGN_TOP_LEFT, 2, 2);

  createPageCard(page, 30, 102, "Mission Profile", 0x38B6FF, &_lblHomeMode);
  createPageCard(page, 140, 114, "Comms Matrix", 0x4CD39B, &_lblHomeLinks);
  createPageCard(page, 262, 96, "Quick Ops", 0xEAA95A, &_lblHomeHint);
}

void MiniOsUi::createAvionicsPage(lv_obj_t* page) {
  lv_obj_t* title = lv_label_create(page);
  lv_label_set_text(title, LV_SYMBOL_CHARGE " AVIONICS");
  lv_obj_set_style_text_color(title, lv_color_hex(0xA8E0FF), LV_PART_MAIN);
  lv_obj_align(title, LV_ALIGN_TOP_LEFT, 2, 2);

  createPageCard(page, 30, 104, "Attitude", 0x2AC3DE, &_lblAvAttitude);
  createPageCard(page, 142, 104, "Altimetry", 0x3BD28A, &_lblAvAltitude);
  createPageCard(page, 254, 104, "Pressure", 0xF1A65A, &_lblAvPressure);
}

void MiniOsUi::createMeteoPage(lv_obj_t* page) {
  lv_obj_t* title = lv_label_create(page);
  lv_label_set_text(title, LV_SYMBOL_TINT " METEO");
  lv_obj_set_style_text_color(title, lv_color_hex(0x98D8FF), LV_PART_MAIN);
  lv_obj_align(title, LV_ALIGN_TOP_LEFT, 2, 2);

  createPageCard(page, 30, 104, "Atmosphere Core", 0x48C1FF, &_lblMeteoCore);
  createPageCard(page, 142, 104, "Gas Channel", 0x63CF8C, &_lblMeteoGas);
  createPageCard(page, 254, 104, "Health", 0xF3A85E, &_lblMeteoHealth);
}

void MiniOsUi::createTrackerPage(lv_obj_t* page) {
  lv_obj_t* title = lv_label_create(page);
  lv_label_set_text(title, LV_SYMBOL_GPS " TRACKER");
  lv_obj_set_style_text_color(title, lv_color_hex(0x9FD9FF), LV_PART_MAIN);
  lv_obj_align(title, LV_ALIGN_TOP_LEFT, 2, 2);

  createPageCard(page, 30, 92, "GNSS Fix", 0x2BC7EE, &_lblTrackerFix);
  createPageCard(page, 130, 104, "Coordinates", 0x43CD8E, &_lblTrackerCoords);
  createPageCard(page, 242, 116, "LoRa Telemetry", 0xEEA562, &_lblTrackerRadio);
}

void MiniOsUi::createGpsPage(lv_obj_t* page) {
  lv_obj_t* title = lv_label_create(page);
  lv_label_set_text(title, LV_SYMBOL_GPS " GPS MODE");
  lv_obj_set_style_text_color(title, lv_color_hex(0xA7DDFF), LV_PART_MAIN);
  lv_obj_align(title, LV_ALIGN_TOP_LEFT, 2, 2);

  createPageCard(page, 30, 96, "Fix Monitor", 0x35BBEE, &_lblGpsFix);
  createPageCard(page, 134, 104, "Navigation", 0x4ACF90, &_lblGpsNav);
  createPageCard(page, 246, 112, "Signal Quality", 0xEEA55F, &_lblGpsQuality);
}

void MiniOsUi::createBlackBoxPage(lv_obj_t* page) {
  lv_obj_t* title = lv_label_create(page);
  lv_label_set_text(title, LV_SYMBOL_SAVE " BLACK BOX");
  lv_obj_set_style_text_color(title, lv_color_hex(0xA6DFFF), LV_PART_MAIN);
  lv_obj_align(title, LV_ALIGN_TOP_LEFT, 2, 2);

  createPageCard(page, 30, 88, "Storage", 0x40BFF4, &_lblBoxStorage);
  createPageCard(page, 126, 98, "Fault Counters", 0x67C78E, &_lblBoxFaults);
  createPageCard(page, 232, 126, "Last Payload", 0xF0AB62, &_lblBoxPayload);
}

void MiniOsUi::createConfigPage(lv_obj_t* page) {
  lv_obj_t* title = lv_label_create(page);
  lv_label_set_text(title, LV_SYMBOL_SETTINGS " CONFIG");
  lv_obj_set_style_text_color(title, lv_color_hex(0xA9E1FF), LV_PART_MAIN);
  lv_obj_align(title, LV_ALIGN_TOP_LEFT, 2, 2);

  lv_obj_t* modeCard = createPageCard(page, 30, 84, "Mode Selector", 0x38B6FF, nullptr);
  _ddMode = lv_dropdown_create(modeCard);
  lv_obj_set_width(_ddMode, LV_PCT(100));
  lv_dropdown_set_options(_ddMode, "Home\nAvionics\nMeteo\nTracker\nGPS\nBlack Box\nConfig");
  lv_obj_align(_ddMode, LV_ALIGN_TOP_LEFT, 0, 24);
  lv_obj_add_event_cb(_ddMode, onModeDropdownChanged, LV_EVENT_VALUE_CHANGED, this);

  lv_obj_t* linksCard = lv_obj_create(page);
  lv_obj_set_size(linksCard, LV_PCT(100), 168);
  lv_obj_align(linksCard, LV_ALIGN_TOP_LEFT, 0, 122);
  lv_obj_set_style_radius(linksCard, 14, LV_PART_MAIN);
  lv_obj_set_style_bg_color(linksCard, lv_color_hex(kColorCard), LV_PART_MAIN);
  lv_obj_set_style_bg_opa(linksCard, LV_OPA_80, LV_PART_MAIN);
  lv_obj_set_style_border_width(linksCard, 1, LV_PART_MAIN);
  lv_obj_set_style_border_color(linksCard, lv_color_hex(0x4BCF96), LV_PART_MAIN);
  lv_obj_set_style_pad_all(linksCard, 8, LV_PART_MAIN);
  lv_obj_set_layout(linksCard, LV_LAYOUT_FLEX);
  lv_obj_set_flex_flow(linksCard, LV_FLEX_FLOW_COLUMN);
  lv_obj_set_flex_align(linksCard, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER);

  lv_obj_t* linksTitle = lv_label_create(linksCard);
  lv_label_set_text(linksTitle, "Connectivity");
  lv_obj_set_style_text_color(linksTitle, lv_color_hex(0x4BCF96), LV_PART_MAIN);

  createSwitchRow(linksCard, "Wi-Fi", &_swWifi);
  createSwitchRow(linksCard, "Bluetooth LE", &_swBle);
  createSwitchRow(linksCard, "LoRa Radio", &_swLora);
  createSwitchRow(linksCard, "GNSS", &_swGps);
  createSwitchRow(linksCard, "Web Server", &_swServer);

  createPageCard(page, 298, 60, "Server Profile", 0xEDA45F, &_lblCfgServer);
}

void MiniOsUi::setActivePage(uint8_t index, bool fromUser) {
  if (index >= kPageCount) {
    return;
  }

  for (uint8_t i = 0; i < kPageCount; ++i) {
    if (_pages[i] == nullptr) {
      continue;
    }
    if (i == index) {
      lv_obj_clear_flag(_pages[i], LV_OBJ_FLAG_HIDDEN);
    } else {
      lv_obj_add_flag(_pages[i], LV_OBJ_FLAG_HIDDEN);
    }
  }

  _activePage = index;
  if (_lblNavMode) {
    lv_label_set_text_fmt(_lblNavMode, "%s %s", pageIcon(_activePage), pageName(_activePage));
  }

  syncDropdownSelection();

  if (fromUser) {
    _lastInteractionMs = millis();
  }
}

void MiniOsUi::syncDropdownSelection() {
  if (!_ddMode) {
    return;
  }
  _syncingDropdown = true;
  lv_dropdown_set_selected(_ddMode, _activePage);
  _syncingDropdown = false;
}

void MiniOsUi::refreshStatusBar(const SystemState& state) {
  const uint32_t seconds = state.timestamp_ms / 1000u;
  const uint32_t minutes = seconds / 60u;
  const uint32_t secs = seconds % 60u;
  lv_label_set_text_fmt(_lblStatusTime, "T+%02lu:%02lu", static_cast<unsigned long>(minutes), static_cast<unsigned long>(secs));

  lv_label_set_text_fmt(
    _lblStatusLinks,
    "W%s B%s L%s G%s",
    state.connectivity.wifi_connected ? "+" : "-",
    state.connectivity.ble_connected ? "+" : "-",
    state.radio.ok ? "+" : "-",
    state.gnss.ok ? "+" : "-");

  lv_label_set_text_fmt(_lblStatusBattery, "%s %.0f%%", batteryIcon(state.power.battery_percent), state.power.battery_percent);
}

void MiniOsUi::refreshPages(const SystemState& state) {
  lv_label_set_text_fmt(
    _lblHomeMode,
    "%s %s\nMode request: %u\nAuto-cycle: %s",
    pageIcon(_activePage),
    pageName(_activePage),
    static_cast<unsigned>(state.ui.requested_mode),
    state.ui.auto_cycle ? "ON" : "OFF");

  lv_label_set_text_fmt(
    _lblHomeLinks,
    "Wi-Fi: %s (%d dBm)\nBLE: %s\nLoRa: TX %lu / RX %lu\nGPS: %s",
    state.connectivity.wifi_connected ? "ONLINE" : "OFFLINE",
    static_cast<int>(state.connectivity.wifi_rssi_dbm),
    state.connectivity.ble_connected ? "ONLINE" : "OFFLINE",
    static_cast<unsigned long>(state.radio.packets_tx),
    static_cast<unsigned long>(state.radio.packets_rx),
    state.gnss.ok ? "FIX" : "NO FIX");

  lv_label_set_text_fmt(
    _lblHomeHint,
    "Prev/Next: swap pages\nConfig: mode + radios\nBlack Box: diagnostics");

  lv_label_set_text_fmt(
    _lblAvAttitude,
    "Pitch: %.1f deg\nRoll: %.1f deg\nYaw: %.1f deg",
    state.flight.pitch_deg,
    state.flight.roll_deg,
    state.flight.yaw_deg);

  lv_label_set_text_fmt(
    _lblAvAltitude,
    "Altitude: %.1f m\nVario: %.2f m/s",
    state.flight.altitude_m,
    state.flight.vertical_speed_mps);

  lv_label_set_text_fmt(
    _lblAvPressure,
    "Pressure: %.0f Pa\nStatus: %s",
    state.flight.pressure_pa,
    state.flight.ok ? "OK" : "FAULT");

  lv_label_set_text_fmt(
    _lblMeteoCore,
    "Temp: %.1f C\nHumidity: %.1f %%RH\nPressure: %.0f Pa",
    state.meteo.temperature_c,
    state.meteo.humidity_rh,
    state.meteo.pressure_pa);

  lv_label_set_text_fmt(_lblMeteoGas, "Gas resistance: %.0f ohms", state.meteo.gas_ohms);

  lv_label_set_text_fmt(
    _lblMeteoHealth,
    "Meteo: %s\nHealth flags: 0x%08lX",
    state.meteo.ok ? "OK" : "FAULT",
    static_cast<unsigned long>(state.health.flags));

  lv_label_set_text_fmt(
    _lblTrackerFix,
    "Fix: %s (type %u)\nSatellites: %u",
    state.gnss.ok ? "LOCKED" : "SEARCHING",
    static_cast<unsigned>(state.gnss.fix_type),
    static_cast<unsigned>(state.gnss.satellites));

  lv_label_set_text_fmt(
    _lblTrackerCoords,
    "Lat: %.6f\nLon: %.6f\nAge: %lu ms",
    state.gnss.lat,
    state.gnss.lon,
    static_cast<unsigned long>(state.gnss.last_fix_age_ms));

  lv_label_set_text_fmt(
    _lblTrackerRadio,
    "RSSI: %d dBm\nTX/RX: %lu/%lu  Node: %u\nMesh fwd/drop: %lu/%lu",
    static_cast<int>(state.radio.rssi),
    static_cast<unsigned long>(state.radio.packets_tx),
    static_cast<unsigned long>(state.radio.packets_rx),
    static_cast<unsigned>(state.radio.mesh_node_id),
    static_cast<unsigned long>(state.radio.mesh_packets_forwarded),
    static_cast<unsigned long>(state.radio.mesh_packets_dropped));

  lv_label_set_text_fmt(
    _lblGpsFix,
    "Fix: %s  Type: %u\nSat: %u  HDOP: %.1f\nAge: %lu ms",
    state.gnss.ok ? "LOCKED" : "NO FIX",
    static_cast<unsigned>(state.gnss.fix_type),
    static_cast<unsigned>(state.gnss.satellites),
    state.gnss.hdop,
    static_cast<unsigned long>(state.gnss.last_fix_age_ms));

  lv_label_set_text_fmt(
    _lblGpsNav,
    "Speed: %.2f m/s\nCourse: %.1f deg\nAltitude: %.1f m",
    state.gnss.ground_speed_mps,
    state.gnss.course_deg,
    state.gnss.altitude_m);

  lv_label_set_text_fmt(
    _lblGpsQuality,
    "Lat: %.6f\nLon: %.6f\nHealth: %s",
    state.gnss.lat,
    state.gnss.lon,
    state.gnss.ok ? "GOOD" : "DEGRADED");

  lv_label_set_text_fmt(
    _lblBoxStorage,
    "Logger: %s\nServer: %s:%u",
    (state.health.flags & HEALTH_LOGGER_OK) ? "READY" : "OFF",
    state.connectivity.server_online ? "ONLINE" : "OFFLINE",
    static_cast<unsigned>(state.connectivity.server_port));

  lv_label_set_text_fmt(
    _lblBoxFaults,
    "Flight: %lu  Meteo: %lu\nGNSS: %lu  LoRa: %lu\nLogger: %lu",
    static_cast<unsigned long>(state.health.fault_count_flight),
    static_cast<unsigned long>(state.health.fault_count_meteo),
    static_cast<unsigned long>(state.health.fault_count_gnss),
    static_cast<unsigned long>(state.health.fault_count_lora),
    static_cast<unsigned long>(state.health.fault_count_logger));

  char payloadPreview[80] = {0};
  strncpy(payloadPreview, state.radio.last_payload, sizeof(payloadPreview) - 1);
  payloadPreview[sizeof(payloadPreview) - 1] = '\0';
  lv_label_set_text_fmt(_lblBoxPayload, "%s", payloadPreview[0] ? payloadPreview : "(empty)");

  setSwitchState(_swWifi, state.connectivity.wifi_enabled);
  setSwitchState(_swBle, state.connectivity.ble_enabled);
  setSwitchState(_swLora, state.radio.ok);
  setSwitchState(_swGps, state.gnss.ok);
  setSwitchState(_swServer, state.connectivity.server_enabled);

  lv_label_set_text_fmt(
    _lblCfgServer,
    "State: %s\nPort: %u  Clients: %u",
    state.connectivity.server_online ? "ONLINE" : "OFFLINE",
    static_cast<unsigned>(state.connectivity.server_port),
    static_cast<unsigned>(state.connectivity.server_clients));
}

void MiniOsUi::onPrevClicked(lv_event_t* e) {
  MiniOsUi* self = static_cast<MiniOsUi*>(lv_event_get_user_data(e));
  if (!self) {
    return;
  }
  const uint8_t next = static_cast<uint8_t>((self->_activePage + kPageCount - 1u) % kPageCount);
  self->setActivePage(next, true);
}

void MiniOsUi::onNextClicked(lv_event_t* e) {
  MiniOsUi* self = static_cast<MiniOsUi*>(lv_event_get_user_data(e));
  if (!self) {
    return;
  }
  const uint8_t next = static_cast<uint8_t>((self->_activePage + 1u) % kPageCount);
  self->setActivePage(next, true);
}

void MiniOsUi::onModeDropdownChanged(lv_event_t* e) {
  MiniOsUi* self = static_cast<MiniOsUi*>(lv_event_get_user_data(e));
  if (!self || self->_syncingDropdown) {
    return;
  }
  lv_obj_t* target = static_cast<lv_obj_t*>(lv_event_get_target(e));
  if (!target) {
    return;
  }
  const uint16_t selected = lv_dropdown_get_selected(target);
  if (selected < kPageCount) {
    self->setActivePage(static_cast<uint8_t>(selected), true);
  }
}

uint8_t MiniOsUi::modeToPage(MissionMode mode) {
  switch (mode) {
    case MissionMode::AVIONICS:
      return 1;
    case MissionMode::METEO:
      return 2;
    case MissionMode::TRACKER:
      return 3;
    case MissionMode::GPS:
      return 4;
    case MissionMode::BLACK_BOX:
      return 5;
    case MissionMode::CONFIG:
      return 6;
    case MissionMode::BOOT:
    default:
      return 0;
  }
}

const char* MiniOsUi::pageName(uint8_t index) {
  switch (index) {
    case 1:
      return "Avionics";
    case 2:
      return "Meteo";
    case 3:
      return "Tracker";
    case 4:
      return "GPS";
    case 5:
      return "Black Box";
    case 6:
      return "Config";
    case 0:
    default:
      return "Home";
  }
}

const char* MiniOsUi::pageIcon(uint8_t index) {
  switch (index) {
    case 1:
      return LV_SYMBOL_CHARGE;
    case 2:
      return LV_SYMBOL_TINT;
    case 3:
      return LV_SYMBOL_GPS;
    case 4:
      return LV_SYMBOL_GPS;
    case 5:
      return LV_SYMBOL_SAVE;
    case 6:
      return LV_SYMBOL_SETTINGS;
    case 0:
    default:
      return LV_SYMBOL_HOME;
  }
}
