#define KORE_IMPLEMENTATION
#include "frame.h"
#include "kore.h"

int kmain(int argc, char** argv) {
    KORE_UNUSED(argc);
    KORE_UNUSED(argv);

    Frame main_frame  = frame_open(800, 600, "Kore Frame Example");

    u32* layer_pixels = frame_add_pixels_layer(&main_frame, 800, 600);
    if (!layer_pixels) {
        eprn("Failed to add pixel layer to frame");
        return 1;
    }

    // Main loop
    while (frame_loop(&main_frame)) {
        // Update pixel data (simple color fill for demonstration)
        for (int y = 0; y < 600; ++y) {
            for (int x = 0; x < 800; ++x) {
                layer_pixels[y * 800 + x] = 0xFF0000FF; // ARGB: opaque blue
            }
        }

        // Optionally print FPS
        // f64 fps = frame_fps(&main_frame);
        // eprn("Current FPS: %.2f", fps); // Uncomment to see FPS in console
    }

    frame_free_pixels_layer(layer_pixels);

    return 0;
}
