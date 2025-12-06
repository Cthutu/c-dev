#define KORE_IMPLEMENTATION
#include <frame/frame.h>
#include <kore/kore.h>
#include <math.h>

static inline u32 pack_rgba(f32 r, f32 g, f32 b) {
    r      = r < 0.f ? 0.f : (r > 1.f ? 1.f : r);
    g      = g < 0.f ? 0.f : (g > 1.f ? 1.f : g);
    b      = b < 0.f ? 0.f : (b > 1.f ? 1.f : b);
    u32 ri = (u32)(r * 255.0f + 0.5f);
    u32 gi = (u32)(g * 255.0f + 0.5f);
    u32 bi = (u32)(b * 255.0f + 0.5f);
    return 0xFF000000 | (ri << 16) | (gi << 8) | bi;
}

// Simple plasma-like animation with orbiting bands and a subtle pixel grid.
static void render_demo(u32* pixels, int w, int h, f64 time_s) {
    if (!pixels) {
        return;
    }
    float t     = (float)time_s;
    float inv_w = 1.0f / (float)w;
    float inv_h = 1.0f / (float)h;
    float cx    = 0.5f;
    float cy    = 0.5f;
    for (int y = 0; y < h; ++y) {
        float fy = (float)y * inv_h;
        for (int x = 0; x < w; ++x) {
            float fx          = (float)x * inv_w;
            float dx          = fx - cx;
            float dy          = fy - cy;
            float radius      = sqrtf(dx * dx + dy * dy);

            float wave1       = sinf(t * 1.2f + fx * 12.0f + sinf(fy * 6.0f));
            float wave2       = sinf(t * 0.7f + fy * 10.0f + cosf(fx * 7.0f));
            float ring        = sinf(radius * 18.0f - t * 1.5f);

            float v           = (wave1 + wave2 + ring) / 3.0f;
            float hue         = 0.6f + 0.4f * v; // keep within 0..1 range

            float r           = 0.6f + 0.4f * sinf(6.28318f * (hue + 0.00f));
            float g           = 0.6f + 0.4f * sinf(6.28318f * (hue + 0.33f));
            float b           = 0.6f + 0.4f * sinf(6.28318f * (hue + 0.66f));

            // subtle grid to emphasize pixel-perfect scaling
            float grid        = ((x ^ y) & 1) ? 0.98f : 1.0f;
            pixels[y * w + x] = pack_rgba(r * grid, g * grid, b * grid);
        }
    }
}

int kmain(int argc, char** argv) {
    KORE_UNUSED(argc);
    KORE_UNUSED(argv);

    Frame main_frame  = frame_open(800, 600, "Kore Frame Example");

    u32* layer_pixels = frame_add_pixels_layer(&main_frame, 400, 300);
    if (!layer_pixels) {
        eprn("Failed to add pixel layer to frame");
        return 1;
    }

    const int layer_w = 400;
    const int layer_h = 300;
    TimePoint start   = time_now();

    // Main loop
    while (frame_loop(&main_frame)) {
        TimePoint now   = time_now();
        TimeDuration dt = time_elapsed(start, now);
        f64 t           = time_secs(dt);
        render_demo(layer_pixels, layer_w, layer_h, t);

        // Optionally print FPS
        // f64 fps = frame_fps(&main_frame);
        // eprn("Current FPS: %.2f", fps); // Uncomment to see FPS in console
    }

    frame_free_pixels_layer(layer_pixels);

    return 0;
}
