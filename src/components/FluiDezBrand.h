#pragma once

class GfxRenderer;

// FluiDez Reader brand lockup (symbol above the wordmark) for full-screen
// activities such as the boot and default sleep screens.
namespace FluiDezBrand {

int lockupHeight();

// Draws the lockup horizontally centred with its top at `top` and returns the
// y coordinate just below the wordmark. Only black pixels are drawn.
int drawLockup(const GfxRenderer& renderer, int top);

// Top that centres the lockup plus `belowHeight` px of text under it,
// lifted slightly above the geometric centre.
int centredLockupTop(const GfxRenderer& renderer, int belowHeight);

}  // namespace FluiDezBrand
