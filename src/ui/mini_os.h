#pragma once

#include <Arduino.h>
#include <lvgl.h>
#include "../models/state.h"

class MiniOsUi {
public:
  void begin();
  void render(const SystemState& state);

private:
  static constexpr uint8_t kPageCount = 7;

  bool _ready = false;
  bool _syncingDropdown = false;
  uint8_t _activePage = 0;
  MissionMode _lastRequestedMode = MissionMode::BOOT;
  uint32_t _lastDataRefreshMs = 0;
  uint32_t _lastAutoSwitchMs = 0;
  uint32_t _lastInteractionMs = 0;

  lv_obj_t* _root = nullptr;
  lv_obj_t* _statusBar = nullptr;
  lv_obj_t* _body = nullptr;
  lv_obj_t* _navBar = nullptr;

  lv_obj_t* _pages[kPageCount] = {nullptr};

  lv_obj_t* _lblStatusTime = nullptr;
  lv_obj_t* _lblStatusLinks = nullptr;
  lv_obj_t* _lblStatusBattery = nullptr;

  lv_obj_t* _btnPrev = nullptr;
  lv_obj_t* _btnNext = nullptr;
  lv_obj_t* _lblNavMode = nullptr;

  lv_obj_t* _lblHomeMode = nullptr;
  lv_obj_t* _lblHomeLinks = nullptr;
  lv_obj_t* _lblHomeHint = nullptr;

  lv_obj_t* _lblAvAttitude = nullptr;
  lv_obj_t* _lblAvAltitude = nullptr;
  lv_obj_t* _lblAvPressure = nullptr;

  lv_obj_t* _lblMeteoCore = nullptr;
  lv_obj_t* _lblMeteoGas = nullptr;
  lv_obj_t* _lblMeteoHealth = nullptr;

  lv_obj_t* _lblTrackerFix = nullptr;
  lv_obj_t* _lblTrackerCoords = nullptr;
  lv_obj_t* _lblTrackerRadio = nullptr;

  lv_obj_t* _lblGpsFix = nullptr;
  lv_obj_t* _lblGpsNav = nullptr;
  lv_obj_t* _lblGpsQuality = nullptr;

  lv_obj_t* _lblBoxStorage = nullptr;
  lv_obj_t* _lblBoxFaults = nullptr;
  lv_obj_t* _lblBoxPayload = nullptr;

  lv_obj_t* _ddMode = nullptr;
  lv_obj_t* _swWifi = nullptr;
  lv_obj_t* _swBle = nullptr;
  lv_obj_t* _swLora = nullptr;
  lv_obj_t* _swGps = nullptr;
  lv_obj_t* _swServer = nullptr;
  lv_obj_t* _lblCfgServer = nullptr;

  void buildTheme();
  void createStatusBar();
  void createBody();
  void createNavBar();
  void createHomePage(lv_obj_t* page);
  void createAvionicsPage(lv_obj_t* page);
  void createMeteoPage(lv_obj_t* page);
  void createTrackerPage(lv_obj_t* page);
  void createGpsPage(lv_obj_t* page);
  void createBlackBoxPage(lv_obj_t* page);
  void createConfigPage(lv_obj_t* page);

  void setActivePage(uint8_t index, bool fromUser);
  void syncDropdownSelection();

  void refreshStatusBar(const SystemState& state);
  void refreshPages(const SystemState& state);

  static void onPrevClicked(lv_event_t* e);
  static void onNextClicked(lv_event_t* e);
  static void onModeDropdownChanged(lv_event_t* e);

  static uint8_t modeToPage(MissionMode mode);
  static const char* pageName(uint8_t index);
  static const char* pageIcon(uint8_t index);
};
