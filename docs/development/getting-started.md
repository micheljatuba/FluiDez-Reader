---
title: Getting Started
parent: Development
nav_order: 1
---

# Getting Started

This guide helps you build and run FluiDez Reader locally.

## Prerequisites

- PlatformIO Core (`pio`) or VS Code + PlatformIO IDE
- Python 3.8+
- `clang-format` 21+ in your `PATH` (CI uses clang-format 21)
- USB-C cable
- A supported reader for hardware testing (FluiDez Reader is tested only on the Xteink X4 Pro)

If `./bin/clang-format-fix` fails with either of these errors, install clang-format 21:

- `clang-format: No such file or directory`
- `.clang-format: error: unknown key 'AlignFunctionDeclarations'`

Examples:

```sh
# Debian/Ubuntu (try this first)
sudo apt-get update && sudo apt-get install -y clang-format-21

# If the package is unavailable, add LLVM apt repo and retry
wget https://apt.llvm.org/llvm.sh
chmod +x llvm.sh
sudo ./llvm.sh 21
sudo apt-get update
sudo apt-get install -y clang-format-21

# macOS (Homebrew)
brew install clang-format
```

Then verify:

```sh
clang-format-21 --version
```

The reported major version must be 21 or newer.

On Linux, Nix users can enter the development shell with `nix develop -f nix` or `nix-shell nix`. It provides PlatformIO Core (installed into `.venv` on first use), `clang-format`, and the Python packages for the serial monitor.

## Clone and initialize

```sh
git clone --recursive https://github.com/micheljatuba/FluiDez-Reader
cd FluiDez-Reader
```

If you already cloned without submodules:

```sh
git submodule update --init --recursive
```

## Build

```sh
pio run -e x4-pro             # X4 Pro firmware
pio run -e x4-pro-simulator   # X4 Pro simulator
```

Each reader has a firmware environment and a simulator environment:

| Reader | Firmware | Simulator |
| --- | --- | --- |
| Xteink X4 Pro (tested) | `x4-pro` | `x4-pro-simulator` |
| Xteink X4 Classic | `x4-classic` | `x4-classic-simulator` |
| Xteink X3 / X4 | `default` | `simulator` (X4) or `simulator-X3` |
| Seeed Studio Sticky | `sticky` | `sticky-simulator` |

`pio run` without an environment builds `default`, `sticky`, and `x4-pro` (the `default_envs` in `platformio.ini`). To run a simulator, see [Simulator](../simulator.md).

## Flash

Connect the reader with a USB-C data cable and upload the firmware environment for your reader, for example:

```sh
pio run -e x4-pro --target upload
```

## Validation

```sh
./bin/clang-format-fix
pio check --fail-on-defect low --fail-on-defect medium --fail-on-defect high
pio run
```

## What to read next

- [Architecture Overview](./architecture.md)
- [Testing and Debugging](./testing-debugging.md)
- [Publicar uma versão](./releasing.md) (publishing a release, in Portuguese)
