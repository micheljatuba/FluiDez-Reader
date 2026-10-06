#include "MainLoopPacing.h"

#include <Arduino.h>
#include <HalGPIO.h>
#include <HalPowerManager.h>

MainLoopPacing::~MainLoopPacing() {
  if (renderBusy) {
    delay(inputPollDelayMs);
    return;
  }

  if (skipDelay) {
    powerManager.setPowerSaving(false);
    yield();
    return;
  }

  if (millis() - lastActivityTime < HalPowerManager::IDLE_POWER_SAVING_MS) {
    delay(inputPollDelayMs);
    return;
  }

  powerManager.setPowerSaving(true);
#ifndef SIMULATOR
  // Button contacts can interrupt the wait between debounce polls. Touch needs
  // the next gpio.update(), so touch boards retain their shorter idle budget.
  const unsigned long idleBudgetMs = gpio.hasTouch() ? 20 : 50;
  const unsigned long idleStart = millis();
  while (millis() - idleStart < idleBudgetMs) {
    delay(10);
    if (gpio.rawInputActive()) break;
  }
#else
  delay(50);
#endif
}
