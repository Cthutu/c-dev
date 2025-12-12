#define KORE_IMPLEMENTATION
#include <frame/draw.h>
#include <frame/frame.h>
#include <kore/kore.h>
#include <math.h>
#include <stddef.h>

// Simple plasma-like animation with orbiting bands and a subtle pixel grid.
static void render_demo(u32* pixels, int w, int h, f64 time_s) {
    KORE_UNUSED(time_s);

    PixelBufferDrawContext draw_ctx = draw_create_pixel_context(w, h, pixels);
    DrawContext* ctx                = &draw_ctx.ctx;

    draw_horz_line(ctx, 0, 0, w, COLOUR_RGB(255, 0, 0));
    draw_horz_line(ctx, 0, h - 1, w, COLOUR_RGB(255, 0, 0));
    draw_vert_line(ctx, 0, 0, h, COLOUR_RGB(255, 0, 0));
    draw_vert_line(ctx, w - 1, 0, h, COLOUR_RGB(255, 0, 0));

    const int cx = w / 2;
    const int cy = h / 2;
    static const struct {
        double dx;
        double dy;
        u32 colour;
    } octants[] = {
        {0.9238795325,
         -0.3826834324,
         COLOUR_RGB(255, 128, 0)}, // East-NorthEast (orange)
        {0.3826834324,
         -0.9238795325,
         COLOUR_RGB(255, 255, 0)}, // North-NorthEast (yellow)
        {-0.3826834324,
         -0.9238795325,
         COLOUR_RGB(0, 255, 0)}, // North-NorthWest (green)
        {-0.9238795325,
         -0.3826834324,
         COLOUR_RGB(0, 255, 255)}, // West-NorthWest (cyan)
        {-0.9238795325,
         0.3826834324,
         COLOUR_RGB(0, 128, 255)}, // West-SouthWest (blue)
        {-0.3826834324,
         0.9238795325,
         COLOUR_RGB(0, 0, 255)}, // South-SouthWest (blue)
        {0.3826834324,
         0.9238795325,
         COLOUR_RGB(128, 0, 255)}, // South-SouthEast (purple)
        {0.9238795325,
         0.3826834324,
         COLOUR_RGB(255, 0, 255)}, // East-SouthEast (magenta)
    };

    for (size_t i = 0; i < sizeof(octants) / sizeof(octants[0]); ++i) {
        const double dx = octants[i].dx;
        const double dy = octants[i].dy;

        const double tx =
            dx > 0.0 ? (w - 1 - cx) / dx : (dx < 0.0 ? (0.0 - cx) / dx : 1e9);
        const double ty =
            dy > 0.0 ? (h - 1 - cy) / dy : (dy < 0.0 ? (0.0 - cy) / dy : 1e9);
        const double t = (tx < ty) ? tx : ty;

        int x1         = (int)lround(cx + dx * t);
        int y1         = (int)lround(cy + dy * t);

        if (x1 < 0) {
            x1 = 0;
        } else if (x1 >= w) {
            x1 = w - 1;
        }
        if (y1 < 0) {
            y1 = 0;
        } else if (y1 >= h) {
            y1 = h - 1;
        }

        draw_line(ctx, cx, cy, x1, y1, octants[i].colour);
    }
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
