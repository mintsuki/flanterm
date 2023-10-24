/* Copyright (C) 2022-2023 mintsuki and contributors.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _FLANTERM_FR_H
#define _FLANTERM_FB_H 1

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#include "../flanterm.h"

#define FLANTERM_FB_FONT_GLYPHS 256

struct flanterm_fb_char {
    uint32_t c;
    uint32_t fg;
    uint32_t bg;
};

struct flanterm_fb_queue_item {
    size_t x, y;
    struct flanterm_fb_char c;
};

struct flanterm_fb_context {
    struct flanterm_context term;

    size_t font_width;
    size_t font_height;
    size_t glyph_width;
    size_t glyph_height;

    size_t font_scale_x;
    size_t font_scale_y;

    size_t offset_x, offset_y;

    volatile uint32_t *framebuffer;
    size_t pitch;
    size_t width;
    size_t height;
    size_t bpp;

#ifdef FLANTERM_FB_SUPPORT_BPP
    uint8_t red_mask_size, red_mask_shift;
    uint8_t green_mask_size, green_mask_shift;
    uint8_t blue_mask_size, blue_mask_shift;
#endif

    size_t font_bits_size;
    uint8_t *font_bits;
    size_t font_bool_size;
    bool *font_bool;

    uint32_t ansi_colours[8];
    uint32_t ansi_bright_colours[8];
    uint32_t default_fg, default_bg;
    uint32_t default_fg_bright, default_bg_bright;

#ifndef FLANTERM_FB_DISABLE_CANVAS
    size_t canvas_size;
    uint32_t *canvas;
#endif

    size_t grid_size;
    size_t queue_size;
    size_t map_size;

    struct flanterm_fb_char *grid;

    struct flanterm_fb_queue_item *queue;
    size_t queue_i;

    struct flanterm_fb_queue_item **map;

    uint32_t text_fg;
    uint32_t text_bg;
    size_t cursor_x;
    size_t cursor_y;

    uint32_t saved_state_text_fg;
    uint32_t saved_state_text_bg;
    size_t saved_state_cursor_x;
    size_t saved_state_cursor_y;

    size_t old_cursor_x;
    size_t old_cursor_y;
};

struct flanterm_context *flanterm_fb_init(
    void *(*_malloc)(size_t),
    void (*_free)(void *, size_t),
    uint32_t *framebuffer, size_t width, size_t height, size_t pitch,
#ifdef FLANTERM_FB_SUPPORT_BPP
    uint8_t red_mask_size, uint8_t red_mask_shift,
    uint8_t green_mask_size, uint8_t green_mask_shift,
    uint8_t blue_mask_size, uint8_t blue_mask_shift,
#endif
#ifndef FLANTERM_FB_DISABLE_CANVAS
    uint32_t *canvas,
#endif
    uint32_t *ansi_colours, uint32_t *ansi_bright_colours,
    uint32_t *default_bg, uint32_t *default_fg,
    uint32_t *default_bg_bright, uint32_t *default_fg_bright,
    void *font, size_t font_width, size_t font_height, size_t font_spacing,
    size_t font_scale_x, size_t font_scale_y,
    size_t margin
);

#ifndef FLANTERM_FB_DISABLE_BUMP_ALLOC
static inline struct flanterm_context *flanterm_fb_simple_init(
    uint32_t *framebuffer, size_t width, size_t height, size_t pitch
#ifdef FLANTERM_FB_SUPPORT_BPP
    ,
    uint8_t red_mask_size, uint8_t red_mask_shift,
    uint8_t green_mask_size, uint8_t green_mask_shift,
    uint8_t blue_mask_size, uint8_t blue_mask_shift
#endif
) {
    size_t font_scale_x = 1;
    size_t font_scale_y = 1;
    if (width >= (1920 + 1920 / 3) && height >= (1080 + 1080 / 3)) {
        font_scale_x = 2;
        font_scale_y = 2;
    }
    if (width >= (3840 + 3840 / 3) && height >= (2160 + 2160 / 3)) {
        font_scale_x = 4;
        font_scale_y = 4;
    }

    return flanterm_fb_init(
        NULL,
        NULL,
        framebuffer, width, height, pitch,
#ifdef FLANTERM_FB_SUPPORT_BPP
        red_mask_size, red_mask_shift,
        green_mask_size, green_mask_shift,
        blue_mask_size, blue_mask_shift,
#endif
#ifndef FLANTERM_FB_DISABLE_CANVAS
        NULL,
#endif
        NULL, NULL,
        NULL, NULL,
        NULL, NULL,
        NULL, 0, 0, 1,
        font_scale_x, font_scale_y,
        0
    );
}
#endif

#ifdef __cplusplus
}
#endif

#endif
