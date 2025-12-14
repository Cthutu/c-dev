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
        pixels[y0 * w + x0] = colour;

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
    KORE_UNUSED(layer);
    KORE_UNUSED(x);
    KORE_UNUSED(y);
    KORE_UNUSED(width);
    KORE_UNUSED(height);
    KORE_UNUSED(colour);
}

void draw_filled_rect(
    GfxLayer* layer, int x, int y, int width, int height, u32 colour) {
    KORE_UNUSED(layer);
    KORE_UNUSED(x);
    KORE_UNUSED(y);
    KORE_UNUSED(width);
    KORE_UNUSED(height);
    KORE_UNUSED(colour);
}

void draw_circle(GfxLayer* layer, int x, int y, int radius, u32 colour) {
    KORE_UNUSED(layer);
    KORE_UNUSED(x);
    KORE_UNUSED(y);
    KORE_UNUSED(radius);
    KORE_UNUSED(colour);
}

void draw_filled_circle(
    GfxLayer* layer, int x, int y, int radius, u32 colour) {
    KORE_UNUSED(layer);
    KORE_UNUSED(x);
    KORE_UNUSED(y);
    KORE_UNUSED(radius);
    KORE_UNUSED(colour);
}

//------------------------------------------------------------------------------
// Colour helpers
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

#endif // KORE_IMPLEMENTATION
