# Changelog

All notable changes to FluiDez Reader are documented in this file.

FluiDez Reader is a maintained fork of [CrossInk](https://github.com/uxjulia/crossink), which is based on [CrossPoint Reader](https://github.com/crosspoint-reader/crosspoint-reader). Changes inherited from CrossInk are listed in the [CrossInk changelog](https://github.com/uxjulia/crossink/blob/main/CHANGELOG.md).

## [Unreleased]

### Added

- `NOVIDADES.md` describes what each version changes for readers, in Brazilian Portuguese. GitHub release pages open with the version's section, and CI fails when the version in `platformio.ini` has none.

### Removed

- Repository files FluiDez Reader does not use: the CrossInk logo images (`src/images/crossink.png`, `crossink-white.png` and `Logo120.h`), the leftover GitHub Agentic Workflows files (`.github/aw/` and `.github/skills/`) and the issue templates, since Issues and pull requests are turned off.
- The AI assistant instructions (`AGENTS.md`, `CLAUDE.md` and `.claude/`) are no longer published. They stay on the maintainer's computer.
- Camera and location metadata from the focus reading photos in the documentation.

## [v1.6-fluidez10] - 2026-09-30

### Added

- Brazilian Portuguese translations for the 132 interface strings that still appeared in English, including Nearby File Transfer, dictionary lookup, touch gestures, frontlight controls, KOReader account sign-up, and the date settings.

### Changed

- Month names in the header date, reading statistics, and the Dashboard follow the interface language, for example "30 set 2026" in Brazilian Portuguese. Short statistics dates put the day first when the chosen date format does, for example "29 Sep" instead of "Sep 29".

## [v1.6-fluidez9] - 2026-09-29

### Added

- Before installing an update, the update screen states that updates are installed at your own risk.

### Fixed

- Check for Updates and KOReader authentication no longer crash and restart the X4 Pro while connecting to Wi-Fi. On the ESP32-S3 readers (X4 Pro, X4 Classic, and Sticky), these screens now get the reader-sized render stack, as in CrossInk ([uxjulia/crossink#762](https://github.com/uxjulia/crossink/issues/762)). Because Check for Updates in `1.6-fluidez8` can crash on these readers, install this version from the SD card.
- The update-complete screen wraps the power-on instructions instead of letting them run off the screen, and the Brazilian Portuguese text reads "Pressione e segure o botão liga/desliga para ligar novamente".

## [v1.6-fluidez8] - 2026-09-29

Based on CrossInk development after v1.5.1 (commit [`b0eb0aa6`](https://github.com/uxjulia/crossink/commit/b0eb0aa699a6e8d84828bb5a107737e1c30fb80b)). The inherited changes are listed in the [CrossInk changelog at that commit](https://github.com/uxjulia/crossink/blob/b0eb0aa699a6e8d84828bb5a107737e1c30fb80b/CHANGELOG.md).

### Known issues

- Check for Updates crashes and restarts the X4 Pro while connecting to Wi-Fi, so readers on this version cannot update over the air. KOReader authentication can crash the same way, and the X4 Classic and Sticky, which use the same ESP32-S3 processor, may be affected too. Fixed in v1.6-fluidez9: install it or a newer release from the SD card (`Settings > System > SD Card Firmware Update`).

### Added

- Five more SD-card font families: Gelasio, EB Garamond, Crimson Pro, Jost, and Arimo. They are built locally with `lib/EpdFont/scripts/build-sd-fonts.py`; see [SD card fonts](docs/sd-card-fonts.md).
- Recent Books can pin up to six books from the long-press menu. Pinned books stay at the top of Recent Books in the order they were pinned, follow Continue Reading on the Home screen, and are kept when older or finished books leave the list.
- Lyra Grid UI theme: Lyra Carousel's icon menu with a 3x2 Home grid of six books (current, pinned, then recent), each with its reading-progress bar and a ribbon on pinned books.
- Holding a Home book cover (or holding Confirm on the selected book in multi-cover themes without touch) opens Pin to Top/Unpin, Mark Finished, and Remove from Recent Books without leaving Home.
- FluiDez Reader identity: a new symbol and Lexend Deca wordmark on the boot and default sleep screens, the Settings version footer, the default device name, and the web portal (logo, page titles, footer, and accent colour). Brand assets and the generator live in `docs/brand/` and `scripts/generate_brand_assets.py`.
- Pushing a `v<version>` tag builds the X3/X4, X4 Pro, X4 Classic, and Sticky firmware and publishes it as a GitHub release, which Check for Updates then offers.
- CI now builds the X4 Pro simulator and runs isolated headless smoke tests with the default, Classic, and Dashboard themes, retaining failure logs.

### Changed

- Check for Updates follows FluiDez Reader releases (`micheljatuba/FluiDez-Reader`) and offers a release when its `fluidez` build number is newer (for example `1.6-fluidez9` after `1.6-fluidez8`), instead of treating every FluiDez build of the same base version as current. Release firmware includes English and Brazilian Portuguese.
- Touch readers check for input sooner after the screen has been idle, so a tap is no longer held back by the low-power sleep interval.
- Keyboard, Wi-Fi selection, Nearby transfer and sync, and font download result screens follow Time to Sleep when left idle. The reader still stays awake while scanning, connecting, transferring, syncing, or downloading; unconfirmed typed text is discarded if it goes to sleep.
- Cache clearing, stats backup, clock sync, firmware update, and font download screens no longer keep the CPU at full speed while waiting for input.
- Lyra Carousel shows up to five books (two on each side of the selected cover) instead of three.
- Dashboard stats use shorter, larger labels (for example "Leitura", "Restante", "Previsão") that wrap instead of being cut, and the footer gives each item the width it needs.
- Crash reports, the serial boot log, and the USB device name on X4 Pro and X4 Classic identify FluiDez Reader.

### Fixed

- Per-book reading stats survive an SD card failure while replacing the stats file, and Delete Book Stats also removes the unfinished copy.
- Dashboard Home and sleep screens no longer draw long stat labels over the cover or let stats, title, chapter, and footer overlap or leave the screen; the layout is computed from measured text and shrinks or drops the least important parts only when space runs out.
- Portuguese Dashboard screens show "Restante" instead of the English "Time Left", and the Brazilian streak label reads "12 dias seguidos".
- Lyra Carousel Home screens refresh cached books after changing the UI language, UI scale, or front-button mapping, after firmware updates, and when a book's title or author changes.
- The sleep countdown starts when long downloads, cache clearing, and other blocking work finish, so their results stay visible for the full Time to Sleep period.
- Quick Lock and consumed shortcuts now retain the main loop's normal waiting and idle CPU power-saving policy instead of spinning through early returns.
- Web file manager navigation preserves folder names containing percent signs instead of decoding them twice.
- EPUB metadata renaming recognizes EPUB 3 creator role refinements and no longer selects a tagged translator instead of the author.
- Upload results distinguish optimized files, originals sent after optimization failures, and failed transfers, retaining warnings when the file list refreshes.
- Simulator smoke-test timeouts now preserve captured diagnostics instead of dropping the simulator log.
