#define KORE_IMPLEMENTATION
#include <frame/draw.h>
#include <frame/frame.h>
#include <kore/kore.h>
#include <math.h>

// Simple plasma-like animation with orbiting bands and a subtle pixel grid.
static void render_demo(u32* pixels, int w, int h, f64 time_s) {
    KORE_UNUSED(time_s);

    PixelBufferDrawContext draw_ctx = draw_create_pixel_context(w, h, pixels);
    DrawContext* ctx                = &draw_ctx.ctx;

    draw_horz_line(ctx, 0, 0, w, colour_rgb(255, 0, 0));
    draw_horz_line(ctx, 0, h - 1, w, colour_rgb(255, 0, 0));
    draw_vert_line(ctx, 0, 0, h, colour_rgb(255, 0, 0));
    draw_vert_line(ctx, w - 1, 0, h, colour_rgb(255, 0, 0));
}

int kmain(int argc, char** argv) {
    KORE_UNUSED(argc);
    KORE_UNUSED(argv);

    Frame main_frame  = frame_open(800, 600, true, "Kore Frame Example");

    u32* layer_pixels = frame_add_pixels_layer(&main_frame, 400, 300);
    if (!layer_pixels) {
        eprn("Failed to add pixel layer to frame");
        return 1;
    }

    const int layer_w   = 400;
    const int layer_h   = 300;
    TimePoint start     = time_now();
    TimePoint fps_timer = start;
    f64 current_fps     = 0.0;
    bool fullscreen     = false;

    // Main loop
    while (frame_loop(&main_frame)) {
        FrameEvent ev = frame_event_poll(&main_frame);

        switch (ev.type) {
        case FRAME_EVENT_NONE:
            TimePoint now   = time_now();
            TimeDuration dt = time_elapsed(start, now);
            f64 t           = time_secs(dt);
            render_demo(layer_pixels, layer_w, layer_h, t);

            // Report FPS every 5 seconds
            if (time_secs(time_elapsed(fps_timer, now)) >= 5.0) {
                current_fps = frame_fps(&main_frame);
                eprn("Current FPS: %.2f", current_fps);
                fps_timer = now;
            }
            break;

        case FRAME_EVENT_KEY_DOWN:
            switch (ev.key.keycode) {
            case KEY_ESCAPE:
                frame_done(&main_frame);
                break;

            case KEY_ENTER:
                if (frame_event_is_alt_pressed(&ev)) {
                    fullscreen = !fullscreen;
                    frame_fullscreen(&main_frame, fullscreen);
                }
                break;

            default:
                break;
            }
            break;

        default:
            break;
        }
    }

    frame_free_pixels_layer(layer_pixels);

    return 0;
}
