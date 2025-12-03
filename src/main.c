#define KORE_IMPLEMENTATION
#include "kore.h"
#include "term.h"
#include <stdio.h>

int kmain(int argc, char** argv) {
    KORE_UNUSED(argc);
    KORE_UNUSED(argv);

    term_init();

    TermSize fb_size = term_size_get();
    TimePoint start_time = time_now();
    const TimeDuration target_frame_duration = time_from_ms(16);
    const f64 frames_per_second              = 60.0;

    while (term_loop()) {
        TimePoint frame_start = time_now();
        TermEvent event = term_poll_event();

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

            u8 hue_base = (u8)(frame << 3);
            u32 ink      = term_rgb(20, 20, 30);

            // Fast vertical gradient sweep
            for (u16 y = 0; y < height; ++y) {
                u8 band       = (u8)(hue_base + (y << 2));
                u8 r          = (u8)(band);
                u8 g          = (u8)(255 - band);
                u8 b          = (u8)((band ^ (frame >> 1)) | 0x30);
                u32 paper     = term_rgb(r, g, b);
                TermRect row  = {0, y, width, 1};
                term_fb_rect(row, ' ', ink, paper);
            }

            // Moving beams
            u16 v_x      = (u16)(frame % width);
            u16 v_width  = (width > 8) ? 8 : width;
            if (v_x + v_width > width) {
                v_width = (u16)(width - v_x);
            }
            TermRect beam_v = {v_x, 0, v_width, height};
            term_fb_rect_colour(
                beam_v, term_rgb(245, 250, 255), term_rgb(40, 140, 255));

            u16 h_y       = (u16)((frame * 3u) % height);
            u16 h_height  = (height > 4) ? 4 : height;
            if (h_y + h_height > height) {
                h_height = (u16)(height - h_y);
            }
            TermRect beam_h = {0, h_y, width, h_height};
            term_fb_rect_colour(
                beam_h, term_rgb(255, 255, 255), term_rgb(255, 90, 160));

            // Sparkle trail
            for (u16 x = 0; x < width; x += 2) {
                u16 y        = (u16)((x + frame) % height);
                TermRect dot = {x, y, 1, 1};
                term_fb_rect_char(dot, ((frame + x) & 4u) ? '*' : '+');
            }

            if (width > 8) {
                TermRect hud = {0, 0, width, 1};
                term_fb_rect_colour(
                    hud, term_rgb(30, 30, 45), term_rgb(235, 235, 235));
                term_fb_write(
                    2, 0, "term_fb animation — press 'q' to quit");
            }

            term_fb_present();
            frame++;
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

    return 0;
}
