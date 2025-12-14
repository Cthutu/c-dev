//------------------------------------------------------------------------------
// Drawing module
//
// Copyright (C)2025 Matt Davies, all rights reserved
//------------------------------------------------------------------------------

#pragma once

//------------------------------------------------------------------------------

#include <frame/gfx.h>
#include <kore/kore.h>

//------------------------------------------------------------------------------

#define COLOUR_RGBA(r, g, b, a)                                                \
    (((u32)(a) << 24) | ((u32)(b) << 16) | ((u32)(g) << 8) | (u32)(r))
#define COLOUR_RGB(r, g, b) COLOUR_RGBA(r, g, b, 255u)

void draw_plot(GfxLayer* layer, int x, int y, u32 colour);
void draw_horz_line(GfxLayer* layer, int x, int y, int length, u32 colour);
void draw_vert_line(GfxLayer* layer, int x, int y, int length, u32 colour);
void draw_line(GfxLayer* layer, int x0, int y0, int x1, int y1, u32 colour);
void draw_rect(
    GfxLayer* layer, int x, int y, int width, int height, u32 colour);
void draw_filled_rect(
    GfxLayer* layer, int x, int y, int width, int height, u32 colour);
void draw_circle(GfxLayer* layer, int x, int y, int radius, u32 colour);
void draw_filled_circle(GfxLayer* layer, int x, int y, int radius, u32 colour);

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// I M P L E M E N T A T I O N
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

#ifdef KORE_IMPLEMENTATION

//------------------------------------------------------------------------------
// Pixel Buffer implementation
//------------------------------------------------------------------------------

static inline int draw_line_outcode(int x, int y, int max_x, int max_y) {
    int code = 0;
    if (x < 0) {
        code |= 1;
    } else if (x > max_x) {
        code |= 2;
    }

    if (y < 0) {
        code |= 4;
    } else if (y > max_y) {
        code |= 8;
    }

    return code;
}

void draw_plot(GfxLayer* layer, int x, int y, u32 colour) {
    if (!layer) {
        return;
    }
    const int width  = gfx_layer_get_width(layer);
    const int height = gfx_layer_get_height(layer);
    if (x < 0 || x >= width || y < 0 || y >= height) {
        return;
    }
    u32* pixels           = gfx_layer_get_pixels(layer);
    pixels[y * width + x] = colour;
}

void draw_horz_line(GfxLayer* layer, int x, int y, int length, u32 colour) {
    if (!layer) {
        return;
    }
    const int width  = gfx_layer_get_width(layer);
    const int height = gfx_layer_get_height(layer);
    if (length == 0 || y < 0 || y >= height) {
        return;
    }

    const int step  = (length > 0) ? 1 : -1;
    int remaining   = (length > 0) ? length : -length;
    int draw_x      = x;
    const int max_x = width - 1;
    u32* pixels     = gfx_layer_get_pixels(layer) + (y * width);
    while (remaining--) {
        if (draw_x >= 0 && draw_x <= max_x) {
            pixels[draw_x] = colour;
        }
        draw_x += step;
    }
}

void draw_vert_line(GfxLayer* layer, int x, int y, int length, u32 colour) {
    if (!layer) {
        return;
    }
    const int width  = gfx_layer_get_width(layer);
    const int height = gfx_layer_get_height(layer);
    if (length == 0 || x < 0 || x >= width) {
        return;
    }

    const int step  = (length > 0) ? 1 : -1;
    int remaining   = (length > 0) ? length : -length;
    int draw_y      = y;
    const int max_y = height - 1;
    u32* pixels     = gfx_layer_get_pixels(layer) + x;
    while (remaining--) {
        if (draw_y >= 0 && draw_y <= max_y) {
            pixels[draw_y * width] = colour;
        }
        draw_y += step;
    }
}

void draw_line(GfxLayer* layer, int x0, int y0, int x1, int y1, u32 colour) {

    if (!layer) {
        return;
    }

    // If the caller provides axis-aligned lines, defer to the helpers that are
    // already inclusive.
    if (y0 == y1) {
        const int len =
            (x1 >= x0) ? (x1 - x0 + 1) : -(x0 - x1 + 1); // inclusive end
        draw_horz_line(layer, x0, y0, len, colour);
        return;
    }
    if (x0 == x1) {
        const int len =
            (y1 >= y0) ? (y1 - y0 + 1) : -(y0 - y1 + 1); // inclusive end
        draw_vert_line(layer, x0, y0, len, colour);
        return;
    }

    const int width  = gfx_layer_get_width(layer);
    const int height = gfx_layer_get_height(layer);
    if (width <= 0 || height <= 0) {
        return;
    }

    const int max_x = width - 1;
    const int max_y = height - 1;

    int out0        = draw_line_outcode(x0, y0, max_x, max_y);
    int out1        = draw_line_outcode(x1, y1, max_x, max_y);

    while (true) {
        if ((out0 | out1) == 0) {
            break; // Fully inside
        }
        if (out0 & out1) {
            return; // Fully outside
        }

        const int outcode = out0 ? out0 : out1;
        int clip_x;
        int clip_y;

        if (outcode & 8) {
            clip_x = x0 + (x1 - x0) * (max_y - y0) / (y1 - y0);
            clip_y = max_y;
        } else if (outcode & 4) {
            clip_x = x0 + (x1 - x0) * (0 - y0) / (y1 - y0);
            clip_y = 0;
        } else if (outcode & 2) {
            clip_y = y0 + (y1 - y0) * (max_x - x0) / (x1 - x0);
            clip_x = max_x;
        } else {
            clip_y = y0 + (y1 - y0) * (0 - x0) / (x1 - x0);
            clip_x = 0;
        }

        if (outcode == out0) {
            x0   = clip_x;
            y0   = clip_y;
            out0 = draw_line_outcode(x0, y0, max_x, max_y);
        } else {
            x1   = clip_x;
            y1   = clip_y;
            out1 = draw_line_outcode(x1, y1, max_x, max_y);
        }
    }

    int dx      = (x1 > x0) ? (x1 - x0) : (x0 - x1);
    int sx      = (x0 < x1) ? 1 : -1;
    int dy      = (y1 > y0) ? -(y1 - y0) : -(y0 - y1);
    int sy      = (y0 < y1) ? 1 : -1;
    int err     = dx + dy;

    u32* pixels = gfx_layer_get_pixels(layer);
    const int w = width;

    for (;;) {
        pixels[y0 * w + x0] = colour; // inclusive endpoint

        if (x0 == x1 && y0 == y1) {
            break;
        }

        const int e2 = err << 1;
        if (e2 >= dy) {
            err += dy;
            x0 += sx;
        }
        if (e2 <= dx) {
            err += dx;
            y0 += sy;
        }
    }
}

void draw_rect(
    GfxLayer* layer, int x, int y, int width, int height, u32 colour) {
    draw_line(layer, x, y, x + width, y, colour);
    draw_line(layer, x + width - 1, y, x + width - 1, y + height, colour);
    draw_line(
        layer, x + width - 1, y + height - 1, x - 1, y + height - 1, colour);
    draw_line(layer, x, y + height, x, y, colour);
}

void draw_filled_rect(
    GfxLayer* layer, int x, int y, int width, int height, u32 colour) {

    const int w = gfx_layer_get_width(layer);
    const int h = gfx_layer_get_height(layer);

    //
    // Clip area to bounds
    //
    if (x < 0) {
        width += x;
        x = 0;
    }
    if (y < 0) {
        height += y;
        y = 0;
    }
    width  = KORE_CLAMP(width, 0, w - x);
    height = KORE_CLAMP(height, 0, h - y);
    if (width <= 0 || height <= 0) {
        return;
    }

    //
    // Draw filled rectangle
    //

    u32* pixels = gfx_layer_get_pixels(layer);
    for (int row = 0; row < height; ++row) {
        u32* row_start = pixels + (y + row) * w + x;
        for (int col = 0; col < width; ++col) {
            row_start[col] = colour;
        }
    }
}

void draw_circle(GfxLayer* layer, int x, int y, int radius, u32 colour) {
    if (!layer || radius < 0) {
        return;
    }
    if (radius == 0) {
        draw_plot(layer, x, y, colour);
        return;
    }

    const int w = gfx_layer_get_width(layer);
    const int h = gfx_layer_get_height(layer);
    if (w <= 0 || h <= 0) {
        return;
    }

    const int max_x = w - 1;
    const int max_y = h - 1;
    if (x + radius < 0 || x - radius > max_x || y + radius < 0 ||
        y - radius > max_y) {
        return;
    }

    u32* pixels = gfx_layer_get_pixels(layer);

    int dx      = radius;
    int dy      = 0;
    int err     = 1 - dx;

    while (dx >= dy) {
        const int px1 = x + dx;
        const int px2 = x - dx;
        const int px3 = x + dy;
        const int px4 = x - dy;

        const int py1 = y + dy;
        const int py2 = y - dy;
        const int py3 = y + dx;
        const int py4 = y - dx;

        if (px1 >= 0 && px1 <= max_x) {
            if (py1 >= 0 && py1 <= max_y) {
                pixels[py1 * w + px1] = colour;
            }
            if (py2 >= 0 && py2 <= max_y) {
                pixels[py2 * w + px1] = colour;
            }
        }
        if (px2 >= 0 && px2 <= max_x) {
            if (py1 >= 0 && py1 <= max_y) {
                pixels[py1 * w + px2] = colour;
            }
            if (py2 >= 0 && py2 <= max_y) {
                pixels[py2 * w + px2] = colour;
            }
        }
        if (px3 >= 0 && px3 <= max_x) {
            if (py3 >= 0 && py3 <= max_y) {
                pixels[py3 * w + px3] = colour;
            }
            if (py4 >= 0 && py4 <= max_y) {
                pixels[py4 * w + px3] = colour;
            }
        }
        if (px4 >= 0 && px4 <= max_x) {
            if (py3 >= 0 && py3 <= max_y) {
                pixels[py3 * w + px4] = colour;
            }
            if (py4 >= 0 && py4 <= max_y) {
                pixels[py4 * w + px4] = colour;
            }
        }

        ++dy;
        if (err <= 0) {
            err += (dy << 1) + 1;
        } else {
            --dx;
            err += ((dy - dx) << 1) + 1;
        }
    }
}

void draw_filled_circle(GfxLayer* layer, int x, int y, int radius, u32 colour) {
    if (!layer || radius < 0) {
        return;
    }
    if (radius == 0) {
        draw_plot(layer, x, y, colour);
        return;
    }

    const int w = gfx_layer_get_width(layer);
    const int h = gfx_layer_get_height(layer);
    if (w <= 0 || h <= 0) {
        return;
    }

    const int max_x = w - 1;
    const int max_y = h - 1;
    if (x + radius < 0 || x - radius > max_x || y + radius < 0 ||
        y - radius > max_y) {
        return;
    }

    u32* pixels = gfx_layer_get_pixels(layer);

    int dx      = radius;
    int dy      = 0;
    int err     = 1 - dx;

    while (dx >= dy) {
        int left  = x - dx;
        int right = x + dx;

        int row = y + dy;
        if (row >= 0 && row <= max_y) {
            int lx = (left < 0) ? 0 : left;
            int rx = (right > max_x) ? max_x : right;
            if (lx <= rx) {
                u32* p   = pixels + row * w + lx;
                int cnt  = rx - lx + 1;
                while (cnt--) {
                    *p++ = colour;
                }
            }
        }
        if (dy != 0) {
            row = y - dy;
            if (row >= 0 && row <= max_y) {
                int lx = (left < 0) ? 0 : left;
                int rx = (right > max_x) ? max_x : right;
                if (lx <= rx) {
                    u32* p   = pixels + row * w + lx;
                    int cnt  = rx - lx + 1;
                    while (cnt--) {
                        *p++ = colour;
                    }
                }
            }
        }

        if (dx != dy) {
            left  = x - dy;
            right = x + dy;

            row = y + dx;
            if (row >= 0 && row <= max_y) {
                int lx = (left < 0) ? 0 : left;
                int rx = (right > max_x) ? max_x : right;
                if (lx <= rx) {
                    u32* p   = pixels + row * w + lx;
                    int cnt  = rx - lx + 1;
                    while (cnt--) {
                        *p++ = colour;
                    }
                }
            }
            row = y - dx;
            if (row >= 0 && row <= max_y) {
                int lx = (left < 0) ? 0 : left;
                int rx = (right > max_x) ? max_x : right;
                if (lx <= rx) {
                    u32* p   = pixels + row * w + lx;
                    int cnt  = rx - lx + 1;
                    while (cnt--) {
                        *p++ = colour;
                    }
                }
            }
        }

        ++dy;
        if (err <= 0) {
            err += (dy << 1) + 1;
        } else {
            --dx;
            err += ((dy - dx) << 1) + 1;
        }
    }
}

//------------------------------------------------------------------------------
// Colour helpers
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

#endif // KORE_IMPLEMENTATION
