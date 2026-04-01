//------------------------------------------------------------------------------
// Terminal module
//
// Copyright (C)2025 Matt Davies, all rights reserved
//------------------------------------------------------------------------------
//> use: core

#pragma once

//------------------------------------------------------------------------------

#include <core/core.h>

//------------------------------------------------------------------------------
// Terminal information
//------------------------------------------------------------------------------

typedef struct {
    u16 width;
    u16 height;
} TermSize;

TermSize term_size_get(void);

//------------------------------------------------------------------------------
// Terminal interaction API
//------------------------------------------------------------------------------

typedef enum {
    TERM_EVENT_NONE,
    TERM_EVENT_KEY,
    TERM_EVENT_RESIZE,
} TermEventKind;

typedef struct {
    TermEventKind kind;
    union {
        char     key;
        TermSize size;
    };
} TermEvent;

typedef struct Term {
    TermSize size;
    Array(TermEvent) event_queue;
    bool initialised;
    bool running;
} Term;

void      term_init();
void      term_done();
bool      term_loop();
TermEvent term_poll_event();

void term_cursor_show();
void term_cursor_hide();

void term_cursor_goto(int x, int y);
void term_cursor_move(int dx, int dy);
void term_cursor_up(int delta);
void term_cursor_down(int delta);
void term_cursor_right(int delta);
void term_cursor_left(int delta);
void term_cursor_home(void);

//------------------------------------------------------------------------------
// Terminal frame buffer API
//------------------------------------------------------------------------------

typedef struct {
    u16 x;
    u16 y;
    u16 width;
    u16 height;
} TermRect;

void term_fb_cls(u32 ink, u32 paper);
void term_fb_clip_rect(TermRect  rect,
                       TermRect* out_clipped_rect,
                       TermRect* out_local_rect);

//
// Colouring
//

u32 term_rgb(u8 r, u8 g, u8 b);
u32 term_rgba(u8 r, u8 g, u8 b, u8 a);
u32 term_blend(u32 dest, u32 src, f32 alpha);

// Individual layers
void term_fb_rect_ink(TermRect rect, u32 colour);
void term_fb_rect_paper(TermRect rect, u32 colour);
void term_fb_rect_colour(TermRect rect, u32 ink, u32 paper);
void term_fb_rect_char(TermRect rect, u32 ch);

// Character painting
void term_fb_rect(TermRect rect, u32 ch, u32 ink, u32 paper);

// Writing strings
void term_fb_write(u16 x, u16 y, cstr string);
void term_fb_formatv(u16 x, u16 y, cstr fmt, va_list args);
void term_fb_format(u16 x, u16 y, cstr fmt, ...);

//
// Presentation
//

void term_fb_present();

//------------------------------------------------------------------------------
// Terminal information dumping
//------------------------------------------------------------------------------

void dump_term_size();

//------------------------------------------------------------------------------
// Wide-char width calculation
//------------------------------------------------------------------------------

int wcwidth(u32 ucs);

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
