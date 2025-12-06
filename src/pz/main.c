#define KORE_IMPLEMENTATION
#include <frame/frame.h>
#include <kore/kore.h>

int kmain(int argc, char** argv) {
    KORE_UNUSED(argc);
    KORE_UNUSED(argv);

    Frame main_frame = frame_open(800, 600, "Pixelf Zombie");
    u32* pixels      = frame_add_pixels_layer(&main_frame, 400, 300);

    random_seed(time_now());

    while (frame_loop(&main_frame)) {
        // Example: Fill the pixel layer with a color gradient
        for (int i = 0; i < 400 * 300; ++i) {
            pixels[i] = 0xff000000 + (random_u64() & 0xFFFFFF);
        }

        FrameEvent event = frame_poll_event(&main_frame);
        switch (event.type) {
        case FRAME_EVENT_NONE:
            break;
        case FRAME_EVENT_KEYDOWN:
            if (event.key.keycode == FRAME_KEY_ESCAPE) { // Escape key
                frame_done(&main_frame);
            }
            prn("Key Down: %d", event.key.keycode);
            break;
        default:
            break;
        }
    }

    frame_free_pixels_layer(pixels);
    return 0;
}
