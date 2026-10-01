#pragma once

#include <cstddef>
#include <cstdint>
#include <functional>

class GfxRenderer;
class Page;

namespace EpubGrayscale {
constexpr int GRAYSCALE_STRIP_ROWS = 80;

// Preserves the live BW buffer and existing controller synchronization. False
// leaves the caller responsible for its existing BW-snapshot fallback.
//
// skipRequested is polled only while asyncRefreshPending, at points before the
// gray waveform starts. When it returns true the overlay is dropped: the B/W
// page from the base refresh stays on the panel, the controller baseline is
// re-synced from the live framebuffer, and the function returns true.
bool runTiledGrayscalePass(GfxRenderer& renderer, const Page& page, int fontId, int marginLeft, int marginTop,
                           bool foregroundBlack, bool needsTextGrayscale, bool needsImageGrayscale, uint8_t* scratch,
                           size_t scratchSize, bool asyncRefreshPending,
                           const std::function<bool()>& skipRequested = {});
}  // namespace EpubGrayscale
