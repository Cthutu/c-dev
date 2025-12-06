#define KORE_IMPLEMENTATION
#include <frame/frame.h>
#include <kore/kore.h>

int kmain(int argc, char** argv) {
    KORE_UNUSED(argc);
    KORE_UNUSED(argv);

    Frame main_frame = frame_open(800, 600, "Pixelf Zombie");
    u32* pixels      = frame_add_pixels_layer(&main_frame, 400, 300);

    while (frame_loop(&main_frame)) {
        // Example: Fill the pixel layer with a color gradient
        for (int y = 0; y < 300; ++y) {
            for (int x = 0; x < 400; ++x) {
                u8 r                = (u8)(x * 255 / 399);
                u8 g                = (u8)(y * 255 / 299);
                u8 b                = 128;
                pixels[y * 400 + x] = (0xFF << 24) | (r << 16) | (g << 8) | b;
            }
        }
    }

    return 0;
}
