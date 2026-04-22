# Flanterm

A fast, reasonably complete terminal emulator with multiple output backends;
a framebuffer backend is included. Freestanding C99 - suitable for kernels,
bootloaders, and other bare-metal environments.

Flanterm implements a partial subset/superset of the Linux console/VT100
vocabulary described in
[console_codes(4)](https://man7.org/linux/man-pages/man4/console_codes.4.html).

## Quick start

Framebuffer context initialisation:

```c
#include <flanterm.h>
#include <flanterm_backends/fb.h>

struct flanterm_context *ft_ctx = flanterm_fb_init(
    NULL,
    NULL,
    framebuffer_ptr, width, height, pitch,
    red_mask_size, red_mask_shift,
    green_mask_size, green_mask_shift,
    blue_mask_size, blue_mask_shift,
    NULL,
    NULL, NULL,
    NULL, NULL,
    NULL, NULL,
    NULL, 0, 0, 1,
    0, 0,
    0,
    0
);
```

`framebuffer_ptr`, `width`, `height`, `pitch`, and the six `*_mask_*`
arguments describe the framebuffer; the rest are covered in
[BACKEND_FB.md](BACKEND_FB.md).

Terminal output:

```c
const char msg[] = "Hello world\n";
flanterm_write(ft_ctx, msg, sizeof(msg));
```

## Documentation

- [FRONTEND.md](FRONTEND.md) -- frontend API.
- [BACKEND_FB.md](BACKEND_FB.md) -- framebuffer backend.
