---
title: Testing & Debugging
parent: Development
nav_order: 4
---

# Testing and Debugging

FluiDez Reader runs on real hardware, so debugging usually combines local build checks, simulator checks, and on-device logs.

## Local checks

Make sure `clang-format` 21+ is installed and available in `PATH` before running the formatting step.
If needed, see [Getting Started](./getting-started.md).

```sh
./bin/clang-format-fix
pio check --fail-on-defect low --fail-on-defect medium --fail-on-defect high
pio run -e simulator
pio run -e default
```

`pio run` without `-e` builds the X3/X4 and Sticky firmware targets from `platformio.ini`. Use it for a comprehensive build check, but prefer explicit environments while iterating.

## Main-loop pacing regression tests

```sh
cmake -S test -B build/test
cmake --build build/test --target MainLoopPacingDeviceTest MainLoopPacingSimulatorTest ActivityPowerPolicyGuardTest
ctest --test-dir build/test --output-on-failure -R '^((Device|Simulator)\.MainLoopPacingTest|ActivityPowerPolicyGuard)\.'
```

The fixture compiles the production `loop()` body from `src/main.cpp`, refreshed
by CMake whenever that file changes, with hardware and activity boundaries
stubbed. It links the production `MainLoopPacing` implementation. Tests cover
Quick Lock and shortcut early returns, inactivity timestamps, clock wraparound,
render-lock release ordering, fast activity polling, and USB Drive's separate
storage loop. The existing 10 ms active delay, 20/50 ms device idle budgets, and
50 ms simulator idle delay are preserved.

The idle countdown starts when a frame that began with user input or busy work
finishes, including work that blocks the loop for minutes, or when the activity
becomes busy during that frame. Slow frames that began idle do not postpone
sleep. `ActivityPowerPolicyGuard` scans `src/activities` and rejects
unconditional `preventAutoSleep()` or `skipLoopDelay()` overrides; tie them to
bounded scanning, connection, transfer, synchronization, or download states.

These tests use a controlled clock. They verify scheduling decisions and calls
to power management, not measured ESP32 CPU usage or battery-life gains.

On the X4 Pro, compare against the previous firmware with the same book and
settings:

- Enable Quick Lock, leave Wi-Fi off, and wait at least five seconds. With debug
  logging enabled and no active render, the normal `PWR` low-power transition
  should still occur. Unlock with the configured gesture and check responsiveness.
- Hold and release a screenshot or other button chord; check that it fires once,
  releases cleanly, and does not trigger an extra page turn.
- Check rapid page turns, Home single/double taps, Quick Lock's sleep timeout,
  and sleep/wake. No book cache reset is needed for this change.
- Exercise File Transfer and USB Drive to check that their active transfer loops
  remain responsive. Quantify battery effects only with an on-device comparison.
- Set Time to Sleep to 1 minute. Leave a keyboard, the Wi-Fi network list, a
  Nearby ready or result screen, and a font download result idle; each should
  sleep after about one minute. Typing, scanning, connecting, transfers, and
  syncs should not be interrupted.
- Download a font family that takes longer than the timeout; its result should
  stay visible for a full minute after the download ends.

## Web portal regression tests

With Node.js 22 or later, run the browser-side tests without installing npm
packages or building the firmware:

```sh
node --test test/web_file_manager/browser_test.js test/pxc_v2/browser_test.js
```

These tests execute the production JavaScript with isolated browser, ZIP, and
network stubs. They cover percent signs and Unicode in folder paths, EPUB 2/3
author roles, optimization outcomes, WebSocket-to-HTTP fallback, interrupted
batches, cancellation, filename collisions, and image-encoder golden files.
They run in CI's host unit-test job. Native DOM parsing and the visible upload
flow should also be checked in a browser against **File Transfer** on the device.

## X4 Pro headless simulator smoke test

CI's `x4-pro-simulator-smoke` job builds `x4-pro-simulator` once on Ubuntu 24.04,
then runs the existing smoke runner with the default, Classic, and Dashboard
themes. It is required by the aggregate **Test Status** job, alongside the
firmware builds, formatting, static analysis, and host unit tests.

The simulator supports Linux/WSL and macOS, not native Windows. On Debian/Ubuntu,
install the native build dependencies and use the Linux flags from the
simulator's sample configuration:

```sh
sudo apt-get update
sudo apt-get install -y build-essential pkg-config libsdl2-dev libssl-dev curl
# Install PlatformIO in an active Python virtual environment (CI uses Python 3.14).
python -m pip install https://github.com/pioarduino/platformio-core/archive/refs/tags/v6.1.19.zip
export PLATFORMIO_BUILD_FLAGS="-Wno-narrowing -lssl -lcrypto"
export SDL_VIDEODRIVER=dummy
pio run -j2 -e x4-pro-simulator
python scripts/run_simulator_smoke_test.py --env x4-pro-simulator --no-build --timeout 120
python scripts/run_simulator_smoke_test.py --env x4-pro-simulator --no-build --timeout 120 --theme classic
python scripts/run_simulator_smoke_test.py --env x4-pro-simulator --no-build --timeout 120 --theme dashboard
```

`PLATFORMIO_BUILD_FLAGS` adds GCC's equivalent of the configured Clang narrowing
flag and the OpenSSL link libraries; it does not replace the device flags in
`platformio.ini`. Keep these overrides local to simulator builds, not firmware
builds. SDL's dummy video driver avoids a display server or Xvfb.

Each run copies the tracked `test/epubs/test_reader_rendering_matrix.epub` into
a fresh temporary `fs_` directory, so existing books, settings, and caches are
not touched. The runner exercises the app's navigation, settings, EPUB reader,
and X4 Pro input checks. It requires a zero exit code and the
`Simulator smoke test passed` marker, and rejects known crash markers.
Each run has a 120-second timeout in CI (the runner's standalone default remains
45 seconds). A timeout exits with code 124 and prints the captured simulator
output before cleaning up the temporary filesystem.

CI retains combined stdout/stderr from the runner tests, build, and smoke runs
in the `x4-pro-simulator-smoke-logs` artifact for seven days when the job fails.
The build step is limited to 15 minutes, each smoke step to three minutes, and
the whole job to 25 minutes. No failure is ignored. The runner's regression
tests can also be run separately without PlatformIO or SDL:

```sh
python -m unittest discover -s test/scripts -p test_run_simulator_smoke_test.py -v
```

For Windows validation in Docker, mount the checkout read-only and copy the
sources into the disposable container before building. Exclude host build
outputs (`.pio/build`, `build`), personal `platformio.local.ini` overrides, and
the unused top-level `assets/tabler-icons` tree; keep `freeink-sdk/libs/assets`
because the simulator needs its icon library. Install tools only inside that
container, not a shared WSL installation. These smoke checks do not flash
hardware or prove physical touch, frontlight, storage, or display behavior.

## Flash and monitor

Flash firmware:

```sh
pio run -e default --target upload
```

Open serial monitor:

```sh
pio device monitor
```

Optional enhanced monitor:

```sh
python3 -m pip install pyserial colorama matplotlib
python3 scripts/debugging_monitor.py
```

## Useful bug report contents

- Firmware version and build environment
- Exact steps to reproduce
- Expected vs actual behavior
- Serial logs from boot through failure
- Whether issue reproduces after clearing the affected book cache or using **Clear Reading Cache**

## Common troubleshooting references

- [Common Issues](../troubleshooting.md)
