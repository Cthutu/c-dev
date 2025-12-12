//------------------------------------------------------------------------------
// Drawing module
//
// Copyright (C)2025 Matt Davies, all rights reserved
//------------------------------------------------------------------------------

#pragma once

//------------------------------------------------------------------------------

#include <kore/kore.h>

//------------------------------------------------------------------------------

#define COLOUR_RGBA(r, g, b, a)                                                \
    (((u32)(a) << 24) | ((u32)(b) << 16) | ((u32)(g) << 8) | (u32)(r))
#define COLOUR_RGB(r, g, b) COLOUR_RGBA(r, g, b, 255u)

typedef enum {
    DRAW_CONTEXT_TYPE_PIXEL_BUFFER,
} DrawContextType;

typedef struct DrawContext_t {
    DrawContextType type;
    void (*draw_horz_line_func)(
        struct DrawContext_t* ctx, int x, int y, int length, u32 colour);
    void (*draw_vert_line_func)(
        struct DrawContext_t* ctx, int x, int y, int length, u32 colour);
    void (*draw_line_func)(
        struct DrawContext_t* ctx, int x0, int y0, int x1, int y1, u32 colour);
    void (*draw_rect_func)(struct DrawContext_t* ctx,
                           int x,
                           int y,
                           int width,
                           int height,
                           u32 colour);
    void (*draw_filled_rect_func)(struct DrawContext_t* ctx,
                                  int x,
                                  int y,
                                  int width,
                                  int height,
                                  u32 colour);
    void (*draw_circle_func)(
        struct DrawContext_t* ctx, int x, int y, int radius, u32 colour);
    void (*draw_filled_circle_func)(
        struct DrawContext_t* ctx, int x, int y, int radius, u32 colour);
} DrawContext;

typedef struct {
    DrawContext ctx;
    int width;
    int height;
    u32* pixel_buffer;
} PixelBufferDrawContext;

PixelBufferDrawContext
draw_create_pixel_context(int width, int height, u32* pixel_buffer);

void draw_horz_line(DrawContext* ctx, int x, int y, int length, u32 colour);
void draw_vert_line(DrawContext* ctx, int x, int y, int length, u32 colour);
void draw_line(DrawContext* ctx, int x0, int y0, int x1, int y1, u32 colour);
void draw_rect(
    DrawContext* ctx, int x, int y, int width, int height, u32 colour);
void draw_filled_rect(
    DrawContext* ctx, int x, int y, int width, int height, u32 colour);
void draw_circle(DrawContext* ctx, int x, int y, int radius, u32 colour);
void draw_filled_circle(DrawContext* ctx, int x, int y, int radius, u32 colour);

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// I M P L E M E N T A T I O N
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

#ifdef KORE_IMPLEMENTATION

//------------------------------------------------------------------------------
// Pixel Buffer implementation
//------------------------------------------------------------------------------

void pixel_buffer_draw_horz_line(
    DrawContext* ctx_void, int x, int y, int length, u32 colour) {
    PixelBufferDrawContext* ctx = (PixelBufferDrawContext*)ctx_void;

    if (length == 0 || y < 0 || y >= ctx->height) {
        return;
    }

    const int step     = (length > 0) ? 1 : -1;
    int remaining      = (length > 0) ? length : -length;
    int draw_x         = x;
    const int max_x    = ctx->width - 1;
    u32* row_start     = ctx->pixel_buffer + (y * ctx->width);
    while (remaining--) {
        if (draw_x >= 0 && draw_x <= max_x) {
            row_start[draw_x] = colour;
        }
        draw_x += step;
    }
}

void pixel_buffer_draw_vert_line(
    DrawContext* ctx_void, int x, int y, int length, u32 colour) {
    PixelBufferDrawContext* ctx = (PixelBufferDrawContext*)ctx_void;

    if (length == 0 || x < 0 || x >= ctx->width) {
        return;
    }

    const int step     = (length > 0) ? 1 : -1;
    int remaining      = (length > 0) ? length : -length;
    int draw_y         = y;
    const int max_y    = ctx->height - 1;
    u32* col_start     = ctx->pixel_buffer + x;
    while (remaining--) {
        if (draw_y >= 0 && draw_y <= max_y) {
            col_start[draw_y * ctx->width] = colour;
        }
        draw_y += step;
    }
}

// TODO: Fix this and add clipping against context width and height
void pixel_buffer_draw_line(
    DrawContext* ctx_void, int x0, int y0, int x1, int y1, u32 colour) {

    PixelBufferDrawContext* ctx = (PixelBufferDrawContext*)ctx_void;
    const int width             = ctx->width;
    const int height            = ctx->height;

    int dx = x1 - x0;
    int dy = y1 - y0;

    if (dx == 0) {
        // Vertical line (exclusive of end point)
        pixel_buffer_draw_vert_line(ctx_void, x0, y0, dy, colour);
        return;
    }
    if (dy == 0) {
        // Horizontal line (exclusive of end point)
        pixel_buffer_draw_horz_line(ctx_void, x0, y0, dx, colour);
        return;
    }

    int step_x = (dx > 0) ? 1 : -1;
    int step_y = (dy > 0) ? 1 : -1;
    dx         = (dx > 0) ? dx : -dx;
    dy         = (dy > 0) ? dy : -dy;

    int x = x0;
    int y = y0;

    if (dx >= dy) {
        int err = dx / 2;
        for (int i = 0; i < dx; ++i) {
            if (x >= 0 && x < width && y >= 0 && y < height) {
                ctx->pixel_buffer[y * width + x] = colour;
            }
            x += step_x;
            err -= dy;
            if (err < 0) {
                y += step_y;
                err += dx;
            }
        }
    } else {
        int err = dy / 2;
        for (int i = 0; i < dy; ++i) {
            if (x >= 0 && x < width && y >= 0 && y < height) {
                ctx->pixel_buffer[y * width + x] = colour;
            }
            y += step_y;
            err -= dx;
            if (err < 0) {
                x += step_x;
                err += dy;
            }
        }
    }
}

void pixel_buffer_draw_rect(
    DrawContext* ctx_void, int x, int y, int width, int height, u32 colour) {
    KORE_UNUSED(ctx_void);
    KORE_UNUSED(x);
    KORE_UNUSED(y);
    KORE_UNUSED(width);
    KORE_UNUSED(height);
    KORE_UNUSED(colour);
}

void pixel_buffer_draw_filled_rect(
    DrawContext* ctx_void, int x, int y, int width, int height, u32 colour) {
    KORE_UNUSED(ctx_void);
    KORE_UNUSED(x);
    KORE_UNUSED(y);
    KORE_UNUSED(width);
    KORE_UNUSED(height);
    KORE_UNUSED(colour);
}

void pixel_buffer_draw_circle(
    DrawContext* ctx_void, int x, int y, int radius, u32 colour) {
    KORE_UNUSED(ctx_void);
    KORE_UNUSED(x);
    KORE_UNUSED(y);
    KORE_UNUSED(radius);
    KORE_UNUSED(colour);
}

void pixel_buffer_draw_filled_circle(
    DrawContext* ctx_void, int x, int y, int radius, u32 colour) {
    KORE_UNUSED(ctx_void);
    KORE_UNUSED(x);
    KORE_UNUSED(y);
    KORE_UNUSED(radius);
    KORE_UNUSED(colour);
}

//------------------------------------------------------------------------------
// Draw API
//------------------------------------------------------------------------------

PixelBufferDrawContext
draw_create_pixel_context(int width, int height, u32* pixel_buffer) {
    PixelBufferDrawContext ctx;
    ctx.ctx.type                    = DRAW_CONTEXT_TYPE_PIXEL_BUFFER;
    ctx.ctx.draw_horz_line_func     = pixel_buffer_draw_horz_line;
    ctx.ctx.draw_vert_line_func     = pixel_buffer_draw_vert_line;
    ctx.ctx.draw_line_func          = pixel_buffer_draw_line;
    ctx.ctx.draw_rect_func          = pixel_buffer_draw_rect;
    ctx.ctx.draw_filled_rect_func   = pixel_buffer_draw_filled_rect;
    ctx.ctx.draw_circle_func        = pixel_buffer_draw_circle;
    ctx.ctx.draw_filled_circle_func = pixel_buffer_draw_filled_circle;
    ctx.width                       = width;
    ctx.height                      = height;
    ctx.pixel_buffer                = pixel_buffer;
    return ctx;
}

void draw_horz_line(DrawContext* ctx, int x, int y, int length, u32 colour) {
    ctx->draw_horz_line_func(ctx, x, y, length, colour);
}

void draw_vert_line(DrawContext* ctx, int x, int y, int length, u32 colour) {
    ctx->draw_vert_line_func(ctx, x, y, length, colour);
}

void draw_line(DrawContext* ctx, int x0, int y0, int x1, int y1, u32 colour) {
    ctx->draw_line_func(ctx, x0, y0, x1, y1, colour);
}

void draw_rect(
    DrawContext* ctx, int x, int y, int width, int height, u32 colour) {
    ctx->draw_rect_func(ctx, x, y, width, height, colour);
}

void draw_filled_rect(
    DrawContext* ctx, int x, int y, int width, int height, u32 colour) {
    ctx->draw_filled_rect_func(ctx, x, y, width, height, colour);
}

void draw_circle(DrawContext* ctx, int x, int y, int radius, u32 colour) {
    ctx->draw_circle_func(ctx, x, y, radius, colour);
}

void draw_filled_circle(
    DrawContext* ctx, int x, int y, int radius, u32 colour) {
    ctx->draw_filled_circle_func(ctx, x, y, radius, colour);
}

//------------------------------------------------------------------------------
// Colour helpers
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

#endif // KORE_IMPLEMENTATION
