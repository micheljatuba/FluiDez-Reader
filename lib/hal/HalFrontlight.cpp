#include "HalFrontlight.h"

#include <Arduino.h>
#include <BoardConfig.h>
#include <Logging.h>
#include <driver/gpio.h>

HalFrontlight HalFrontlight::instance;

namespace {
// PWM frontlight pads on this board, or PIN_UNASSIGNED. PMIC- and I2C-driven
// frontlights have no ESP pad to park.
int8_t pwmPad(const bool warm) {
  const auto& fl = BoardConfig::ACTIVE.frontlight;
  if (fl.viaPm1Pwm) return BoardConfig::PIN_UNASSIGNED;
  return warm ? fl.gpioWarm : fl.gpio;
}
}  // namespace

void HalFrontlight::begin(const uint8_t brightness, const uint8_t warmth, const bool on) {
  if (!manager.present()) {
    return;
  }
  // parkForDeepSleep() latches a pad hold that survives deep sleep and the wake
  // reset; a held pad silently ignores the LEDC drive, so always release first.
  for (const bool warm : {false, true}) {
    const int8_t pin = pwmPad(warm);
    if (pin >= 0) gpio_hold_dis(static_cast<gpio_num_t>(pin));
  }
  manager.begin();
  lastBrightness = brightness > 100 ? 100 : brightness;
  manager.setColorTemperature(warmth > 100 ? 100 : warmth);
  lit = on;
  manager.setBrightness(lit ? lastBrightness : 0);
  LOG_INF("LIGHT", "Frontlight up: %u%% warm=%u%% %s", lastBrightness, manager.colorTemperature(), lit ? "on" : "off");
}

void HalFrontlight::setBrightness(const uint8_t percent) {
  lastBrightness = percent > 100 ? 100 : percent;
  if (lit) {
    manager.setBrightness(lastBrightness);
  }
}

void HalFrontlight::setWarmth(const uint8_t warmPercent) {
  manager.setColorTemperature(warmPercent > 100 ? 100 : warmPercent);
}

void HalFrontlight::setOn(const bool on) {
  if (on == lit) {
    return;
  }
  lit = on;
  manager.setBrightness(lit ? lastBrightness : 0);
}

void HalFrontlight::parkForDeepSleep() {
  if (!manager.present()) {
    return;
  }
  manager.setBrightness(0);
  const bool offLevel = !BoardConfig::ACTIVE.frontlight.activeHigh;
  for (const bool warm : {false, true}) {
    const int8_t pin = pwmPad(warm);
    if (pin < 0) continue;
    const auto pad = static_cast<gpio_num_t>(pin);
    ledcDetach(pin);
    gpio_hold_dis(pad);
    pinMode(pin, OUTPUT);
    digitalWrite(pin, offLevel ? HIGH : LOW);
    gpio_hold_en(pad);
  }
  LOG_INF("LIGHT", "Frontlight parked for deep sleep");
}
