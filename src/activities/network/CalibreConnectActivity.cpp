#include "CalibreConnectActivity.h"

#include <ESPmDNS.h>
#include <GfxRenderer.h>
#include <I18n.h>
#include <LibraryBuilder.h>
#include <WiFi.h>
#include <esp_task_wdt.h>

#include <string>
#include <string_view>

#include "MappedInputManager.h"
#include "SdCardFontSystem.h"
#include "SilentRestart.h"
#include "WifiSelectionActivity.h"
#include "components/CompactHeader.h"
#include "components/UITheme.h"
#include "fontIds.h"

namespace {
constexpr const char* HOSTNAME = "fluidez";
// How long a received or failed book stays visible under "Status".
constexpr unsigned long RESULT_VISIBLE_MS = 6000;
}  // namespace

void CalibreConnectActivity::onEnter() {
  Activity::onEnter();
  sdFontSystem.releaseLoadedFont(renderer);

  requestUpdate();
  state = CalibreConnectState::WIFI_SELECTION;
  connectedIP.clear();
  connectedSSID.clear();
  lastHandleClientTime = 0;
  calibreIP.clear();
  currentUploadName.clear();
  resultName.clear();
  result = TransferResult::None;
  lastProgressReceived = 0;
  lastProgressTotal = 0;
  receivedCount = 0;
  exitRequested = false;

  if (WiFi.status() != WL_CONNECTED) {
    startActivityForResult(std::make_unique<WifiSelectionActivity>(renderer, mappedInput),
                           [this](const ActivityResult& result) {
                             if (!result.isCancelled) {
                               const auto& wifi = std::get<WifiResult>(result.data);
                               connectedIP = wifi.ip;
                               connectedSSID = wifi.ssid;
                             }
                             onWifiSelectionComplete(!result.isCancelled);
                           });
  } else {
    connectedIP = WiFi.localIP().toString().c_str();
    connectedSSID = WiFi.SSID().c_str();
    startWebServer();
  }
}

void CalibreConnectActivity::onExit() {
  library::invalidateLibraryIndex();
  Activity::onExit();

  MDNS.end();

  if (WiFi.getMode() != WIFI_MODE_NULL) {
    WiFi.disconnect(false);
    delay(30);
    if (returnToReader) {
      silentRestartToReader();
    } else {
      silentRestart();
    }
  }
}

void CalibreConnectActivity::onWifiSelectionComplete(const bool connected) {
  if (!connected) {
    finish();
    return;
  }

  startWebServer();
}

void CalibreConnectActivity::startWebServer() {
  state = CalibreConnectState::SERVER_STARTING;
  requestUpdate();

  MDNS.end();
  if (MDNS.begin(HOSTNAME)) {
    // mDNS is optional for the Calibre plugin but still helpful for users.
    LOG_DBG("CAL", "mDNS started: http://%s.local/", HOSTNAME);
  }

  webServer.reset(new FluiDezWebServer());
  webServer->begin();

  if (webServer->isRunning()) {
    state = CalibreConnectState::SERVER_RUNNING;
    requestUpdate();
  } else {
    state = CalibreConnectState::ERROR;
    requestUpdate();
  }
}

void CalibreConnectActivity::stopWebServer() {
  if (webServer) {
    webServer->stop();
    webServer.reset();
  }
}

void CalibreConnectActivity::loop() {
  if (mappedInput.wasPressed(MappedInputManager::Button::Back)) {
    exitRequested = true;
  }

  if (webServer && webServer->isRunning()) {
    const unsigned long timeSinceLastHandleClient = millis() - lastHandleClientTime;
    if (lastHandleClientTime > 0 && timeSinceLastHandleClient > 100) {
      LOG_DBG("CAL", "WARNING: %lu ms gap since last handleClient", timeSinceLastHandleClient);
    }

    esp_task_wdt_reset();
    constexpr int MAX_ITERATIONS = 80;
    for (int i = 0; i < MAX_ITERATIONS && webServer->isRunning(); i++) {
      webServer->handleClient();
      if ((i & 0x07) == 0x07) {
        esp_task_wdt_reset();
      }
      if ((i & 0x0F) == 0x0F) {
        yield();
        if (mappedInput.wasPressed(MappedInputManager::Button::Back)) {
          exitRequested = true;
          break;
        }
      }
    }
    lastHandleClientTime = millis();

    updateTransferStatus();
  }

  if (exitRequested) {
    finish();
    return;
  }
}

void CalibreConnectActivity::updateTransferStatus() {
  const auto status = webServer->getWsUploadStatus();
  const unsigned long now = millis();

  const bool receiving = status.inProgress && status.total > 0 && status.received <= status.total;
  const bool receivedRecently = status.lastCompleteAt != 0 && now - status.lastCompleteAt < RESULT_VISIBLE_MS;
  const bool failedRecently = status.lastFailedAt != 0 && now - status.lastFailedAt < RESULT_VISIBLE_MS;
  TransferResult nextResult = TransferResult::None;
  std::string_view nextResultName;
  if (failedRecently && (!receivedRecently || status.lastFailedAt >= status.lastCompleteAt)) {
    nextResult = TransferResult::Failed;
    nextResultName = status.lastFailedName;
  } else if (receivedRecently) {
    nextResult = TransferResult::Received;
    nextResultName = status.lastCompleteName;
  }

  const std::string& clientIP = webServer->getLastClientIp();
  const std::string_view uploadName = receiving ? std::string_view(status.filename) : std::string_view();
  const size_t received = receiving ? status.received : 0;
  const size_t total = receiving ? status.total : 0;

  const bool textChanged =
      clientIP != calibreIP || uploadName != currentUploadName || nextResult != result || nextResultName != resultName;
  const bool countersChanged =
      received != lastProgressReceived || total != lastProgressTotal || status.completedCount != receivedCount;
  if (!textChanged && !countersChanged) {
    return;
  }

  if (textChanged) {
    // Strings can reallocate while the render task reads them.
    RenderLock lock(*this);
    calibreIP = clientIP;
    currentUploadName = uploadName;
    result = nextResult;
    resultName = nextResultName;
    lastProgressReceived = received;
    lastProgressTotal = total;
    receivedCount = status.completedCount;
  } else {
    lastProgressReceived = received;
    lastProgressTotal = total;
    receivedCount = status.completedCount;
  }
  requestUpdate();
}

void CalibreConnectActivity::render(RenderLock&&) {
  const auto& metrics = UITheme::getInstance().getMetrics();
  const auto pageWidth = renderer.getScreenWidth();
  const auto pageHeight = renderer.getScreenHeight();

  renderer.clearScreen();

  CompactHeader::drawTitle(renderer, tr(STR_CALIBRE_WIRELESS));
  const auto height = renderer.getLineHeight(UI_10_FONT_ID);
  const auto top = (pageHeight - height) / 2;

  if (state == CalibreConnectState::SERVER_STARTING) {
    renderer.drawCenteredText(UI_12_FONT_ID, top, tr(STR_CALIBRE_STARTING));
  } else if (state == CalibreConnectState::ERROR) {
    renderer.drawCenteredText(UI_12_FONT_ID, top, tr(STR_CONNECTION_FAILED), true, EpdFontFamily::BOLD);
  } else if (state == CalibreConnectState::SERVER_RUNNING) {
    const int subHeaderTop = CompactHeader::contentTop(metrics);
    GUI.drawSubHeader(renderer, Rect{0, subHeaderTop, pageWidth, metrics.tabBarHeight}, connectedSSID.c_str());

    // Keep the network name and full address independently readable on narrow
    // screens. Sharing one subheader row forces one of them to be truncated.
    const std::string ipLabel = std::string(tr(STR_IP_ADDRESS_PREFIX)) + connectedIP;
    const int ipTop = subHeaderTop + metrics.tabBarHeight + metrics.verticalSpacing;
    renderer.drawText(SMALL_FONT_ID, metrics.contentSidePadding, ipTop, ipLabel.c_str());

    int y = ipTop + height + metrics.verticalSpacing * 3;
    const auto heightText12 = renderer.getTextHeight(UI_12_FONT_ID);
    renderer.drawText(UI_12_FONT_ID, metrics.contentSidePadding, y, tr(STR_CALIBRE_SETUP), true, EpdFontFamily::BOLD);
    y += heightText12 + metrics.verticalSpacing * 2;

    renderer.drawText(SMALL_FONT_ID, metrics.contentSidePadding, y, tr(STR_CALIBRE_INSTRUCTION_1));
    renderer.drawText(SMALL_FONT_ID, metrics.contentSidePadding, y + height, tr(STR_CALIBRE_INSTRUCTION_2));
    renderer.drawText(SMALL_FONT_ID, metrics.contentSidePadding, y + height * 2, tr(STR_CALIBRE_INSTRUCTION_3));
    renderer.drawText(SMALL_FONT_ID, metrics.contentSidePadding, y + height * 3, tr(STR_CALIBRE_INSTRUCTION_4));

    y += height * 3 + metrics.verticalSpacing * 4;
    renderer.drawText(UI_12_FONT_ID, metrics.contentSidePadding, y, tr(STR_CALIBRE_STATUS), true, EpdFontFamily::BOLD);
    y += heightText12 + metrics.verticalSpacing * 2;

    // Always show whether Calibre has found the reader, even when no book is
    // arriving, so the section never looks empty.
    const int textWidth = pageWidth - metrics.contentSidePadding * 2;
    const std::string connection = calibreIP.empty() ? std::string(tr(STR_CALIBRE_WAITING))
                                                     : std::string(tr(STR_CALIBRE_CONNECTED)) + " (" + calibreIP + ")";
    const std::string connectionLine = renderer.truncatedText(UI_10_FONT_ID, connection.c_str(), textWidth);
    renderer.drawText(UI_10_FONT_ID, metrics.contentSidePadding, y, connectionLine.c_str());
    y += height + metrics.verticalSpacing;

    if (lastProgressTotal > 0) {
      const std::string label = std::string(tr(STR_CALIBRE_RECEIVING)) + currentUploadName;
      const std::string labelLine = renderer.truncatedText(SMALL_FONT_ID, label.c_str(), textWidth);
      renderer.drawText(SMALL_FONT_ID, metrics.contentSidePadding, y, labelLine.c_str());
      GUI.drawProgressBar(
          renderer,
          Rect{metrics.contentSidePadding, y + height + metrics.verticalSpacing, textWidth, metrics.progressBarHeight},
          lastProgressReceived, lastProgressTotal);
    } else if (result != TransferResult::None) {
      const char* prefix = result == TransferResult::Failed ? tr(STR_CALIBRE_RECEIVE_FAILED) : tr(STR_CALIBRE_RECEIVED);
      const std::string message = std::string(prefix) + resultName;
      const std::string messageLine = renderer.truncatedText(SMALL_FONT_ID, message.c_str(), textWidth);
      renderer.drawText(SMALL_FONT_ID, metrics.contentSidePadding, y, messageLine.c_str());
    } else if (receivedCount > 0) {
      const std::string message = std::string(tr(STR_CALIBRE_BOOKS_RECEIVED)) + std::to_string(receivedCount);
      renderer.drawText(SMALL_FONT_ID, metrics.contentSidePadding, y, message.c_str());
    }

    const auto labels = mappedInput.mapLabels(mappedInput.withBackArrow(tr(STR_EXIT)), "", "", "");
    GUI.drawButtonHints(renderer, labels.btn1, labels.btn2, labels.btn3, labels.btn4);
  }
  renderer.displayBuffer(screenTransitionRefresh.modeFor(static_cast<uint8_t>(state)));
}
