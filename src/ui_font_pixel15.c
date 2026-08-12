/*******************************************************************************
 * Size: 15 px
 * Bpp: 1
 * Opts: --bpp 1 --size 15 --font C:/Users/moham/Documents/Waveshare/3.5inch/ui/assets/Pixellari.ttf -o C:/Users/moham/Documents/Waveshare/3.5inch/ui/assets\ui_font_pixel15.c --format lvgl -r 0x20-0x7f --no-compress --no-prefilter
 ******************************************************************************/

#include "ui.h"

#ifndef UI_FONT_PIXEL15
#define UI_FONT_PIXEL15 1
#endif

#if UI_FONT_PIXEL15

/*-----------------
 *    BITMAPS
 *----------------*/

/*Store the image of the glyphs*/
static LV_ATTRIBUTE_LARGE_CONST const uint8_t glyph_bitmap[] = {
    /* U+0020 " " */
    0x0,

    /* U+0021 "!" */
    0xff, 0xff, 0x3c,

    /* U+0022 "\"" */
    0xde, 0xf7, 0xb0,

    /* U+0023 "#" */
    0x66, 0x66, 0x66, 0xff, 0xff, 0x66, 0x66, 0xff,
    0xff, 0x66, 0x66, 0x66,

    /* U+0024 "$" */
    0x31, 0x9f, 0xff, 0x79, 0xe7, 0xff, 0x8c, 0x60,

    /* U+0025 "%" */
    0x30, 0xc7, 0x9c, 0xcf, 0x8c, 0xf0, 0xce, 0xc7,
    0xfe, 0x37, 0x30, 0xf3, 0x1f, 0x33, 0x9e, 0x30,
    0xc0,

    /* U+0026 "&" */
    0x3c, 0x7e, 0xe6, 0xce, 0xec, 0x78, 0x78, 0xfe,
    0xce, 0xc6, 0xff, 0x7f,

    /* U+0027 "'" */
    0xff,

    /* U+0028 "(" */
    0x6f, 0x6d, 0xb6, 0xd9, 0xb0,

    /* U+0029 ")" */
    0xd9, 0xb6, 0xdb, 0x6f, 0x60,

    /* U+002A "*" */
    0x33, 0xff, 0xde, 0xff, 0x30,

    /* U+002B "+" */
    0x30, 0xcf, 0xff, 0x30, 0xc0,

    /* U+002C "," */
    0xf6,

    /* U+002D "-" */
    0xff, 0xf0,

    /* U+002E "." */
    0xf0,

    /* U+002F "/" */
    0xc, 0x31, 0xc6, 0x38, 0xc3, 0x1c, 0x63, 0x8c,
    0x30,

    /* U+0030 "0" */
    0x79, 0xff, 0x7e, 0xfd, 0xfa, 0xfd, 0xfb, 0xf7,
    0xfd, 0xe0,

    /* U+0031 "1" */
    0x31, 0xcf, 0x3c, 0x30, 0xc3, 0xc, 0x33, 0xff,
    0xc0,

    /* U+0032 "2" */
    0x7b, 0xfc, 0xc3, 0xc, 0x73, 0x9c, 0xf3, 0xff,
    0xc0,

    /* U+0033 "3" */
    0x7b, 0xfc, 0xc3, 0x1c, 0x61, 0xc3, 0xcf, 0xf7,
    0x80,

    /* U+0034 "4" */
    0xc, 0x38, 0xf3, 0xee, 0xd9, 0xbf, 0xff, 0xc,
    0x18, 0x30,

    /* U+0035 "5" */
    0xff, 0xfc, 0x30, 0xfb, 0xf0, 0xc3, 0xcf, 0xf7,
    0x80,

    /* U+0036 "6" */
    0x7b, 0xfc, 0xf0, 0xfb, 0xfc, 0xf3, 0xcf, 0xf7,
    0x80,

    /* U+0037 "7" */
    0xff, 0xfc, 0x18, 0x71, 0xc7, 0xc, 0x18, 0x30,
    0x60, 0xc0,

    /* U+0038 "8" */
    0x7b, 0xfc, 0xf3, 0x7b, 0xfc, 0xf3, 0xcf, 0xf7,
    0x80,

    /* U+0039 "9" */
    0x7b, 0xfc, 0xf3, 0xfd, 0xf0, 0xc3, 0xcf, 0xf7,
    0x80,

    /* U+003A ":" */
    0xf0, 0xf,

    /* U+003B ";" */
    0xf0, 0xf, 0x60,

    /* U+003C "<" */
    0x7, 0x8f, 0xcf, 0xde, 0xf, 0x1, 0xf8, 0x7e,
    0xf,

    /* U+003D "=" */
    0xff, 0xf0, 0x0, 0xff, 0xf0,

    /* U+003E ">" */
    0xf0, 0x7e, 0xf, 0xc0, 0xf0, 0x79, 0xfb, 0xf1,
    0xe0,

    /* U+003F "?" */
    0x7b, 0xfc, 0xc3, 0x1c, 0xe3, 0xc, 0x0, 0xc3,
    0x0,

    /* U+0040 "@" */
    0x3f, 0xcf, 0xfb, 0x83, 0xe7, 0x9d, 0xf3, 0xb6,
    0x76, 0xce, 0xd9, 0xdb, 0x3b, 0xff, 0x3f, 0x70,
    0x7, 0xfc, 0x7f, 0x80,

    /* U+0041 "A" */
    0x3c, 0x7e, 0xe7, 0xc3, 0xc3, 0xff, 0xff, 0xc3,
    0xc3, 0xc3, 0xc3,

    /* U+0042 "B" */
    0xfe, 0xff, 0xc3, 0xc3, 0xff, 0xfe, 0xc3, 0xc3,
    0xc3, 0xff, 0xfe,

    /* U+0043 "C" */
    0x79, 0xff, 0x1e, 0xc, 0x18, 0x30, 0x60, 0xc7,
    0xfd, 0xe0,

    /* U+0044 "D" */
    0xfe, 0xff, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3,
    0xc3, 0xff, 0xfe,

    /* U+0045 "E" */
    0xff, 0xfc, 0x30, 0xff, 0xfc, 0x30, 0xc3, 0xff,
    0xc0,

    /* U+0046 "F" */
    0xff, 0xfc, 0x30, 0xff, 0xfc, 0x30, 0xc3, 0xc,
    0x0,

    /* U+0047 "G" */
    0x79, 0xff, 0x1e, 0xc, 0x19, 0xf3, 0xe3, 0xc7,
    0xfd, 0xe0,

    /* U+0048 "H" */
    0xc3, 0xc3, 0xc3, 0xc3, 0xff, 0xff, 0xc3, 0xc3,
    0xc3, 0xc3, 0xc3,

    /* U+0049 "I" */
    0xff, 0x66, 0x66, 0x66, 0x6f, 0xf0,

    /* U+004A "J" */
    0x1e, 0x3c, 0x30, 0x60, 0xc1, 0x83, 0x6, 0xcd,
    0xf9, 0xe0,

    /* U+004B "K" */
    0xc3, 0xc7, 0xce, 0xdc, 0xf8, 0xf0, 0xf8, 0xdc,
    0xce, 0xc7, 0xc3,

    /* U+004C "L" */
    0xc3, 0xc, 0x30, 0xc3, 0xc, 0x30, 0xc3, 0xff,
    0xc0,

    /* U+004D "M" */
    0xc1, 0xf0, 0xfc, 0xff, 0xfd, 0xfe, 0x6f, 0x7,
    0x83, 0xc1, 0xe0, 0xf0, 0x60,

    /* U+004E "N" */
    0xc3, 0xe3, 0xf3, 0xfb, 0xff, 0xdf, 0xdf, 0xcf,
    0xc7, 0xc3, 0xc3,

    /* U+004F "O" */
    0x7e, 0xff, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3,
    0xc3, 0xff, 0x7e,

    /* U+0050 "P" */
    0xfd, 0xff, 0x1e, 0x3c, 0x7f, 0xff, 0x60, 0xc1,
    0x83, 0x0,

    /* U+0051 "Q" */
    0x7e, 0x7f, 0xb0, 0xd8, 0x6c, 0x36, 0x1b, 0xd,
    0x86, 0xc7, 0x7f, 0x9f, 0xe0,

    /* U+0052 "R" */
    0xfe, 0xff, 0xc3, 0xc3, 0xff, 0xfe, 0xc3, 0xc3,
    0xc3, 0xc3, 0xc3,

    /* U+0053 "S" */
    0x7e, 0xff, 0xc3, 0xc0, 0xfe, 0x7f, 0x3, 0x3,
    0xc3, 0xff, 0x7e,

    /* U+0054 "T" */
    0xff, 0xff, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18,
    0x18, 0x18, 0x18,

    /* U+0055 "U" */
    0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3,
    0xc3, 0xff, 0x7e,

    /* U+0056 "V" */
    0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xe7,
    0x7e, 0x3c, 0x18,

    /* U+0057 "W" */
    0xc0, 0xe0, 0x70, 0x39, 0x9c, 0xce, 0x67, 0x33,
    0x99, 0xff, 0xbf, 0xcc, 0xc0,

    /* U+0058 "X" */
    0xc3, 0xe7, 0x66, 0x7e, 0x3c, 0x18, 0x3c, 0x7e,
    0x66, 0xe7, 0xc3,

    /* U+0059 "Y" */
    0xc3, 0xe7, 0x66, 0x7e, 0x3c, 0x18, 0x18, 0x18,
    0x18, 0x18, 0x18,

    /* U+005A "Z" */
    0xff, 0xff, 0xc0, 0xe0, 0xe0, 0xe0, 0x60, 0x70,
    0x70, 0x70, 0x7f, 0xff, 0xe0,

    /* U+005B "[" */
    0xff, 0x6d, 0xb6, 0xdb, 0xf0,

    /* U+005C "\\" */
    0xc3, 0xe, 0x18, 0x70, 0xc3, 0xe, 0x18, 0x70,
    0xc3,

    /* U+005D "]" */
    0xfd, 0xb6, 0xdb, 0x6f, 0xf0,

    /* U+005E "^" */
    0x10, 0x71, 0xe7, 0x7c, 0x60,

    /* U+005F "_" */
    0xff, 0xff,

    /* U+0060 "`" */
    0xdd, 0x80,

    /* U+0061 "a" */
    0x7b, 0xfc, 0xf3, 0xcf, 0x3f, 0xdf,

    /* U+0062 "b" */
    0xc1, 0x83, 0x6, 0xef, 0xfc, 0xf1, 0xe3, 0xe7,
    0xff, 0x70,

    /* U+0063 "c" */
    0x7b, 0xfc, 0xf0, 0xc3, 0x3f, 0xde,

    /* U+0064 "d" */
    0x6, 0xc, 0x1b, 0xbf, 0xf9, 0xf1, 0xe3, 0xcf,
    0xfd, 0xd8,

    /* U+0065 "e" */
    0x7b, 0xfc, 0xff, 0xc3, 0x3f, 0xde,

    /* U+0066 "f" */
    0x37, 0x6f, 0xf6, 0x66, 0x66, 0x60,

    /* U+0067 "g" */
    0x77, 0xff, 0x3e, 0x3c, 0x79, 0xff, 0xbb, 0x7,
    0xff, 0xf0,

    /* U+0068 "h" */
    0xc1, 0x83, 0x6, 0xef, 0xfc, 0xf1, 0xe3, 0xc7,
    0x8f, 0x18,

    /* U+0069 "i" */
    0xf3, 0xff, 0xfc,

    /* U+006A "j" */
    0x33, 0x3, 0x33, 0x33, 0x33, 0x33, 0xfe,

    /* U+006B "k" */
    0xc1, 0x83, 0x6, 0x3c, 0xfb, 0xbe, 0x7c, 0xdd,
    0x9f, 0x18,

    /* U+006C "l" */
    0xff, 0xff, 0xfc,

    /* U+006D "m" */
    0xff, 0xbf, 0xfc, 0xcf, 0x33, 0xcc, 0xf3, 0x3c,
    0xcf, 0x33,

    /* U+006E "n" */
    0xfb, 0xfc, 0xf3, 0xcf, 0x3c, 0xf3,

    /* U+006F "o" */
    0x7b, 0xfc, 0xf3, 0xcf, 0x3f, 0xde,

    /* U+0070 "p" */
    0xdd, 0xff, 0x9e, 0x3c, 0x7c, 0xff, 0xee, 0xc1,
    0x83, 0x0,

    /* U+0071 "q" */
    0x77, 0xff, 0x3e, 0x3c, 0x79, 0xff, 0xbb, 0x6,
    0xc, 0x18,

    /* U+0072 "r" */
    0xfb, 0xfc, 0xf0, 0xc3, 0xc, 0x30,

    /* U+0073 "s" */
    0x7f, 0xf1, 0xe7, 0x8f, 0xfe,

    /* U+0074 "t" */
    0x66, 0x6f, 0xf6, 0x66, 0x67, 0x30,

    /* U+0075 "u" */
    0xcf, 0x3c, 0xf3, 0xcf, 0x3f, 0xdf,

    /* U+0076 "v" */
    0xcf, 0x3c, 0xf3, 0xcf, 0xf7, 0x8c,

    /* U+0077 "w" */
    0xcc, 0xf3, 0x3c, 0xcf, 0x33, 0xcc, 0xf3, 0x3f,
    0xfd, 0xff,

    /* U+0078 "x" */
    0xcf, 0xf7, 0x8c, 0x31, 0xef, 0xf3,

    /* U+0079 "y" */
    0xcf, 0x3c, 0xf3, 0xcf, 0x3f, 0xde, 0x3b, 0xce,
    0x0,

    /* U+007A "z" */
    0xff, 0xf1, 0x8c, 0x31, 0x8f, 0xff,

    /* U+007B "{" */
    0x39, 0xd8, 0xc6, 0x63, 0xc, 0x63, 0xe, 0x70,

    /* U+007C "|" */
    0xff, 0xff, 0xff, 0xc0,

    /* U+007D "}" */
    0xe7, 0xc, 0x63, 0xc, 0x66, 0x31, 0xb9, 0xc0,

    /* U+007E "~" */
    0x73, 0xff, 0xce
};


/*---------------------
 *  GLYPH DESCRIPTION
 *--------------------*/

static const lv_font_fmt_txt_glyph_dsc_t glyph_dsc[] = {
    {.bitmap_index = 0, .adv_w = 0, .box_w = 0, .box_h = 0, .ofs_x = 0, .ofs_y = 0} /* id = 0 reserved */,
    {.bitmap_index = 0, .adv_w = 75, .box_w = 1, .box_h = 1, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1, .adv_w = 60, .box_w = 2, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 4, .adv_w = 105, .box_w = 5, .box_h = 4, .ofs_x = 1, .ofs_y = 7},
    {.bitmap_index = 7, .adv_w = 135, .box_w = 8, .box_h = 12, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 19, .adv_w = 135, .box_w = 5, .box_h = 12, .ofs_x = 2, .ofs_y = -1},
    {.bitmap_index = 27, .adv_w = 210, .box_w = 12, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 44, .adv_w = 150, .box_w = 8, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 56, .adv_w = 60, .box_w = 2, .box_h = 4, .ofs_x = 1, .ofs_y = 7},
    {.bitmap_index = 57, .adv_w = 75, .box_w = 3, .box_h = 12, .ofs_x = 1, .ofs_y = -1},
    {.bitmap_index = 62, .adv_w = 90, .box_w = 3, .box_h = 12, .ofs_x = 2, .ofs_y = -1},
    {.bitmap_index = 67, .adv_w = 105, .box_w = 6, .box_h = 6, .ofs_x = 0, .ofs_y = 6},
    {.bitmap_index = 72, .adv_w = 120, .box_w = 6, .box_h = 6, .ofs_x = 1, .ofs_y = 2},
    {.bitmap_index = 77, .adv_w = 60, .box_w = 2, .box_h = 4, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 78, .adv_w = 120, .box_w = 6, .box_h = 2, .ofs_x = 1, .ofs_y = 4},
    {.bitmap_index = 80, .adv_w = 60, .box_w = 2, .box_h = 2, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 81, .adv_w = 105, .box_w = 6, .box_h = 12, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 90, .adv_w = 135, .box_w = 7, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 100, .adv_w = 135, .box_w = 6, .box_h = 11, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 109, .adv_w = 120, .box_w = 6, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 118, .adv_w = 120, .box_w = 6, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 127, .adv_w = 135, .box_w = 7, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 137, .adv_w = 120, .box_w = 6, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 146, .adv_w = 120, .box_w = 6, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 155, .adv_w = 135, .box_w = 7, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 165, .adv_w = 120, .box_w = 6, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 174, .adv_w = 120, .box_w = 6, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 183, .adv_w = 60, .box_w = 2, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 185, .adv_w = 60, .box_w = 2, .box_h = 10, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 188, .adv_w = 165, .box_w = 9, .box_h = 8, .ofs_x = 0, .ofs_y = 1},
    {.bitmap_index = 197, .adv_w = 120, .box_w = 6, .box_h = 6, .ofs_x = 1, .ofs_y = 2},
    {.bitmap_index = 202, .adv_w = 165, .box_w = 9, .box_h = 8, .ofs_x = 0, .ofs_y = 1},
    {.bitmap_index = 211, .adv_w = 120, .box_w = 6, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 220, .adv_w = 210, .box_w = 11, .box_h = 14, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 240, .adv_w = 150, .box_w = 8, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 251, .adv_w = 150, .box_w = 8, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 262, .adv_w = 150, .box_w = 7, .box_h = 11, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 272, .adv_w = 165, .box_w = 8, .box_h = 11, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 283, .adv_w = 135, .box_w = 6, .box_h = 11, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 292, .adv_w = 135, .box_w = 6, .box_h = 11, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 301, .adv_w = 150, .box_w = 7, .box_h = 11, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 311, .adv_w = 150, .box_w = 8, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 322, .adv_w = 90, .box_w = 4, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 328, .adv_w = 120, .box_w = 7, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 338, .adv_w = 150, .box_w = 8, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 349, .adv_w = 120, .box_w = 6, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 358, .adv_w = 180, .box_w = 9, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 371, .adv_w = 165, .box_w = 8, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 382, .adv_w = 165, .box_w = 8, .box_h = 11, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 393, .adv_w = 135, .box_w = 7, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 403, .adv_w = 180, .box_w = 9, .box_h = 11, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 416, .adv_w = 150, .box_w = 8, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 427, .adv_w = 150, .box_w = 8, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 438, .adv_w = 150, .box_w = 8, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 449, .adv_w = 165, .box_w = 8, .box_h = 11, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 460, .adv_w = 165, .box_w = 8, .box_h = 11, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 471, .adv_w = 180, .box_w = 9, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 484, .adv_w = 150, .box_w = 8, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 495, .adv_w = 150, .box_w = 8, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 506, .adv_w = 165, .box_w = 9, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 519, .adv_w = 75, .box_w = 3, .box_h = 12, .ofs_x = 1, .ofs_y = -1},
    {.bitmap_index = 524, .adv_w = 105, .box_w = 6, .box_h = 12, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 533, .adv_w = 90, .box_w = 3, .box_h = 12, .ofs_x = 2, .ofs_y = -1},
    {.bitmap_index = 538, .adv_w = 120, .box_w = 7, .box_h = 5, .ofs_x = 0, .ofs_y = 5},
    {.bitmap_index = 543, .adv_w = 135, .box_w = 8, .box_h = 2, .ofs_x = 0, .ofs_y = -3},
    {.bitmap_index = 545, .adv_w = 75, .box_w = 3, .box_h = 3, .ofs_x = 1, .ofs_y = 8},
    {.bitmap_index = 547, .adv_w = 120, .box_w = 6, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 553, .adv_w = 135, .box_w = 7, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 563, .adv_w = 120, .box_w = 6, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 569, .adv_w = 135, .box_w = 7, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 579, .adv_w = 120, .box_w = 6, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 585, .adv_w = 90, .box_w = 4, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 591, .adv_w = 135, .box_w = 7, .box_h = 11, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 601, .adv_w = 135, .box_w = 7, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 611, .adv_w = 60, .box_w = 2, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 614, .adv_w = 60, .box_w = 4, .box_h = 14, .ofs_x = -1, .ofs_y = -3},
    {.bitmap_index = 621, .adv_w = 120, .box_w = 7, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 631, .adv_w = 60, .box_w = 2, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 634, .adv_w = 180, .box_w = 10, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 644, .adv_w = 120, .box_w = 6, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 650, .adv_w = 120, .box_w = 6, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 656, .adv_w = 135, .box_w = 7, .box_h = 11, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 666, .adv_w = 135, .box_w = 7, .box_h = 11, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 676, .adv_w = 120, .box_w = 6, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 682, .adv_w = 120, .box_w = 5, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 687, .adv_w = 75, .box_w = 4, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 693, .adv_w = 120, .box_w = 6, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 699, .adv_w = 120, .box_w = 6, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 705, .adv_w = 180, .box_w = 10, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 715, .adv_w = 120, .box_w = 6, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 721, .adv_w = 120, .box_w = 6, .box_h = 11, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 730, .adv_w = 120, .box_w = 6, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 736, .adv_w = 90, .box_w = 5, .box_h = 12, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 744, .adv_w = 60, .box_w = 2, .box_h = 13, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 748, .adv_w = 90, .box_w = 5, .box_h = 12, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 756, .adv_w = 150, .box_w = 8, .box_h = 3, .ofs_x = 1, .ofs_y = 4}
};

/*---------------------
 *  CHARACTER MAPPING
 *--------------------*/



/*Collect the unicode lists and glyph_id offsets*/
static const lv_font_fmt_txt_cmap_t cmaps[] =
{
    {
        .range_start = 32, .range_length = 95, .glyph_id_start = 1,
        .unicode_list = NULL, .glyph_id_ofs_list = NULL, .list_length = 0, .type = LV_FONT_FMT_TXT_CMAP_FORMAT0_TINY
    }
};



/*--------------------
 *  ALL CUSTOM DATA
 *--------------------*/

#if LVGL_VERSION_MAJOR == 8
/*Store all the custom data of the font*/
static  lv_font_fmt_txt_glyph_cache_t cache;
#endif

#if LVGL_VERSION_MAJOR >= 8
static const lv_font_fmt_txt_dsc_t font_dsc = {
#else
static lv_font_fmt_txt_dsc_t font_dsc = {
#endif
    .glyph_bitmap = glyph_bitmap,
    .glyph_dsc = glyph_dsc,
    .cmaps = cmaps,
    .kern_dsc = NULL,
    .kern_scale = 0,
    .cmap_num = 1,
    .bpp = 1,
    .kern_classes = 0,
    .bitmap_format = 0,
#if LVGL_VERSION_MAJOR == 8
    .cache = &cache
#endif
};



/*-----------------
 *  PUBLIC FONT
 *----------------*/

/*Initialize a public general font descriptor*/
#if LVGL_VERSION_MAJOR >= 8
const lv_font_t ui_font_pixel15 = {
#else
lv_font_t ui_font_pixel15 = {
#endif
    .get_glyph_dsc = lv_font_get_glyph_dsc_fmt_txt,    /*Function pointer to get glyph's data*/
    .get_glyph_bitmap = lv_font_get_bitmap_fmt_txt,    /*Function pointer to get glyph's bitmap*/
    .line_height = 15,          /*The maximum line height required by the font*/
    .base_line = 3,             /*Baseline measured from the bottom of the line*/
#if !(LVGL_VERSION_MAJOR == 6 && LVGL_VERSION_MINOR == 0)
    .subpx = LV_FONT_SUBPX_NONE,
#endif
#if LV_VERSION_CHECK(7, 4, 0) || LVGL_VERSION_MAJOR >= 8
    .underline_position = -2,
    .underline_thickness = 1,
#endif
    .dsc = &font_dsc,          /*The custom font data. Will be accessed by `get_glyph_bitmap/dsc` */
#if LV_VERSION_CHECK(8, 2, 0) || LVGL_VERSION_MAJOR >= 9
    .fallback = NULL,
#endif
    .user_data = NULL,
};



#endif /*#if UI_FONT_PIXEL15*/

