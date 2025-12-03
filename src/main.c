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
} MatrixColumns;

static void
matrix_reset_column(MatrixColumns* columns, u16 x, u16 height, u32* rng_state) {
    if (height == 0) {
        columns->head[x]   = 0;
        columns->speed[x]  = 1;
        columns->length[x] = 6;
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
    columns->speed[x] =
        (u8)(1 + (((r >> 8) % 3) == 0)); // Mostly 1, sometimes 2
    columns->length[x] = (u8)(6 + ((r >> 16) % (max_len - 5)));
}

int kmain(int argc, char** argv) {
    KORE_UNUSED(argc);
    KORE_UNUSED(argv);

    term_init();
    srand((unsigned)time(NULL));

    const char glyphs[]   = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ@#$%&";
    const int glyph_count = (int)(sizeof(glyphs) - 1);

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

                for (u16 i = 0; i < width; ++i) {
                    array_push(columns.head, 0);
                    array_push(columns.speed, 1);
                    array_push(columns.length, 6);
                }

                for (u16 i = 0; i < width; ++i) {
                    matrix_reset_column(&columns, i, height, &rng_state);
                }

                last_fb_dim = fb_size;
            }

            TimeDuration elapsed = time_elapsed(start_time, frame_start);
            u32 frame       = (u32)(time_secs(elapsed) * frames_per_second);
            bool advance    = (frame & 1u) == 0; // Slow the fall rate

            u32 paper_bg    = term_rgb(0, 0, 0);
            u32 ink_bg      = term_rgb(0, 40, 0);
            TermRect screen = {0, 0, width, height};
            term_fb_rect(screen, ' ', ink_bg, paper_bg);

            for (u16 x = 0; x < width; ++x) {
                i16 head = columns.head[x];
                if (advance) {
                    head += columns.speed[x];
                }
                i16 tail = head - (i16)columns.length[x];

                if (tail > (i16)height) {
                    matrix_reset_column(&columns, x, height, &rng_state);
                    continue;
                }

                if (advance) {
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

                    char glyph =
                        glyphs[(x * 37 + y * 17 + frame) % glyph_count];

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

    return 0;
}
