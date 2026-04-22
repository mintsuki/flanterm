# Flanterm framebuffer backend

Flanterm's framebuffer output backend. See [README](README.md) for a quick
start and [FRONTEND.md](FRONTEND.md) for the frontend API.

Add `flanterm_backends/fb.c` to the build and include
`<flanterm_backends/fb.h>` alongside `<flanterm.h>`.

## Table of contents

- [`flanterm_fb_init`](#flanterm_fb_init)
- [Rotation](#rotation)
- [Fonts](#fonts)
- [Allocator and the built-in bump allocator](#allocator-and-the-built-in-bump-allocator)
- [Flush callback](#flush-callback)
- [Build-time configuration](#build-time-configuration)

## `flanterm_fb_init`

```c
struct flanterm_context *flanterm_fb_init(
    void *(*_malloc)(size_t),
    void (*_free)(void *, size_t),
    uint32_t *framebuffer, size_t width, size_t height, size_t pitch,
    uint8_t red_mask_size, uint8_t red_mask_shift,
    uint8_t green_mask_size, uint8_t green_mask_shift,
    uint8_t blue_mask_size, uint8_t blue_mask_shift,
    uint32_t *canvas,
    uint32_t *ansi_colours, uint32_t *ansi_bright_colours,
    uint32_t *default_bg, uint32_t *default_fg,
    uint32_t *default_bg_bright, uint32_t *default_fg_bright,
    void *font, size_t font_width, size_t font_height, size_t font_spacing,
    size_t font_scale_x, size_t font_scale_y,
    size_t margin,
    int rotation
);
```

Returns a context on success, or `NULL` on failure (allocator failure,
invalid framebuffer geometry, unsupported pixel format, etc.).

| Parameter | Meaning |
| --- | --- |
| `_malloc`, `_free` | Client allocator. If both are `NULL`, the built-in bump allocator is used (only one live instance at a time; see [below](#allocator-and-the-built-in-bump-allocator)). |
| `framebuffer` | The 32-bit framebuffer, writable as `uint32_t *`. |
| `width`, `height` | Physical framebuffer dimensions in pixels. Swapped internally for `ROTATE_90`/`ROTATE_270` (see [Rotation](#rotation)). |
| `pitch` | Physical scanline stride in bytes. Must be a multiple of `sizeof(uint32_t)` and at least `width * 4`. |
| `red/green/blue_mask_size` | Channel bit-widths. Each must be at least 8, and all three must match. |
| `red/green/blue_mask_shift` | Low bit position of each channel. `shift + size <= 32` for every channel. |
| `canvas` | Optional background image: one `0x00RRGGBB` pixel per terminal pixel, sized `logical_width x logical_height`. `NULL` falls back to a solid `default_bg`. Forced to `NULL` under the built-in bump allocator. |
| `ansi_colours` | Optional 8-entry base ANSI palette (`0x00RRGGBB`, order: black, red, green, brown, blue, magenta, cyan, grey). `NULL` uses the default. |
| `ansi_bright_colours` | Optional 8-entry bright palette (same format). `NULL` uses the default. |
| `default_bg`, `default_fg` | Normal-intensity default colours (`0x00RRGGBB`). `NULL` uses black/grey. |
| `default_bg_bright`, `default_fg_bright` | Defaults when the bold/bright flag is active. `NULL` uses dark grey/white. |
| `font` | Optional VGA-style bitmap font (one byte per scanline, 256 consecutive glyphs). `NULL` uses the built-in 8x16 font. |
| `font_width`, `font_height` | Glyph dimensions when `font` is non-`NULL`. See [Fonts](#fonts). |
| `font_spacing` | Extra pixels after each glyph; see [Fonts](#fonts). |
| `font_scale_x`, `font_scale_y` | Integer glyph scale. `0`, `0` enables auto-scale: `2x` at logical dimensions >= 2560x1440, `4x` at >= 5120x2880. |
| `margin` | Margin pixels on each side of the text area. `margin * 2 < logical_width` and `margin * 2 < logical_height` must hold, or init fails. |
| `rotation` | One of `FLANTERM_FB_ROTATE_0`, `FLANTERM_FB_ROTATE_90`, `FLANTERM_FB_ROTATE_180`, `FLANTERM_FB_ROTATE_270`. |

Palettes, `default_*` colours, `font` data, and `canvas` pixels are copied
into the context during init; the input buffers do not need to outlive the
call.

## Rotation

`rotation` rotates the rendered terminal relative to the physical
framebuffer. `width`, `height`, and `pitch` always describe the physical
framebuffer; for `ROTATE_90`/`ROTATE_270` the library swaps `width` and
`height` internally to derive the logical dimensions used for cell layout
(so a physical 1920x1080 with `ROTATE_90` yields a logical 1080x1920
terminal).

## Fonts

The built-in font is 8x16 VGA-style. User fonts follow the same packed
layout: one byte per scanline, `font_height` scanlines per glyph, 256
consecutive glyphs. The source is therefore always 8 pixels wide;
`font_width` and `font_spacing` only widen the on-screen cell.

Cell columns past the 8 source pixels (from `font_width > 8` or
`font_spacing > 0`) follow the VGA 9-dot line-graphics rule: glyphs
`0xC0`-`0xDF` replicate their rightmost source pixel into the extra columns
(keeping box-drawing characters connected); other glyphs leave those
columns blank.

## Allocator and the built-in bump allocator

When both `_malloc` and `_free` are `NULL`, Flanterm uses a static bump pool
sized by `FLANTERM_FB_BUMP_ALLOC_POOL_SIZE` (default 873000 bytes, enough
for a 1920x1200 terminal on typical 32-bit and 64-bit targets). In that
mode:

- Only one instance may be live; later `flanterm_fb_init` calls return
  `NULL` until the previous instance is deinitialised.
- The canvas is forced off.
- Logical-dimension limits are derived from the pool size as the largest
  `FLANTERM_FB_BUMP_ALLOC_ASPECT_WIDTH:FLANTERM_FB_BUMP_ALLOC_ASPECT_HEIGHT`
  rectangle (default 16:10) that fits assuming the built-in 8x16 font.
  Framebuffers exceeding those limits have the usable area clipped and
  centred.

With a custom allocator, `_malloc` is used for every allocation and the
paired `_free` runs when freeing. `_free` receives the allocation
size as its second argument.

## Flush callback

```c
void flanterm_fb_set_flush_callback(
    struct flanterm_context *ctx,
    void (*flush_callback)(volatile void *address, size_t length)
);
```

When set, the framebuffer backend invokes the callback at the end of every
double-buffer flush (after `flanterm_flush`, `flanterm_full_refresh`, and
any autoflush). Arguments are the framebuffer base address and
`pitch * physical_height` -- the full physical framebuffer extent, not just
the dirty region -- suitable for driving CPU cache maintenance.

The callback must be installed after `flanterm_fb_init` returns; the
init-time refresh runs before any context exists to register it on.

## Build-time configuration

Macros that can be defined when compiling `fb.c`:

| Macro | Default | Effect |
| --- | --- | --- |
| `FLANTERM_FB_DISABLE_BUMP_ALLOC` | undefined | Compiles out the built-in bump allocator. `flanterm_fb_init` with `_malloc == NULL` then always returns `NULL`. Saves roughly `FLANTERM_FB_BUMP_ALLOC_POOL_SIZE` bytes of `.bss`. |
| `FLANTERM_FB_BUMP_ALLOC_POOL_SIZE` | `873000` | Static bump pool size. Clipping limits for framebuffers that exceed the pool are derived from this value and the aspect ratio as the largest fitting rectangle under the built-in 8x16 (+ 1px spacing) cell. Default size accommodates 1920x1200 at 16:10. |
| `FLANTERM_FB_BUMP_ALLOC_ASPECT_WIDTH` | `16` | Aspect numerator for the bump pool's clipping limits. |
| `FLANTERM_FB_BUMP_ALLOC_ASPECT_HEIGHT` | `10` | Aspect denominator for the bump pool's clipping limits. |
