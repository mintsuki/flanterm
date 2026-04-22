# Flanterm frontend

Flanterm's frontend API. See [README](README.md) for a quick start and
[BACKEND_FB.md](BACKEND_FB.md) for the framebuffer backend.

## Table of contents

- [Build integration](#build-integration)
  - [Required functions](#required-functions)
- [Core API](#core-api)
- [Event callback](#event-callback)

## Build integration

Add `flanterm.c` plus the desired backend source(s) (e.g.
`flanterm_backends/fb.c`) to the build, and put `src/` on the include path.
C99 or newer is required.

### Required functions

The environment Flanterm is linked into must provide standards-compliant
`memset`, `memcpy`, `memmove`, and `memcmp`.

## Core API

Declared in `<flanterm.h>`.

```c
void flanterm_write(struct flanterm_context *ctx, const char *buf, size_t count);
```
Feeds `count` bytes through the parser. Flushes at the end if autoflush is on
(the default).

```c
void flanterm_flush(struct flanterm_context *ctx);
```
Flushes any pending output. Relevant when autoflush is off.

```c
void flanterm_full_refresh(struct flanterm_context *ctx);
```
Redraws from the cell grid, bypassing the dirty-queue. Appropriate after the
backing surface has been clobbered.

```c
void flanterm_deinit(struct flanterm_context *ctx, void (*_free)(void *, size_t));
```
Tears down the context. `_free` must match the allocator passed at init or
be `NULL` if applicable.

```c
void flanterm_get_dimensions(struct flanterm_context *ctx, size_t *cols, size_t *rows);
```
Returns the terminal size in character cells.

```c
void flanterm_set_autoflush(struct flanterm_context *ctx, bool state);
```
Enables/disables autoflush (on by default). With autoflush off, explicit
`flanterm_flush` calls are required to commit writes.

```c
void flanterm_set_callback(
    struct flanterm_context *ctx,
    void (*callback)(struct flanterm_context *, uint64_t, uint64_t, uint64_t, uint64_t)
);
```
Installs the event callback (see [Event callback](#event-callback)).

```c
void flanterm_get_cursor_pos(struct flanterm_context *ctx, size_t *x, size_t *y);
void flanterm_set_cursor_pos(struct flanterm_context *ctx, size_t x, size_t y);
```
Queries or sets the cursor position (zero-based; `set_cursor_pos` clamps to
the terminal bounds).

```c
void flanterm_set_text_fg(struct flanterm_context *ctx, size_t colour, bool bright);
void flanterm_set_text_bg(struct flanterm_context *ctx, size_t colour, bool bright);
```
Sets the foreground/background colour. `colour` is 0-7 (values outside are
ignored); `bright` selects the bright palette slot.

```c
void flanterm_reset_text_fg(struct flanterm_context *ctx);
void flanterm_reset_text_bg(struct flanterm_context *ctx);
```
Resets the foreground/background to the configured default (honouring the
bold/bright state).

```c
void flanterm_clear(struct flanterm_context *ctx, bool move);
```
Clears the screen; `move=true` also homes the cursor.

## Event callback

The event callback function has the signature
`void fn(struct flanterm_context *, uint64_t, uint64_t, uint64_t, uint64_t)`.
The first `uint64_t` is the event ID (defined in `<flanterm.h>`); the other
three are event-specific:

| ID | Symbol | Trigger | Remaining arguments |
| --- | --- | --- | --- |
| 10 | `FLANTERM_CB_DEC`           | Every DEC private mode set/reset (`CSI ? ... h`/`l`), including library-handled ones. | parameter count; pointer to the `uint32_t` parameter array; final byte (`'h'` or `'l'`). |
| 20 | `FLANTERM_CB_BELL`          | BEL (`0x07`). | all zero. |
| 30 | `FLANTERM_CB_PRIVATE_ID`    | Device attributes (`CSI c`) or ID request (`ESC Z`). | all zero. |
| 40 | `FLANTERM_CB_STATUS_REPORT` | Status report (`CSI 5 n`). | all zero. |
| 50 | `FLANTERM_CB_POS_REPORT`    | Cursor position report (`CSI 6 n`). | cursor column (1-based); cursor row (1-based); 0. |
| 60 | `FLANTERM_CB_KBD_LEDS`      | Keyboard LED command (`CSI q`). | LED code; 0; 0. |
| 70 | `FLANTERM_CB_MODE`          | Unhandled ANSI set/reset (`CSI ... h`/`l`); only insert mode (`CSI 4 h`/`l`) is handled internally. | parameter count; parameter array pointer; final byte. |
| 80 | `FLANTERM_CB_LINUX`         | Linux-console private sequence (`CSI Pn1 ; Pn2 ; ... ]`). | parameter count; parameter array pointer; 0. |
| 90 | `FLANTERM_CB_OSC`           | Complete OSC sequence (`OSC Pt ST`/`OSC Pt BEL`). | parsed OSC number (digits before the first `;`, or 0 if none); payload length in bytes; payload pointer (not NUL-terminated). |

Parameter arrays and OSC payloads are valid only for the callback's
duration; anything that must outlive it has to be copied. Flanterm does not
emit responses itself, so the event callback is the hook for device-attribute
replies, LED state, OSC 52 clipboard handling, and the like.
