#!/usr/bin/env python3
"""
bdf_to_parola.py - Convert BDF font file to MD_MAX72XX Version 1 font header

Usage:
    python3 bdf_to_parola.py <font.bdf> [first_char] [last_char] [var_name] [display_height] > output.h

    first_char      First ASCII code to include (default: 32 = space)
    last_char       Last ASCII code to include  (default: 58 = colon)
    var_name        C variable name             (default: MatrixLight8Font)
    display_height  Physical display row count  (default: font's own height)
                    Use 8 to vertically centre a 6px font on an 8-row display.

Characters not present in the BDF or outside the requested range are emitted
as zero-width placeholder entries so the font table stays contiguous.

MD_MAX72XX Version 1 font format (from MD_MAX72xx_lib.h):
  byte 0    : 'F'  (file type indicator)
  byte 1    : 1    (version)
  byte 2    : first ASCII code in table
  byte 3    : last  ASCII code in table
  byte 4    : font height in pixels
  Per character (repeated for each code from first to last):
    byte n       : number of columns (0 = empty/unused character)
    byte n+1..m  : column data, left to right
      bit 7 = top    row (row 0)
      bit 0 = bottom row (row height-1)

BDF bitmap format:
  Each BITMAP row is a hex string (MSB = leftmost pixel).
  Rows are ordered top to bottom.
  Each row is padded to a whole number of bytes.
  Character placement within the font cell is described by BBX:
    BBX <width> <height> <x-offset> <y-offset>
  where y-offset is the distance of the character bottom from the baseline,
  increasing upward (BDF coordinate system).
"""

import sys
import os


def parse_bdf(filename, first_char, last_char):
    """Parse a BDF file and return (chars dict, font_height).

    chars maps ASCII code -> dict with keys:
        bbx_w, bbx_h, bbx_x, bbx_y, bitmap (list of hex strings)
    """
    chars = {}
    font_height = 8

    with open(filename, 'r', encoding='utf-8', errors='replace') as f:
        lines = f.readlines()

    i = 0
    while i < len(lines):
        line = lines[i].strip()

        if line.startswith('FONTBOUNDINGBOX'):
            parts = line.split()
            # FONTBOUNDINGBOX fbbx fbby xoff yoff
            font_height = int(parts[2])

        elif line.startswith('STARTCHAR'):
            encoding = None
            bbx_w = bbx_h = bbx_x = bbx_y = 0
            bitmap_lines = []
            in_bitmap = False

            j = i + 1
            while j < len(lines):
                l = lines[j].strip()
                if l == 'ENDCHAR':
                    break
                if l.startswith('ENCODING'):
                    encoding = int(l.split()[1])
                elif l.startswith('BBX'):
                    parts = l.split()
                    bbx_w  = int(parts[1])
                    bbx_h  = int(parts[2])
                    bbx_x  = int(parts[3])
                    bbx_y  = int(parts[4])
                elif l == 'BITMAP':
                    in_bitmap = True
                elif in_bitmap and l:
                    bitmap_lines.append(l)
                j += 1

            i = j  # skip to ENDCHAR line

            if encoding is not None and first_char <= encoding <= last_char:
                chars[encoding] = {
                    'bbx_w': bbx_w,
                    'bbx_h': bbx_h,
                    'bbx_x': bbx_x,
                    'bbx_y': bbx_y,
                    'bitmap': bitmap_lines,
                }

        i += 1

    return chars, font_height


def char_to_columns(char_data, font_height, display_height):
    """Convert a BDF character definition to MD_MAX72XX column bytes.

    font_height    — logical height declared in the BDF (used for BBX placement)
    display_height — physical row count of the target display (column bytes use
                     this many bits).  When larger than font_height the glyph is
                     vertically centred within the display rows.

    Bit ordering (FC16_HW empirical): bit 0 = top row (row 0), bit 7 = bottom.
    """
    bbx_w  = char_data['bbx_w']
    bbx_h  = char_data['bbx_h']
    bbx_y  = char_data['bbx_y']
    bitmap = char_data['bitmap']

    # Allocate a display_height-row pixel grid, all zeroes.
    pixels = [[0] * bbx_w for _ in range(display_height)]

    # Vertical offset to centre the font_height glyph within display_height rows.
    v_offset = (display_height - font_height) // 2

    # BDF y increases upward; display row 0 is the top of the font cell.
    top_row = v_offset + (font_height - bbx_y - bbx_h)

    for row_idx, hex_row in enumerate(bitmap):
        display_row = top_row + row_idx
        if not (0 <= display_row < display_height) or not hex_row:
            continue

        val = int(hex_row, 16)
        # BDF pads each row to a whole number of bytes; each hex digit = 4 bits
        total_bits = len(hex_row) * 4

        for col in range(bbx_w):
            bit = (val >> (total_bits - 1 - col)) & 1
            pixels[display_row][col] = bit

    # Transpose to column bytes (bit 0 = top row, bit display_height-1 = bottom).
    columns = []
    for col in range(bbx_w):
        col_byte = 0
        for row in range(display_height):
            col_byte |= pixels[row][col] << row
        columns.append(col_byte)

    return columns


def safe_label(code):
    """Return a printable label for a character code."""
    if 32 <= code < 127:
        ch = chr(code)
        return f"'{ch}'" if ch != "'" else "\"'\""
    return f'#{code}'


def generate_header(chars, font_height, display_height, first_char, last_char, font_name, var_name):
    """Return a C header string with the MD_MAX72XX Version 1 font array."""
    out = []
    out.append(f'// {font_name} - MD_MAX72XX Version 1 font header')
    out.append(f'// Generated by tools/bdf_to_parola.py')
    out.append(f'// Source font: {font_name}.bdf  (https://github.com/trip5/Matrix-Fonts)')
    out.append(f'//')
    out.append(f'// ASCII range : {first_char} ({safe_label(first_char)}) .. {last_char} ({safe_label(last_char)})')
    out.append(f'// Font height : {font_height} pixels (rendered in {display_height}-row display grid)')
    if display_height != font_height:
        v_offset = (display_height - font_height) // 2
        out.append(f'// V-offset    : {v_offset} rows  (glyph centred vertically in {display_height}-row display)')
    out.append(f'//')
    out.append(f'// MD_MAX72XX Version 1 format:')
    out.append(f'//   [0]  \'F\'          file-type marker')
    out.append(f'//   [1]  1            version')
    out.append(f'//   [2]  firstASCII')
    out.append(f'//   [3]  lastASCII')
    out.append(f'//   [4]  height       font height in pixels')
    out.append(f'//   Per character: width_byte, col0, col1, ...')
    out.append(f'//   Column byte  : bit 0 = top row (row 0), bit 7 = bottom row  [FC16_HW empirical]')
    out.append('')
    out.append('#pragma once')
    out.append('#ifdef ARDUINO')
    out.append('#include <Arduino.h>')
    out.append('#endif')
    out.append('')
    out.append(f'static const uint8_t {var_name}[] PROGMEM = {{')
    out.append(f"  'F', 1,        // Version 1 header")
    out.append(f'  {first_char}, {last_char},  // ASCII range: {safe_label(first_char)} .. {safe_label(last_char)}')
    out.append(f'  {display_height},          // Display height in pixels')
    out.append('')

    for code in range(first_char, last_char + 1):
        label = safe_label(code)
        if code in chars:
            cols = char_to_columns(chars[code], font_height, display_height)
            if cols:
                col_str = ', '.join(f'0x{b:02X}' for b in cols)
                out.append(f'  {len(cols)}, {col_str},  // {label}')
            else:
                out.append(f'  0,  // {label} (zero-width glyph)')
        else:
            out.append(f'  0,  // {label} (not in font, placeholder)')

    out.append('};')
    out.append('')
    return '\n'.join(out)


if __name__ == '__main__':
    if len(sys.argv) < 2:
        prog = os.path.basename(sys.argv[0])
        print(f'Usage: {prog} <font.bdf> [first_char] [last_char] [var_name] [display_height]',
              file=sys.stderr)
        print(f'  Defaults: first_char=32  last_char=126  var_name=MatrixLight8Font  display_height=font height',
              file=sys.stderr)
        print(f'Example:  {prog} tools/fonts/MatrixLight8.bdf 32 126 MatrixLight8Font > include/MatrixLight8_font.h',
              file=sys.stderr)
        print(f'Example:  {prog} tools/fonts/MatrixLight6.bdf 32 126 MatrixLight6Font 8 > include/MatrixLight6_font.h',
              file=sys.stderr)
        sys.exit(1)

    bdf_file       = sys.argv[1]
    first_char     = int(sys.argv[2])  if len(sys.argv) > 2 else 32
    last_char      = int(sys.argv[3])  if len(sys.argv) > 3 else 126
    var_name       = sys.argv[4]       if len(sys.argv) > 4 else 'MatrixLight8Font'
    font_name      = os.path.splitext(os.path.basename(bdf_file))[0]

    chars, font_height = parse_bdf(bdf_file, first_char, last_char)

    display_height = int(sys.argv[5]) if len(sys.argv) > 5 else font_height

    print(generate_header(chars, font_height, display_height, first_char, last_char, font_name, var_name))
