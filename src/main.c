#define KORE_IMPLEMENTATION
#include "kore.h"
#include "term.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef struct {
    Array(i16) head;
    Array(u8) speed;
    Array(u8) length;
    Array(u8) cadence;
} MatrixColumns;

static void
matrix_reset_column(MatrixColumns* columns, u16 x, u16 height, u32* rng_state) {
    if (height == 0) {
        columns->head[x]   = 0;
        columns->speed[x]  = 1;
        columns->length[x] = 6;
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

int kmain(int argc, char** argv) {
    KORE_UNUSED(argc);
    KORE_UNUSED(argv);

    term_init();
    srand((unsigned)time(NULL));

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
    const usize glyph_count       = sizeof(glyphs) / sizeof(glyphs[0]);
    const usize glyph_narrow_count = sizeof(glyphs_narrow) / sizeof(glyphs_narrow[0]);

    TermSize fb_size      = {0};
    TermSize last_fb_dim  = {0};
    MatrixColumns columns = {0};
    u32 rng_state         = (u32)time(NULL) | 1u;

    TimePoint start_time  = time_now();
    const TimeDuration target_frame_duration = time_from_ms(16);
    const f64 frames_per_second              = 60.0;

    while (term_loop()) {
        TimePoint frame_start = time_now();
        TermEvent event       = term_poll_event();

        switch (event.kind) {
        case TERM_EVENT_NONE: {
            if (fb_size.width == 0 || fb_size.height == 0) {
                fb_size = term_size_get();
            }

            u16 width  = fb_size.width;
            u16 height = fb_size.height;
            if (width == 0 || height == 0) {
                break;
            }

            // Resize/reseed columns when the terminal changes
            if (width != last_fb_dim.width || height != last_fb_dim.height) {
                array_free(columns.head);
                array_free(columns.speed);
                array_free(columns.length);
                array_free(columns.cadence);

                for (u16 i = 0; i < width; ++i) {
                    array_push(columns.head, 0);
                    array_push(columns.speed, 1);
                    array_push(columns.length, 6);
                    array_push(columns.cadence, 2);
                }

                for (u16 i = 0; i < width; ++i) {
                    matrix_reset_column(&columns, i, height, &rng_state);
                }

                last_fb_dim = fb_size;
            }

            TimeDuration elapsed = time_elapsed(start_time, frame_start);
            u32 frame       = (u32)(time_secs(elapsed) * frames_per_second);

            u32 paper_bg    = term_rgb(0, 0, 0);
            u32 ink_bg      = term_rgb(0, 40, 0);
            TermRect screen = {0, 0, width, height};
            term_fb_rect(screen, ' ', ink_bg, paper_bg);

            for (u16 x = 0; x < width; ++x) {
                i16 head      = columns.head[x];
                u8 cadence    = columns.cadence[x];
                bool step_now = cadence == 0 ? false : (frame % cadence == 0);
                if (step_now) {
                    head += columns.speed[x];
                }
                i16 tail = head - (i16)columns.length[x];

                if (tail > (i16)height) {
                    matrix_reset_column(&columns, x, height, &rng_state);
                    continue;
                }

                if (step_now) {
                    columns.head[x] = head;
                }

                for (i16 y = head; y > tail; --y) {
                    if (y < 0 || y >= (i16)height) {
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
                            (usize)(x * 11 + y * 23 + frame * 3) %
                            glyph_narrow_count;
                        glyph = glyphs_narrow[fallback_index];
                    }

                    TermRect cell = {x, (u16)y, 1, 1};
                    term_fb_rect(cell, glyph, term_rgb(r, g, b), paper_bg);
                }
            }

            if (width > 12) {
                TermRect hud = {0, 0, width, 1};
                term_fb_rect_colour(
                    hud, term_rgb(0, 100, 0), term_rgb(0, 0, 0));
                term_fb_write(2, 0, "  Matrix rain | press 'q' to quit  ");
            }

            term_fb_present();

            TimeDuration frame_time = time_elapsed(frame_start, time_now());
            if (frame_time < target_frame_duration) {
                TimeDuration remaining = target_frame_duration - frame_time;
                u32 sleep_ms           = (u32)time_duration_to_ms(remaining);
                if (sleep_ms > 0) {
                    time_sleep_ms(sleep_ms);
                }
            }
        } break;
        case TERM_EVENT_KEY:
            prn("Key pressed: %c", event.key);
            if (event.key == 'q') {
                term_done();
            }
            break;
        case TERM_EVENT_RESIZE:
            prn("Terminal resized: %dx%d", event.size.width, event.size.height);
            fb_size = event.size;
            break;
        }
    }

    array_free(columns.head);
    array_free(columns.speed);
    array_free(columns.length);
    array_free(columns.cadence);

    return 0;
}
