# Changelog

All notable changes to FluiDez Reader are documented in this file.

FluiDez Reader is a maintained fork of [CrossInk](https://github.com/uxjulia/crossink), which is based on [CrossPoint Reader](https://github.com/crosspoint-reader/crosspoint-reader). Changes inherited from CrossInk are listed in the [CrossInk changelog](https://github.com/uxjulia/crossink/blob/main/CHANGELOG.md).

## [Unreleased]

## [v1.6-fluidez14] - 2026-10-06

### Added

- **FluiDez themes:** three native Home layouts with FluiDez's own look, chosen in **UI Theme**. **FluiDez Shelf** (the new default) shows the current book's cover with a progress ring and a shelf of text spines for recent books. **FluiDez Cards** shows a card with the current book, cards for reading time, streak and finished books, and recent covers. **FluiDez Flow** is text-only and reads no cover from the SD card, which makes it the fastest Home. All three share a bottom row of icon buttons whose focused item's full label is shown above it, so long translations are never cut. All three paint progress and stats from data loaded once when Home opens, and generate cover thumbnails only for the covers they show.
- **Sleep Screen page in the web interface** (`/sleep`): pick any picture, frame it, adjust brightness and contrast with a 4-gray e-ink preview, and save. The browser converts it to an 8-bit grayscale BMP at the panel's exact size, uploads it to `/sleep`, pins it and switches the sleep screen to Custom. The page also lists, pins and deletes the images in `/sleep`. New endpoints: `GET`/`POST /api/sleep-image`.
- The Custom sleep screen accepts JPG and PNG photos in the sleep folder or pinned from the web page. Each photo is converted once to a screen-sized BMP under `/.crosspoint/sleep-converted/` and streamed from there afterwards.
- The simulator smoke runner can capture each FluiDez Home layout (`--fluidez-captures <dir>`).

### Changed

- FluiDez Reader is synced with CrossInk `development` at [`6fbc97d1`](https://github.com/uxjulia/crossink/commit/6fbc97d1) (2026-10-01, after CrossInk v1.6.1). This brings in, among others, the Library screen that replaces Recent Books, the five-tab EPUB reader menu on every device, separate top and bottom reader status bars, separate short- and long-press actions for each side button, device-wide and per-book reading-stats tracking, edge gestures on touch readers, the Cover Grid Home theme on PSRAM readers, TTF fonts on ESP32-S3 readers, and faster SD-card reads. The [CrossInk changelog](https://github.com/uxjulia/crossink/blob/main/CHANGELOG.md) lists every change.
- The FluiDez Reader repository history now continues from CrossInk's, so later CrossInk updates arrive as ordinary merges.
- Lyra Carousel now uses CrossInk's Home cache, which stores only cover artwork and draws progress, reading time, the header and menu live. Reading a book no longer invalidates any cached position. It replaces FluiDez Reader's earlier per-book frame cache, which is removed after the first write.
- **Pin to Top** and **Unpin** are available from the Library book actions, and pinned books come first in Library's **Recently Opened** order, as they did in Recent Books.
- The Dashboard Home and sleep screens follow the new reading-stats switches: with tracking off for the device or the book, only progress and Time Left remain, and the all-time footer is hidden.
- Long date formats in the header and in Library date groups use the translated month names.

### Fixed

- Windows builds no longer stop with "Two environments with different actions" once pioarduino shortens long include paths: the build-identity step now only touches `src/util/BuildInfo.cpp` and hands its version defines to the platform's single compile action.
- The SdFat and JPEGDEC patches stay LF on Windows checkouts, so `git apply` accepts them.
- The X4 Pro simulator smoke test drags the frontlight drawer's handle where the theme draws it, so the Classic theme passes as well.
- Two loops flagged by cppcheck in the bookmark and clipping stores use `std::any_of`, keeping the static analysis job clean.

### Notes

- New installs start with **FluiDez Shelf**. Existing installs keep their saved theme. The FluiDez themes use **UI Theme** values 9 (Flow), 10 (Cards) and 11 (Shelf).
- The web interface footer no longer names CrossInk; the credits remain in this changelog and the README.
- The **UI Theme** setting keeps value 7 for Lyra Grid. CrossInk's Cover Grid uses value 8 in FluiDez Reader, so existing installs keep their theme after updating.

## [v1.6-fluidez13] - 2026-10-01

### Changed

- Home navigation and book opening call `ActivityManager` directly, completing the transition away from the temporary `Activity::onGoHome` and `Activity::onSelectBook` wrappers.
- The default theme's recent-book cover rendering is split into focused layout and drawing helpers without changing its appearance or interaction.
- The Calibre plugin, now version 1.1.0, checks every 5 seconds that the connected reader still answers. When the reader leaves Calibre Wireless, Calibre shows it as disconnected after about 15 seconds and reconnects on its own when the screen is reopened. Reopening the screen while Calibre is still connected shows Calibre as connected within a few seconds instead of waiting for the next book.
- Text drawing resolves clipping and screen rotation once per glyph, reducing work when painting menus and book pages. From CrossInk [`8940d64e`](https://github.com/uxjulia/crossink/commit/8940d64eee29718f2c0abc09bc191a334a6c93d9).
- With text anti-aliasing on, a page turn pressed before the current page's text is lightened skips that page's anti-aliasing and shows the next page at once. The page you stop on is still anti-aliased.

### Fixed

- The Status section of the Calibre Wireless screen no longer stays blank. It shows whether Calibre has found the reader, with the computer's IP address, then the book being received with its progress, the last book received or a failed transfer, and how many books arrived while the screen was open.
- The Calibre Wireless receiving line no longer repeats the colon ("Receiving: : book").
- An EPUB chapter load that unexpectedly ends without a chapter shows the indexing error instead of leaving the screen unchanged.

## [v1.6-fluidez12] - 2026-09-30

### Changed

- Font downloads, OPDS catalogs and downloads, and Check for Updates identify themselves with the User-Agent `FluiDez-Reader-ESP32-<version>`. KOReader sync requests are unchanged.
- The installation guide and the user guide are in Brazilian Portuguese and use the reader's Portuguese menu names. The Calibre plugin guide describes only the FluiDez Reader plugin.
- CI and release jobs run on `ubuntu-24.04`, where the builds are validated, instead of `ubuntu-latest`, and use `actions/cache@v6`. The simulator used by the CI smoke tests is pinned to a fixed commit.

### Fixed

- KOReader authentication rejects server responses larger than 4 KB, such as a proxy's HTML error page, instead of running out of memory while the secure connection is open. An incomplete response is reported as a network error. From CrossInk [`56a29f61`](https://github.com/uxjulia/crossink/commit/56a29f61885e96658dd93cc867d6ac9af061e8bb).
- Leaving an EPUB or TXT book releases every rebuildable font cache, including those of fonts used earlier in the session and the built-in fonts' decompression cache, so other screens such as the Home covers get that memory back. From CrossInk [`bc1ef410`](https://github.com/uxjulia/crossink/commit/bc1ef410cfeaded280746d5a796906a6e5d7380e).
- Crash reports from the ESP32-S3 readers (X4 Pro, X4 Classic, and Sticky) keep both cores' registers, backtraces, and running task names, and mark the core that panicked first. Reports from every reader include the firmware ELF SHA256 needed to decode them. From CrossInk [`2dd96fa2`](https://github.com/uxjulia/crossink/commit/2dd96fa22b89c71a9f1f521abe9712c543d00828).

## [v1.6-fluidez11] - 2026-09-30

### Added

- `NOVIDADES.md` describes what each version changes for readers, in Brazilian Portuguese. GitHub release pages open with the version's section, and CI fails when the version in `platformio.ini` has none.
- FluiDez Reader Calibre plugin in `calibre-plugin/`, forked from the [CrossPoint Reader plugin](https://github.com/crosspoint-reader/calibre-plugins). Releases attach it as `fluidez-reader-calibre-plugin.zip`, built by `scripts/build_calibre_plugin.py`, and CI checks that it builds. Besides the automatic network search, it tries `fluidez.local` and the hotspot address `192.168.4.1`, and it also finds readers running earlier firmware. Unlike the CrossPoint Reader plugin, its search on Windows keeps listening when a probe to a closed port is answered with ICMP "port unreachable", which Windows reports as a connection reset on the next receive.

### Changed

- The reader answers at `fluidez.local` instead of `crosspoint.local`, its hotspot is named `FluiDez-Reader` instead of `CrossPoint-Reader`, and it joins Wi-Fi networks as `FluiDez-Reader-<MAC>`. The Calibre discovery reply still starts with `crosspoint`, so the CrossPoint Reader plugin can still find it.
- The Calibre Wireless screen asks for the FluiDez Reader plugin instead of the CrossPoint Reader plugin.

### Removed

- The `v1.6-fluidez8` release, whose Check for Updates crashed the X4 Pro. Its changes are listed under `v1.6-fluidez9`, now the first published FluiDez Reader release.
- Repository files FluiDez Reader does not use: the CrossInk logo images (`src/images/crossink.png`, `crossink-white.png` and `Logo120.h`), the leftover GitHub Agentic Workflows files (`.github/aw/` and `.github/skills/`) and the issue templates, since Issues and pull requests are turned off.
- The AI assistant instructions (`AGENTS.md`, `CLAUDE.md` and `.claude/`) are no longer published. They stay on the maintainer's computer.
- Camera and location metadata from the focus reading photos in the documentation.

## [v1.6-fluidez10] - 2026-09-30

### Added

- Brazilian Portuguese translations for the 132 interface strings that still appeared in English, including Nearby File Transfer, dictionary lookup, touch gestures, frontlight controls, KOReader account sign-up, and the date settings.

### Changed

- Month names in the header date, reading statistics, and the Dashboard follow the interface language, for example "30 set 2026" in Brazilian Portuguese. Short statistics dates put the day first when the chosen date format does, for example "29 Sep" instead of "Sep 29".

## [v1.6-fluidez9] - 2026-09-29

First published FluiDez Reader release. Based on CrossInk development after v1.5.1 (commit [`b0eb0aa6`](https://github.com/uxjulia/crossink/commit/b0eb0aa699a6e8d84828bb5a107737e1c30fb80b)). The inherited changes are listed in the [CrossInk changelog at that commit](https://github.com/uxjulia/crossink/blob/b0eb0aa699a6e8d84828bb5a107737e1c30fb80b/CHANGELOG.md).

### Added

- Five more SD-card font families: Gelasio, EB Garamond, Crimson Pro, Jost, and Arimo. They are built locally with `lib/EpdFont/scripts/build-sd-fonts.py`; see [SD card fonts](docs/sd-card-fonts.md).
- Recent Books can pin up to six books from the long-press menu. Pinned books stay at the top of Recent Books in the order they were pinned, follow Continue Reading on the Home screen, and are kept when older or finished books leave the list.
- Lyra Grid UI theme: Lyra Carousel's icon menu with a 3x2 Home grid of six books (current, pinned, then recent), each with its reading-progress bar and a ribbon on pinned books.
- Holding a Home book cover (or holding Confirm on the selected book in multi-cover themes without touch) opens Pin to Top/Unpin, Mark Finished, and Remove from Recent Books without leaving Home.
- FluiDez Reader identity: a new symbol and Lexend Deca wordmark on the boot and default sleep screens, the Settings version footer, the default device name, and the web portal (logo, page titles, footer, and accent colour). Brand assets and the generator live in `docs/brand/` and `scripts/generate_brand_assets.py`.
- Pushing a `v<version>` tag builds the X3/X4, X4 Pro, X4 Classic, and Sticky firmware and publishes it as a GitHub release, which Check for Updates then offers.
- CI now builds the X4 Pro simulator and runs isolated headless smoke tests with the default, Classic, and Dashboard themes, retaining failure logs.
- Before installing an update, the update screen states that updates are installed at your own risk.

### Changed

- Check for Updates follows FluiDez Reader releases (`micheljatuba/FluiDez-Reader`) and offers a release when its `fluidez` build number is newer (for example `1.6-fluidez10` after `1.6-fluidez9`), instead of treating every FluiDez build of the same base version as current. Release firmware includes English and Brazilian Portuguese.
- Touch readers check for input sooner after the screen has been idle, so a tap is no longer held back by the low-power sleep interval.
- Keyboard, Wi-Fi selection, Nearby transfer and sync, and font download result screens follow Time to Sleep when left idle. The reader still stays awake while scanning, connecting, transferring, syncing, or downloading; unconfirmed typed text is discarded if it goes to sleep.
- Cache clearing, stats backup, clock sync, firmware update, and font download screens no longer keep the CPU at full speed while waiting for input.
- Lyra Carousel shows up to five books (two on each side of the selected cover) instead of three.
- Dashboard stats use shorter, larger labels (for example "Leitura", "Restante", "Previsão") that wrap instead of being cut, and the footer gives each item the width it needs.
- Crash reports, the serial boot log, and the USB device name on X4 Pro and X4 Classic identify FluiDez Reader.

### Fixed

- Check for Updates and KOReader authentication no longer crash and restart the X4 Pro while connecting to Wi-Fi. On the ESP32-S3 readers (X4 Pro, X4 Classic, and Sticky), these screens now get the reader-sized render stack, as in CrossInk ([uxjulia/crossink#762](https://github.com/uxjulia/crossink/issues/762)).
- The update-complete screen wraps the power-on instructions instead of letting them run off the screen, and the Brazilian Portuguese text reads "Pressione e segure o botão liga/desliga para ligar novamente".
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
