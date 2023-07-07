# Flanterm

Flanterm is a fast and reasonably complete terminal emulator with support for
multiple output backends. Included is a fast framebuffer backend.

### Quick usage

To quickly set up and use a framebuffer Flanterm instance, it is possible to
use the `flanterm_fb_simple_init()` function as such:
```c
#include <flanterm/flanterm.h>
#include <flanterm/backends/fb.h>

struct flanterm_context *ft_ctx = flanterm_fb_simple_init(
    framebuffer_ptr, framebuffer_width, framebuffer_height, framebuffer_pitch
);
```
Where `framebuffer_{ptr,width,height,pitch}` represent the corresponding info
about the framebuffer to use for this given instance.

To then print to the terminal instance, simply use the `flanterm_write()`
function on the given instance. For example:
```c
#include <flanterm/flanterm.h>

const char msg[] = "Hello world\n";

flanterm_write(ft_ctx, msg, sizeof(msg));
```
