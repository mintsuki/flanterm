/*
 * Copyright (c) 2014-2020 Hayaki Saito
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef SIXEL_H
#define SIXEL_H

#define SIXEL_PALETTE_MAX          256
#define SIXEL_WIDTH_LIMIT          1000000
#define SIXEL_HEIGHT_LIMIT         1000000

/* return value */
typedef int SIXELSTATUS;
#define SIXEL_OK                   0x0000                          /* succeeded */
#define SIXEL_FALSE                0x1000                          /* failed */

#define SIXEL_RUNTIME_ERROR        (SIXEL_FALSE         | 0x0100)  /* runtime error */
#define SIXEL_BAD_INPUT            (SIXEL_RUNTIME_ERROR | 0x0003)  /* bad input detected */
#define SIXEL_BAD_INTEGER_OVERFLOW (SIXEL_RUNTIME_ERROR | 0x0004)  /* integer overflow */
#define SIXEL_BAD_ALLOCATION       (SIXEL_RUNTIME_ERROR | 0x0001)  /* malloc() failed */

#define SIXEL_SUCCEEDED(status) (((status) & 0x1000) == 0)
#define SIXEL_FAILED(status)    (((status) & 0x1000) != 0)

#ifdef __cplusplus
extern "C" {
#endif

SIXELSTATUS
sixel_decode_raw(
    unsigned char       /* in */  *p,           /* sixel bytes */
    int                 /* in */  len,          /* size of sixel bytes */
    unsigned char       /* out */ **pixels,     /* decoded pixels */
    int                 /* out */ *pwidth,      /* image width */
    int                 /* out */ *pheight,     /* image height */
    unsigned char       /* out */ **palette,    /* ARGB palette */
    int                 /* out */ *ncolors);    /* palette size (<= 256) */

#ifdef __cplusplus
}
#endif

#endif  /* SIXEL_H */
