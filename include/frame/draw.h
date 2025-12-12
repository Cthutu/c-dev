//------------------------------------------------------------------------------
// Drawing module
//
// Copyright (C)2025 Matt Davies, all rights reserved
//------------------------------------------------------------------------------

#pragma once

//------------------------------------------------------------------------------

#include <kore/kore.h>

//------------------------------------------------------------------------------

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

u32 colour_rgba(u8 r, u8 g, u8 b, u8 a);
u32 colour_rgb(u8 r, u8 g, u8 b);

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

    // Clip coords
    if (y < 0 || y >= ctx->height) {
        return;
    }
    if (x < 0) {
        length += x;
        x = 0;
    }
    if (x + length > ctx->width) {
        length = ctx->width - x;
        ;
    }
    if (length <= 0) {
        return;
    }

    u32* row_start = ctx->pixel_buffer + (y * ctx->width) + x;
    for (int i = 0; i < length; ++i) {
        row_start[i] = colour;
    }
}

void pixel_buffer_draw_vert_line(
    DrawContext* ctx_void, int x, int y, int length, u32 colour) {
    PixelBufferDrawContext* ctx = (PixelBufferDrawContext*)ctx_void;

    // Clip coords
    if (x < 0 || x >= ctx->width) {
        return;
    }
    if (y < 0) {
        length += y;
        y = 0;
    }
    if (y + length > ctx->height) {
        length = ctx->height - y;
    }
    if (length <= 0) {
        return;
    }

    u32* col_start = ctx->pixel_buffer + (y * ctx->width) + x;
    for (int i = 0; i < length; ++i) {
        col_start[i * ctx->width] = colour;
    }
}

void pixel_buffer_draw_line(
    DrawContext* ctx_void, int x0, int y0, int x1, int y1, u32 colour) {
    KORE_UNUSED(ctx_void);
    KORE_UNUSED(x0);
    KORE_UNUSED(y0);
    KORE_UNUSED(x1);
    KORE_UNUSED(y1);
    KORE_UNUSED(colour);
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

u32 colour_rgba(u8 r, u8 g, u8 b, u8 a) {
    // Convert to ABGR format
    return ((u32)a << 24) | ((u32)b << 16) | ((u32)g << 8) | (u32)r;
}

u32 colour_rgb(u8 r, u8 g, u8 b) { return colour_rgba(r, g, b, 255); }

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

#endif // KORE_IMPLEMENTATION
