#include "FluiDezBrand.h"

#include <GfxRenderer.h>

#include <algorithm>

#include "images/FluiDezLogo.h"

namespace FluiDezBrand {
namespace {
constexpr int kSymbolWordmarkGap = 18;
constexpr int kOpticalLift = 20;
}  // namespace

int lockupHeight() { return FluiDezLogo::kSymbolSize + kSymbolWordmarkGap + FluiDezLogo::kWordmarkHeight; }

int drawLockup(const GfxRenderer& renderer, const int top) {
  const int pageWidth = renderer.getScreenWidth();
  renderer.drawIcon(FluiDezLogo::kSymbol, (pageWidth - FluiDezLogo::kSymbolSize) / 2, top, FluiDezLogo::kSymbolSize,
                    FluiDezLogo::kSymbolSize);
  const int wordmarkTop = top + FluiDezLogo::kSymbolSize + kSymbolWordmarkGap;
  renderer.drawIcon(FluiDezLogo::kWordmark, (pageWidth - FluiDezLogo::kWordmarkWidth) / 2, wordmarkTop,
                    FluiDezLogo::kWordmarkWidth, FluiDezLogo::kWordmarkHeight);
  return wordmarkTop + FluiDezLogo::kWordmarkHeight;
}

int centredLockupTop(const GfxRenderer& renderer, const int belowHeight) {
  const int total = lockupHeight() + std::max(0, belowHeight);
  return std::max(0, (renderer.getScreenHeight() - total) / 2 - kOpticalLift);
}

}  // namespace FluiDezBrand
