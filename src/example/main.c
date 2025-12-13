#define KORE_IMPLEMENTATION
#include <frame/draw.h>
#include <frame/frame.h>
#include <kore/kore.h>
#include <math.h>
#include <stddef.h>

int kmain(int argc, char** argv) {
    KORE_UNUSED(argc);
    KORE_UNUSED(argv);

    int window_width  = 800;
    int window_height = 600;
    int layer_width   = 400;
    int layer_height  = 300;

    Frame main_frame =
        frame_open(window_width, window_height, true, "Kore Frame Example");

    GfxLayer* layer =
        frame_add_pixels_layer(&main_frame, layer_width, layer_height);
    if (!layer) {
        eprn("Failed to add pixel layer to frame");
        return 1;
    }

    TimePoint start     = time_now();
    TimePoint fps_timer = start;
    f64 current_fps     = 0.0;
    bool fullscreen     = false;
    bool mouse_down     = false;
    int last_x          = -1;
    int last_y          = -1;

    // Main loop
    while (frame_loop(&main_frame)) {
        FrameEvent ev = frame_event_poll(&main_frame);

        switch (ev.type) {
        case FRAME_EVENT_NONE:
            TimePoint now = time_now();

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

        case FRAME_EVENT_MOUSE_BUTTON_DOWN:
            if (ev.mouse.button == MOUSE_BUTTON_LEFT) {
                mouse_down = true;
            }
            break;

        case FRAME_EVENT_MOUSE_BUTTON_UP:
            if (ev.mouse.button == MOUSE_BUTTON_LEFT) {
                mouse_down = false;
                last_x     = -1;
                last_y     = -1;
            }
            break;

        case FRAME_EVENT_MOUSE_MOVE:
            if (mouse_down) {
                int layer_x, layer_y;
                if (frame_map_coords_to_layer(&main_frame,
                                              layer,
                                              ev.mouse.x,
                                              ev.mouse.y,
                                              &layer_x,
                                              &layer_y)) {
                    if (last_x == -1 && last_y == -1) {
                        last_x = layer_x;
                        last_y = layer_y;
                    }
                    draw_line(layer,
                              last_x,
                              last_y,
                              layer_x,
                              layer_y,
                              COLOUR_RGB(255, 0, 0));
                    draw_plot(layer, layer_x, layer_y, COLOUR_RGB(255, 0, 0));
                    last_x = layer_x;
                    last_y = layer_y;
                }
            }

        default:
            break;
        }
    }

    frame_free_pixels_layer(layer);

    return 0;
}
