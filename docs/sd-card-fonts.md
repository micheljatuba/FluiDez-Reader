---
title: SD Card Fonts
nav_order: 4
---

# SD Card Fonts

FluiDez Reader supports loading additional fonts from the SD card, including fonts
with extended Unicode coverage (CJK, Cyrillic, Greek, etc.). All devices can
use `.cpfont` font packs. ESP32-S3 devices can also use static TrueType (`.ttf`)
fonts directly; ESP32-C3 devices need `.cpfont` files.

## Available Pre-Built Fonts

**Manage Fonts** lists the pre-built families from FluiDez Reader's font
catalog, built from `lib/EpdFont/scripts/sd-fonts.yaml` and hosted in FluiDez's
Azure Blob storage (`http://strfluidez001.blob.core.windows.net/fonts/`). The
manual **Build & Publish SD Card Fonts** workflow
(`.github/workflows/release-fonts.yml`) rebuilds and publishes it. The catalog is
served over plain HTTP on purpose: HTTPS stalls inside `esp_http_client` on
ESP32-C3 readers, and every `.cpfont` is CRC-checked before install. Most of the
same packs are also in the upstream
[crossink-fonts](https://github.com/uxjulia/crossink-fonts/tree/main/cpfonts)
repository and on [Inky](https://inky.crossink.dev/#downloads).

### FluiDez Reader families

FluiDez Reader adds Gelasio, EB Garamond, Crimson Pro, Jost, and Arimo to
`lib/EpdFont/scripts/sd-fonts.yaml`, so they are in the catalog above. To
generate them on a computer instead, copy the family folders to `/.fonts/` or
`/fonts/` on the SD card after running:

    python3 -m pip install -r lib/EpdFont/scripts/requirements.txt
    python3 lib/EpdFont/scripts/build-sd-fonts.py       --only Gelasio,EBGaramond,CrimsonPro,Jost,Arimo       --output-dir ./generated-fonts

## Converting Custom Fonts with CrossPoint's Font Builder

To make `.cpfont` packs from your own TrueType/OpenType fonts, use CrossPoint's
[Font Builder](https://crosspointreader.com/fonts). On ESP32-S3 devices, static
`.ttf` files do not need conversion.

## Installing Fonts

There are three ways to install fonts:

### Option 1: Manual SD card copy (Fastest)

1.  Download pre-built `.cpfont` files from [Inky](https://inky.crossink.dev/#downloads) or the
    [crossink-fonts](https://github.com/uxjulia/crossink-fonts/tree/main/cpfonts) repository,
    or use your own static `.ttf` files on an ESP32-S3 device.
    - If you downloaded a `.zip`, extract it first.
2.  Copy font family folders to one of two locations on your SD card:
    - `/.fonts/` — hidden directory (preferred; keeps the SD root tidy
      when hidden files are not shown on your device)
    - `/fonts/` — visible directory (use this if your OS hides dot-files
      and you'd rather see the folder in your file manager)

    Both roots are always scanned at boot and the results are merged: a
    family installed in `/fonts/` shows up even when `/.fonts/` also
    exists, and vice versa. The two roots only collide if the same family
    name appears in both — in that case the copy in `/.fonts/` wins and
    the duplicate in `/fonts/` is ignored. Put each font family in its own
    folder, including TTF families. Do not mix `.cpfont` and `.ttf` files in
    one family folder.

        SD Card Root/
        ├── .fonts/                     ← Hidden root (preferred)
        │   └── Literata/
        │       ├── Literata_12.cpfont
        │       ├── Literata_14.cpfont
        │       ├── Literata_16.cpfont
        │       └── Literata_18.cpfont
        └── fonts/                      ← Visible root (equally valid)
            └── Merriweather/
                ├── Merriweather_12.cpfont
                └── ...

    On an ESP32-S3 device, a TTF family can look like
    `/fonts/MyFont/MyFont-Regular.ttf` alongside its bold, italic, and bold
    italic files.

3.  Insert the SD card and power on your reader

### Option 2: Download from device

This option downloads pre-built `.cpfont` packs.

1. Connect your reader to Wi-Fi
2. Go to **Settings > Reader > Font Options > Manage Fonts**
3. Browse available font families and select to download
4. Downloaded fonts appear immediately in **Settings > Reader > Font Options > Font Family**

**Note**: To change the font sizes that are downloaded, change the option for `Download Font Size Range` _before_ downloading.

### Option 3: Upload via web browser

1. Start **File Transfer** and connect through **Join Network** or **Create Hotspot**
2. Open the web interface URL shown on the reader
3. Navigate to the **Fonts** tab
4. Select a folder containing one font family, then upload its `.cpfont` files
   or, on an ESP32-S3 device, its static `.ttf` files. The web UI only offers
   TTF uploads on devices that support them.

For the full range of text styles with a TTF family, include regular, bold,
italic, and bold italic files. Variable fonts are not supported. Each `.ttf`
file must be 2 MiB or smaller, and the family must total 6 MiB or less. See
[Scalable TTF Fonts](./scalable-fonts.md) for more about using them.

## Dictionary Fonts

EPUB books can use a different installed SD-card family for dictionary definitions.
This can be set globally or per-book via `Font Options`. If a
saved point size is no longer available, FluiDez Reader chooses the closest file from
the dictionary family. If the device experiences low available RAM, you may see the
dictionary font fall back to your reader font. This is normal.

### Generating dictionary font families

Use the dictionary-specific builder to generate the complete family catalog with
the extra coverage used by dictionary definitions:

    python3 -m pip install -r lib/EpdFont/scripts/requirements.txt
    python3 lib/EpdFont/scripts/build-dictionary-fonts.py \
      --output-dir ./generated-dictionary-fonts \
      --clean \
      --jobs 2

The dictionary build includes the `reading` ranges and the built-in ranges, plus
IPA and phonetic-extension characters (`U+0250–U+02FF` and `U+1D00–U+1DBF`) and
combining-mark ranges (`U+1DC0–U+1DFF`, `U+20D0–U+20FF`, and
`U+FE20–U+FE2F`).

The default output is `../crossink-fonts/dictionary-fonts`. Use a separate
`--output-dir` for personal builds, because `--clean` removes the selected output
directory before generating the fonts. The output contains family folders and ZIP
archives; copy a family folder or unzip its archive into `/.fonts/` or `/fonts/`
on the SD card. Use `--only FamilyA,FamilyB` to generate selected families.

## Converting Custom Fonts with Python

The steps below create `.cpfont` packs. ESP32-S3 devices can use static `.ttf`
files directly instead.

### Prerequisites

    pip install freetype-py fonttools

### Single font (one style)

    python3 lib/EpdFont/scripts/fontconvert_sdcard.py \
      MyFont-Regular.ttf \
      --intervals latin-ext \
      --sizes 12,14,16,18 \
      --style regular \
      --name MyFont \
      --output-dir ./MyFont/

### Multi-style font

    python3 lib/EpdFont/scripts/fontconvert_sdcard.py \
      --regular MyFont-Regular.ttf \
      --bold MyFont-Bold.ttf \
      --italic MyFont-Italic.ttf \
      --bolditalic MyFont-BoldItalic.ttf \
      --intervals latin-ext \
      --sizes 12,14,16,18 \
      --name MyFont \
      --output-dir ./MyFont/

### Available Unicode interval presets

| Preset        | Coverage                                                                                                             |
| ------------- | -------------------------------------------------------------------------------------------------------------------- |
| `ascii`       | U+0020–U+007E (Basic Latin)                                                                                          |
| `latin1`      | U+0080–U+00FF (Latin-1 Supplement)                                                                                   |
| `latin-ext`   | European languages (Latin + Extended-A/B + punctuation + ligatures)                                                  |
| `greek`       | Greek + Extended Greek                                                                                               |
| `cyrillic`    | Cyrillic + Supplement                                                                                                |
| `hebrew`      | Hebrew + Alphabetic Presentation Forms                                                                               |
| `georgian`    | Georgian + Georgian Supplement                                                                                       |
| `armenian`    | Armenian                                                                                                             |
| `ethiopic`    | Ethiopic + Extended                                                                                                  |
| `vietnamese`  | Vietnamese subset (ơ/ư and combining marks)                                                                          |
| `punctuation` | General punctuation (U+2000–U+206F)                                                                                  |
| `cjk`         | CJK Unified Ideographs + Hiragana + Katakana + Fullwidth                                                             |
| `hangul`      | Korean Hangul syllables + Jamo + Compatibility Jamo                                                                  |
| `cherokee`    | Cherokee (historic + supplement block)                                                                               |
| `tifinagh`    | Tifinagh                                                                                                             |
| `symbols`     | Math, currency, arrows, box-drawing, misc symbols, dingbats                                                          |
| `reading`     | Literary fiction coverage: Latin, Greek, Cyrillic, math/symbol blocks, supplemental punctuation, and CJK quote marks |
| `builtin`     | Matches the firmware's built-in font conversion intervals                                                            |

Combine presets with commas: `--intervals latin-ext,greek,cyrillic`

You can also specify arbitrary Unicode ranges directly:
`--intervals latin-ext,(0x2100-0x214F)`

To list all presets with codepoint counts:

    python3 lib/EpdFont/scripts/fontconvert_sdcard.py --list-presets

### Additional options

`--force-autohint` — force FreeType's auto-hinter instead of the font's native hinting (useful when a font's built-in hints produce poor results at small sizes).

Install custom fonts via the web interface or manual SD card copy.
