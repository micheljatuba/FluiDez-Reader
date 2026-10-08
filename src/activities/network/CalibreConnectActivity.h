#pragma once

#include <functional>
#include <memory>
#include <string>

#include "activities/Activity.h"
#include "activities/ScreenTransitionRefresh.h"
#include "network/FluiDezWebServer.h"

enum class CalibreConnectState { WIFI_SELECTION, SERVER_STARTING, SERVER_RUNNING, ERROR };

/**
 * CalibreConnectActivity starts the file transfer server in STA mode,
 * but renders Calibre-specific instructions instead of the web transfer UI.
 */
class CalibreConnectActivity final : public Activity {
  enum class TransferResult : uint8_t { None, Received, Failed };

  CalibreConnectState state = CalibreConnectState::WIFI_SELECTION;
  ScreenTransitionRefresh screenTransitionRefresh;

  std::unique_ptr<FluiDezWebServer> webServer;
  std::string connectedIP;
  std::string connectedSSID;
  unsigned long lastHandleClientTime = 0;
  // Status section. Strings are replaced only under RenderLock; progress
  // counters change many times per book and stay lock-free so a slow e-ink
  // refresh never stalls the transfer.
  std::string calibreIP;
  std::string currentUploadName;
  std::string resultName;
  TransferResult result = TransferResult::None;
  size_t lastProgressReceived = 0;
  size_t lastProgressTotal = 0;
  uint32_t receivedCount = 0;
  bool exitRequested = false;
  bool returnToReader = false;

  void onWifiSelectionComplete(bool connected);
  void startWebServer();
  void stopWebServer();
  void updateTransferStatus();

 public:
  explicit CalibreConnectActivity(GfxRenderer& renderer, MappedInputManager& mappedInput, bool returnToReader = false)
      : Activity("CalibreConnect", renderer, mappedInput), returnToReader(returnToReader) {}
  void onEnter() override;
  void onExit() override;
  void loop() override;
  void render(RenderLock&&) override;
  bool skipLoopDelay() override { return webServer && webServer->isRunning(); }
  bool preventAutoSleep() override { return webServer && webServer->isRunning(); }
};
