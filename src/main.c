#define KORE_IMPLEMENTATION
#include "kore.h"
#include "matrix.h"
#include "term.h"

int kmain(int argc, char** argv) {
    KORE_UNUSED(argc);
    KORE_UNUSED(argv);

    term_init();

    TermSize fb_size = {0};
    Matrix matrix    = {0};
    matrix_init(&matrix);

    const TimeDuration target_frame_duration = time_from_ms(16);

    while (term_loop()) {
        TimePoint frame_start = time_now();
        TermEvent event       = term_poll_event();

        switch (event.kind) {
        case TERM_EVENT_NONE: {
            if (fb_size.width == 0 || fb_size.height == 0) {
                fb_size = term_size_get();
            }

            if (fb_size.width == 0 || fb_size.height == 0) {
                break;
            }

            matrix_render(&matrix, fb_size, frame_start);
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

    matrix_shutdown(&matrix);

    return 0;
}
