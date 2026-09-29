"""Generate the FluiDez Reader identity assets from one geometric source.

The symbol is an open book inside a rounded badge: the page tops bow like a
wave and the text lines ripple across both pages, so the reading "flows". The
wordmark sets "Flui" in Lexend Deca Regular and "Dez" in Lexend Deca Bold
(Lexend was designed to improve reading fluency), with "READER" tracked below.

Outputs, relative to the repository root:
  src/images/FluiDezLogo.h   1-bit symbol and wordmark for the boot/sleep screens
  web/assets/logo.png        white symbol for the web portal header (recoloured by CSS)
  docs/brand/                symbol SVG, PNG exports and the brand sheet

Usage:
  python scripts/generate_brand_assets.py              # write the assets above
  python scripts/generate_brand_assets.py --preview D  # write everything into D only

Requires Pillow.
"""

import argparse
import math
from pathlib import Path

from PIL import Image, ImageDraw, ImageFont

ROOT = Path(__file__).resolve().parent.parent
FONT_DIR = ROOT / "lib" / "EpdFont" / "builtinFonts" / "source"
LEXEND_REGULAR = FONT_DIR / "LexendDeca" / "LexendDeca-Regular.ttf"
LEXEND_BOLD = FONT_DIR / "LexendDeca" / "LexendDeca-Bold.ttf"
INTER_REGULAR = FONT_DIR / "Inter" / "Inter-Regular.ttf"
INTER_BOLD = FONT_DIR / "Inter" / "Inter-Bold.ttf"

INK = "#111827"
PAPER = "#F7F5F0"
FLOW = "#1F7A8C"
MIST = "#DCEBEE"

DESIGN = 1000
BADGE = (60, 60, 940, 940)
BADGE_RADIUS = 200
BOOK_LIFT = 26
SPINE_GAP = 14
PAGE_OUTER = 178
TOP_CURVE = ((500 - SPINE_GAP, 352), (410, 292), (292, 236), (PAGE_OUTER, 294))
PAGE_DEPTH = 392
TEXT_OFFSETS = (112, 194, 276)
TEXT_STROKE = 28
TEXT_INSET_SPINE = 42
TEXT_INSET_OUTER = 44
TEXT_LAST_LINE_RATIO = 0.62
WAVE_AMPLITUDE = 16
WAVE_PERIOD = 150

DEVICE_SYMBOL_SIZE = 120
DEVICE_WORDMARK_SIZE = 50
DEVICE_READER_SIZE = 18
WEB_LOGO_SIZE = 96


def cubic(p0, p1, p2, p3, steps=64):
    points = []
    for i in range(steps + 1):
        t = i / steps
        a = (1 - t) ** 3
        b = 3 * (1 - t) ** 2 * t
        c = 3 * (1 - t) * t**2
        d = t**3
        points.append((a * p0[0] + b * p1[0] + c * p2[0] + d * p3[0], a * p0[1] + b * p1[1] + c * p2[1] + d * p3[1]))
    return points


def lifted(points):
    return [(x, y - BOOK_LIFT) for x, y in points]


def mirrored(points):
    return [(DESIGN - x, y) for x, y in points]


def left_top_edge():
    return lifted(cubic(*TOP_CURVE, steps=96))


def left_page():
    top = left_top_edge()
    bottom = [(x, y + PAGE_DEPTH) for x, y in top]
    return top + list(reversed(bottom))


def top_edge_y(x):
    """Height of the left page's top edge at x (the curve is monotonic in x)."""
    edge = left_top_edge()
    for (x0, y0), (x1, y1) in zip(edge, edge[1:]):
        lo, hi = min(x0, x1), max(x0, x1)
        if lo <= x <= hi:
            return y0 if x1 == x0 else y0 + (y1 - y0) * (x - x0) / (x1 - x0)
    return edge[0][1] if x > edge[0][0] else edge[-1][1]


def text_line(offset, start_x, end_x, mirror):
    """A rippling text line following the page top. The ripple uses absolute x,
    so the wave continues across the spine instead of mirroring."""
    points = []
    steps = 80
    for i in range(steps + 1):
        x = start_x + (end_x - start_x) * i / steps
        base_x = DESIGN - x if mirror else x
        y = top_edge_y(base_x) + offset + WAVE_AMPLITUDE * math.sin(2 * math.pi * x / WAVE_PERIOD)
        points.append((x, y))
    return points


def text_lines():
    spine = 500 - SPINE_GAP
    lines = []
    for mirror in (False, True):
        for index, offset in enumerate(TEXT_OFFSETS):
            outer = PAGE_OUTER + TEXT_INSET_OUTER
            inner = spine - TEXT_INSET_SPINE
            length = inner - outer
            if index == len(TEXT_OFFSETS) - 1:
                length *= TEXT_LAST_LINE_RATIO
            # Lines start at the outer margin like left-aligned text on both pages.
            start, end = outer, outer + length
            if mirror:
                start, end = DESIGN - inner, DESIGN - inner + length
            lines.append(text_line(offset, start, end, mirror))
    return lines


def draw_round_line(draw, points, width, fill):
    """Stroke a polyline with round caps by stamping discs along it. Pillow's
    wide-line joints leave jagged edges on dense curves; discs stay smooth."""
    r = width / 2
    step = max(0.5, r / 6)
    for (x0, y0), (x1, y1) in zip(points, points[1:]):
        count = max(1, math.ceil(math.hypot(x1 - x0, y1 - y0) / step))
        for i in range(count + 1):
            x = x0 + (x1 - x0) * i / count
            y = y0 + (y1 - y0) * i / count
            draw.ellipse((x - r, y - r, x + r, y + r), fill=fill)


def symbol_mask(size, badge=True):
    """Ink coverage of the symbol (255 = ink) at `size` px, antialiased."""
    canvas = Image.new("L", (DESIGN, DESIGN), 0)
    draw = ImageDraw.Draw(canvas)
    if badge:
        draw.rounded_rectangle(BADGE, radius=BADGE_RADIUS, fill=255)
        page_fill, line_fill = 0, 255
    else:
        page_fill, line_fill = 255, 0
    page = left_page()
    draw.polygon(page, fill=page_fill)
    draw.polygon(mirrored(page), fill=page_fill)
    for line in text_lines():
        draw_round_line(draw, line, TEXT_STROKE, line_fill)
    return canvas.resize((size, size), Image.LANCZOS)


def tracked_width(font, text, tracking):
    return sum(font.getlength(ch) for ch in text) + tracking * (len(text) - 1)


def draw_tracked(draw, origin, text, font, tracking, fill):
    x, y = origin
    for ch in text:
        draw.text((x, y), ch, font=font, fill=fill)
        x += font.getlength(ch) + tracking


def wordmark_mask(size, reader_size, scale=4, reader=True):
    """Ink coverage of the stacked wordmark (255 = ink), cropped to its bounds."""
    regular = ImageFont.truetype(str(LEXEND_REGULAR), size * scale)
    bold = ImageFont.truetype(str(LEXEND_BOLD), size * scale)
    small = ImageFont.truetype(str(LEXEND_REGULAR), reader_size * scale)
    joint = -size * scale * 0.03
    flui_w = regular.getlength("Flui")
    name_w = flui_w + joint + bold.getlength("Dez")
    tracking = reader_size * scale * 0.42
    reader_w = tracked_width(small, "READER", tracking)
    width = int(max(name_w, reader_w) + size * scale)
    height = int(size * scale * 2.4)
    canvas = Image.new("L", (width, height), 0)
    draw = ImageDraw.Draw(canvas)
    name_x = (width - name_w) / 2
    top = size * scale * 0.2
    draw.text((name_x, top), "Flui", font=regular, fill=255)
    draw.text((name_x + flui_w + joint, top), "Dez", font=bold, fill=255)
    if reader:
        name_bottom = top + bold.getbbox("Dez")[3]
        reader_top = name_bottom + size * scale * 0.2
        draw_tracked(draw, ((width - reader_w) / 2, reader_top), "READER", small, tracking, 255)
    canvas = canvas.crop(canvas.getbbox())
    return canvas.resize((max(1, round(canvas.width / scale)), max(1, round(canvas.height / scale))), Image.LANCZOS)


def threshold(mask, level=128):
    return mask.point(lambda v: 255 if v >= level else 0)


def pack_for_draw_icon(ink):
    """Pack a portrait ink image for GfxRenderer::drawIcon: rotated 90° CCW,
    rows MSB first, 1 = white (transparent), 0 = black."""
    rotated = ink.rotate(90, expand=True)
    width, height = rotated.size
    pixels = rotated.load()
    packed = []
    for y in range(height):
        for x0 in range(0, width, 8):
            byte = 0
            for bit in range(8):
                x = x0 + bit
                white = 1 if x >= width or pixels[x, y] < 128 else 0
                byte |= white << (7 - bit)
            packed.append(byte)
    return packed


def c_array(name, data):
    # 19 values per row is what clang-format packs into the 120-column limit.
    rows = []
    for i in range(0, len(data), 19):
        rows.append("    " + ", ".join(f"0x{v:02X}" for v in data[i : i + 19]) + ",")
    return f"inline constexpr uint8_t {name}[] = {{\n" + "\n".join(rows) + "\n};\n"


def device_header(symbol_ink, wordmark_ink):
    symbol = pack_for_draw_icon(symbol_ink)
    wordmark = pack_for_draw_icon(wordmark_ink)
    return (
        "#pragma once\n"
        "#include <cstdint>\n\n"
        "// Generated by scripts/generate_brand_assets.py. Do not edit by hand.\n"
        "// Portrait bitmaps stored rotated for GfxRenderer::drawIcon (1 = white).\n"
        "namespace FluiDezLogo {\n"
        f"constexpr int kSymbolSize = {symbol_ink.width};\n"
        f"constexpr int kWordmarkWidth = {wordmark_ink.width};\n"
        f"constexpr int kWordmarkHeight = {wordmark_ink.height};\n\n"
        + c_array("kSymbol", symbol)
        + "\n"
        + c_array("kWordmark", wordmark)
        + f"\nstatic_assert(sizeof(kSymbol) == (kSymbolSize + 7) / 8 * kSymbolSize, \"symbol size\");\n"
        + "static_assert(sizeof(kWordmark) == (kWordmarkHeight + 7) / 8 * kWordmarkWidth, \"wordmark size\");\n"
        + "}  // namespace FluiDezLogo\n"
    )


def colored(mask, ink, paper=None):
    """RGBA image: `ink` where the mask has coverage, `paper` (or transparent) elsewhere."""
    fg = Image.new("RGBA", mask.size, ink)
    fg.putalpha(mask)
    if paper is None:
        return fg
    bg = Image.new("RGBA", mask.size, paper)
    return Image.alpha_composite(bg, fg)


def svg_symbol():
    def path(points):
        return "M" + " L".join(f"{x:.1f},{y:.1f}" for x, y in points) + " Z"

    def open_path(points):
        return "M" + " L".join(f"{x:.1f},{y:.1f}" for x, y in points)

    page = left_page()
    lines = "\n".join(
        f'  <path d="{open_path(line)}" fill="none" stroke="{INK}" stroke-width="{TEXT_STROKE}" '
        'stroke-linecap="round" stroke-linejoin="round"/>'
        for line in text_lines()
    )
    x0, y0, x1, y1 = BADGE
    return (
        f'<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 {DESIGN} {DESIGN}" width="{DESIGN}" height="{DESIGN}">\n'
        "  <title>FluiDez Reader</title>\n"
        f'  <rect x="{x0}" y="{y0}" width="{x1 - x0}" height="{y1 - y0}" rx="{BADGE_RADIUS}" fill="{INK}"/>\n'
        f'  <path d="{path(page)}" fill="#FFFFFF"/>\n'
        f'  <path d="{path(mirrored(page))}" fill="#FFFFFF"/>\n'
        f"{lines}\n"
        "</svg>\n"
    )


def lockup(symbol_px, word_px, reader_px, ink, paper, gap_ratio=0.16, pad_ratio=0.22):
    symbol = colored(symbol_mask(symbol_px), ink)
    word = colored(wordmark_mask(word_px, reader_px), ink)
    gap = int(symbol_px * gap_ratio)
    pad = int(symbol_px * pad_ratio)
    width = max(symbol.width, word.width) + pad * 2
    height = symbol.height + gap + word.height + pad * 2
    sheet = Image.new("RGBA", (width, height), paper)
    sheet.alpha_composite(symbol, ((width - symbol.width) // 2, pad))
    sheet.alpha_composite(word, ((width - word.width) // 2, pad + symbol.height + gap))
    return sheet


def horizontal_lockup(symbol_px, ink, paper):
    symbol = colored(symbol_mask(symbol_px), ink)
    word = colored(wordmark_mask(int(symbol_px * 0.42), int(symbol_px * 0.15)), ink)
    pad = int(symbol_px * 0.2)
    gap = int(symbol_px * 0.18)
    width = symbol.width + gap + word.width + pad * 2
    height = max(symbol.height, word.height) + pad * 2
    sheet = Image.new("RGBA", (width, height), paper)
    sheet.alpha_composite(symbol, (pad, (height - symbol.height) // 2))
    sheet.alpha_composite(word, (pad + symbol.width + gap, (height - word.height) // 2))
    return sheet


def device_screen(symbol_ink, wordmark_ink, status, footer, dark, width=480, height=800):
    """Approximate boot/sleep screen preview (status text uses Inter, like the UI font)."""
    screen = Image.new("L", (width, height), 255)
    gap = 18
    total = symbol_ink.height + gap + wordmark_ink.height
    top = (height - total) // 2 - 20
    ink_layer = Image.new("L", (width, height), 0)
    ink_layer.paste(symbol_ink, ((width - symbol_ink.width) // 2, top))
    ink_layer.paste(wordmark_ink, ((width - wordmark_ink.width) // 2, top + symbol_ink.height + gap))
    screen.paste(0, mask=ink_layer)
    draw = ImageDraw.Draw(screen)
    small = ImageFont.truetype(str(INTER_REGULAR), 13)
    status_y = top + total + 20
    draw.text(((width - small.getlength(status)) / 2, status_y), status, font=small, fill=0)
    if footer:
        draw.text(((width - small.getlength(footer)) / 2, height - 36), footer, font=small, fill=0)
    if dark:
        screen = screen.point(lambda v: 255 - v)
    return screen.convert("RGB")


def brand_sheet():
    width, height = 1600, 1040
    sheet = Image.new("RGBA", (width, height), PAPER)
    draw = ImageDraw.Draw(sheet)
    title = ImageFont.truetype(str(INTER_BOLD), 30)
    label = ImageFont.truetype(str(INTER_REGULAR), 22)
    small = ImageFont.truetype(str(INTER_REGULAR), 18)
    draw.text((60, 40), "FluiDez Reader — identidade visual", font=title, fill=INK)
    draw.text((60, 84), "Flui (flui) + Dez (nota dez) = fluidez: leitura que flui.", font=label, fill="#4B5563")

    sheet.alpha_composite(colored(symbol_mask(360), INK), (60, 150))
    draw.text((60, 530), "Símbolo", font=small, fill="#4B5563")

    light = lockup(150, 60, 21, INK, "#FFFFFF")
    sheet.alpha_composite(light, (500, 150))
    dark = lockup(150, 60, 21, "#FFFFFF", INK)
    sheet.alpha_composite(dark, (500 + light.width + 40, 150))
    draw.text((500, 160 + light.height), "Assinatura vertical (clara e escura)", font=small, fill="#4B5563")

    horizontal = horizontal_lockup(96, INK, "#FFFFFF")
    sheet.alpha_composite(horizontal, (500, 600))
    draw.text((500, 610 + horizontal.height), "Assinatura horizontal", font=small, fill="#4B5563")

    swatch_y = 790
    for i, (name, value) in enumerate((("Tinta", INK), ("Papel", PAPER), ("Fluidez", FLOW), ("Névoa", MIST))):
        x = 60 + i * 190
        draw.rounded_rectangle((x, swatch_y, x + 160, swatch_y + 110), radius=18, fill=value, outline="#D1D5DB")
        draw.text((x, swatch_y + 122), f"{name} {value}", font=small, fill=INK)

    type_x = 860
    inter = ImageFont.truetype(str(INTER_REGULAR), 26)
    sheet.alpha_composite(colored(wordmark_mask(44, 16, reader=False), INK), (type_x, swatch_y))
    draw.text((type_x, swatch_y + 50), "Lexend Deca — marca (feita para fluência de leitura)", font=small, fill="#4B5563")
    draw.text((type_x, swatch_y + 88), "Interface do aparelho: Inter", font=inter, fill=INK)
    draw.text((type_x, swatch_y + 128), "Aparelho: preto e branco; cor só no portal e no app", font=small, fill="#4B5563")
    return sheet


def main():
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("--preview", type=Path, help="write every output into this directory only")
    args = parser.parse_args()

    symbol_ink = threshold(symbol_mask(DEVICE_SYMBOL_SIZE))
    wordmark_ink = threshold(wordmark_mask(DEVICE_WORDMARK_SIZE, DEVICE_READER_SIZE))
    web_logo = colored(symbol_mask(WEB_LOGO_SIZE), "#FFFFFF")

    if args.preview:
        out = args.preview
        out.mkdir(parents=True, exist_ok=True)
        header_path = out / "FluiDezLogo.h"
        web_path = out / "logo.png"
        brand_dir = out
        scale = 3
        for name, ink in (("device-symbol-x3.png", symbol_ink), ("device-wordmark-x3.png", wordmark_ink)):
            paper = ink.point(lambda v: 255 - v)
            paper.resize((ink.width * scale, ink.height * scale), Image.NEAREST).save(out / name)
        device_screen(symbol_ink, wordmark_ink, "INICIANDO", "1.6-fluidez7-x4-pro", False).save(out / "boot.png")
        device_screen(symbol_ink, wordmark_ink, "EM REPOUSO", "", True).save(out / "sleep.png")
    else:
        header_path = ROOT / "src" / "images" / "FluiDezLogo.h"
        web_path = ROOT / "web" / "assets" / "logo.png"
        brand_dir = ROOT / "docs" / "brand"
        brand_dir.mkdir(parents=True, exist_ok=True)

    header_path.write_text(device_header(symbol_ink, wordmark_ink), encoding="utf-8", newline="\n")
    web_logo.save(web_path, optimize=True)
    (brand_dir / "fluidez-symbol.svg").write_text(svg_symbol(), encoding="utf-8", newline="\n")
    colored(symbol_mask(512), INK).save(brand_dir / "fluidez-symbol-512.png", optimize=True)
    lockup(240, 96, 33, INK, "#FFFFFF").convert("RGB").save(brand_dir / "fluidez-lockup.png", optimize=True)
    lockup(240, 96, 33, "#FFFFFF", INK).convert("RGB").save(brand_dir / "fluidez-lockup-dark.png", optimize=True)
    brand_sheet().convert("RGB").save(brand_dir / "brand-sheet.png", optimize=True)
    print(f"symbol {symbol_ink.size}, wordmark {wordmark_ink.size}")
    print(f"wrote {header_path}, {web_path} and {brand_dir}")


if __name__ == "__main__":
    main()
