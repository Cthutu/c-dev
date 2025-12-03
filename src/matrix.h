//------------------------------------------------------------------------------
// Matrix animation module
//
// Copyright (C)2025 Matt Davies, all rights reserved
//------------------------------------------------------------------------------

#pragma once

//------------------------------------------------------------------------------

#include "kore.h"
#include "term.h"

//------------------------------------------------------------------------------

typedef struct {
    Array(i16) head;
    Array(u8) speed;
    Array(u8) length;
    Array(u8) cadence;
} MatrixColumns;

typedef struct {
    MatrixColumns columns;
    TermSize last_fb_dim;
    u32 rng_state;
    TimePoint start_time;
} Matrix;

void matrix_init(Matrix* matrix);
void matrix_render(Matrix* matrix, TermSize fb_size, TimePoint frame_start);
void matrix_shutdown(Matrix* matrix);

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// I M P L E M E N T A T I O N
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

#ifdef KORE_IMPLEMENTATION

#    include <time.h>

//------------------------------------------------------------------------------

static void
matrix_reset_column(MatrixColumns* columns, u16 x, u16 height, u32* rng_state) {
    if (height == 0) {
        columns->head[x]    = 0;
        columns->speed[x]   = 1;
        columns->length[x]  = 6;
        columns->cadence[x] = 2;
        return;
    }

    // Xorshift for lightweight pseudo-randomness
    *rng_state ^= *rng_state << 13;
    *rng_state ^= *rng_state >> 17;
    *rng_state ^= *rng_state << 5;
    u32 r       = *rng_state;

    u16 max_len = height > 12 ? (height / 2) : height;
    if (max_len < 6) {
        max_len = 6;
    }

    columns->head[x] = (i16) - (r % (height + 1));
    // Weight towards slow speeds: mostly 1, occasional 2, rare 3
    u8 speed = 1;
    if (((r >> 10) & 7u) == 0u) {
        speed = 3;
    } else if (((r >> 8) & 3u) == 0u) {
        speed = 2;
    }
    columns->speed[x]   = speed;
    columns->length[x]  = (u8)(6 + ((r >> 16) % (max_len - 5)));
    columns->cadence[x] = (u8)(2 + ((r >> 24) % 3)); // step every 2-4 frames
}

void matrix_init(Matrix* matrix) {
    matrix->columns    = (MatrixColumns){0};
    matrix->last_fb_dim = (TermSize){0};
    matrix->rng_state   = (u32)time(NULL) | 1u;
    matrix->start_time  = time_now();
}

void matrix_render(Matrix* matrix, TermSize fb_size, TimePoint frame_start) {
    if (fb_size.width == 0 || fb_size.height == 0) {
        return;
    }

    // Mix of digits, Latin letters, symbols, and Katakana reminiscent of the film
    static const u32 glyphs[] = {
        '0',    '1',    '2',    '3',    '4',    '5',    '6',    '7',    '8',
        '9',    'A',    'B',    'C',    'D',    'E',    'F',    'G',    'H',
        'I',    'J',    'K',    'L',    'M',    'N',    'O',    'P',    'Q',
        'R',    'S',    'T',    'U',    'V',    'W',    'X',    'Y',    'Z',
        '@',    '#',    '$',    '%',    '&',    '+',    '*',    '=',    '?',
        0x30A2, 0x30A4, 0x30A8, 0x30AA, 0x30AB, 0x30AD, 0x30AF, 0x30B1, 0x30B3,
        0x30B5, 0x30B7, 0x30B9, 0x30BB, 0x30BD, 0x30BF, 0x30C1, 0x30C4, 0x30C6,
        0x30C8, 0x30CA, 0x30CB, 0x30CC, 0x30CD, 0x30CE, 0x30CF, 0x30D2, 0x30D5,
        0x30D8, 0x30DB, 0x30DE, 0x30DF, 0x30E0, 0x30E1, 0x30E2, 0x30E4, 0x30E6,
        0x30E8, 0x30E9, 0x30EA, 0x30EB, 0x30EC, 0x30ED, 0x30EF, 0x30F3};
    static const u32 glyphs_narrow[] = {
        '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C',
        'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
        'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', '@', '#', '$',
        '%', '&', '+', '*', '=', '?'};
    const usize glyph_count = sizeof(glyphs) / sizeof(glyphs[0]);
    const usize glyph_narrow_count = sizeof(glyphs_narrow) / sizeof(glyphs_narrow[0]);

    // Resize/reseed columns when the terminal changes
    if (fb_size.width != matrix->last_fb_dim.width ||
        fb_size.height != matrix->last_fb_dim.height) {
        array_free(matrix->columns.head);
        array_free(matrix->columns.speed);
        array_free(matrix->columns.length);
        array_free(matrix->columns.cadence);

        for (u16 i = 0; i < fb_size.width; ++i) {
            array_push(matrix->columns.head, 0);
            array_push(matrix->columns.speed, 1);
            array_push(matrix->columns.length, 6);
            array_push(matrix->columns.cadence, 2);
        }

        for (u16 i = 0; i < fb_size.width; ++i) {
            matrix_reset_column(&matrix->columns, i, fb_size.height, &matrix->rng_state);
        }

        matrix->last_fb_dim = fb_size;
    }

    TimeDuration elapsed = time_elapsed(matrix->start_time, frame_start);
    u32 frame            = (u32)(time_secs(elapsed) * 60.0);

    u32 paper_bg    = term_rgb(0, 0, 0);
    u32 ink_bg      = term_rgb(0, 40, 0);
    TermRect screen = (TermRect){0, 0, fb_size.width, fb_size.height};
    term_fb_rect(screen, ' ', ink_bg, paper_bg);

    for (u16 x = 0; x < fb_size.width; ++x) {
        i16 head      = matrix->columns.head[x];
        u8 cadence    = matrix->columns.cadence[x];
        bool step_now = cadence == 0 ? false : (frame % cadence == 0);
        if (step_now) {
            head += matrix->columns.speed[x];
        }
        i16 tail = head - (i16)matrix->columns.length[x];

        if (tail > (i16)fb_size.height) {
            matrix_reset_column(&matrix->columns, x, fb_size.height, &matrix->rng_state);
            continue;
        }

        if (step_now) {
            matrix->columns.head[x] = head;
        }

        for (i16 y = head; y > tail; --y) {
            if (y < 0 || y >= (i16)fb_size.height) {
                continue;
            }

            int dist   = head - y;
            int g_base = 220 - dist * 28;
            if (g_base < 70) {
                g_base = 70;
            }
            if (dist == 0) {
                g_base = 255; // bright head
            }

            u8 g = (u8)g_base;
            u8 r = (u8)(g / 6);
            u8 b = (u8)(g / 10);

            usize glyph_index =
                (usize)(x * 37 + y * 17 + frame) % glyph_count;
            u32 glyph = glyphs[glyph_index];
            if (wcwidth(glyph) != 1) {
                usize fallback_index =
                    (usize)(x * 11 + y * 23 + frame * 3) % glyph_narrow_count;
                glyph = glyphs_narrow[fallback_index];
            }

            TermRect cell = (TermRect){x, (u16)y, 1, 1};
            term_fb_rect(cell, glyph, term_rgb(r, g, b), paper_bg);
        }
    }

    if (fb_size.width > 12) {
        TermRect hud = (TermRect){0, 0, fb_size.width, 1};
        term_fb_rect_colour(hud, term_rgb(0, 100, 0), term_rgb(0, 0, 0));
        term_fb_write(2, 0, "  Matrix rain | press 'q' to quit  ");
    }
}

void matrix_shutdown(Matrix* matrix) {
    array_free(matrix->columns.head);
    array_free(matrix->columns.speed);
    array_free(matrix->columns.length);
    array_free(matrix->columns.cadence);
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

#endif // KORE_IMPLEMENTATION
