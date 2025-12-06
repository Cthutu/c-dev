//------------------------------------------------------------------------------
// Frame module
//
// Copyright (C)2025 Matt Davies, all rights reserved
//------------------------------------------------------------------------------

#pragma once

//------------------------------------------------------------------------------

#include <frame/gfx.h>
#include <kore/kore.h>

#if KORE_OS_LINUX
#    include <GL/glx.h>
#    include <X11/Xlib.h>
#    include <X11/Xutil.h>
#endif // KORE_OS_LINUX

//------------------------------------------------------------------------------

typedef struct {
    // Frame parameters
    const char* title;
    int width;
    int height;
    Array(GfxLayer*) layers;

    // FPS Tracking
    TimePoint last_time;
    u64 frame_count;
    f64 fps;

// OS Windows references
#if KORE_OS_WINDOWS
    HWND hwnd;
    HDC hdc;
    HGLRC hglrc;
#elif KORE_OS_LINUX
    GLXContext glx_ctx;
    Display* display;
    Window window;
    Colormap colormap;
#else
#    error "Unsupported OS"
#endif
} Frame;

//------------------------------------------------------------------------------

Frame frame_open(int width, int height, cstr title);
bool frame_loop(Frame* w);
u32* frame_add_pixels_layer(Frame* w, int width, int height);
f64 frame_fps(Frame* w);

//------------------------------------------------------------------------------
// Clean up API

void frame_free_pixels_layer(u32* pixels);

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// I M P L E M E N T A T I O N
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

#ifdef KORE_IMPLEMENTATION

//------------------------------------------------------------------------------
// Linux frame implementation

#    if KORE_OS_LINUX

global_variable Atom g_wm_delete_window = None;

internal void frame_cleanup(Frame* f) {
    if (!f) {
        return;
    }
    if (f->display && f->glx_ctx) {
        glXMakeCurrent(f->display, f->window, f->glx_ctx);
    }
    for (u64 i = 0; i < array_count(f->layers); ++i) {
        gfx_layer_destroy(f->layers[i]);
    }
    array_free(f->layers);
    gfx_shutdown();

    if (f->display && f->glx_ctx) {
        glXMakeCurrent(f->display, None, NULL);
        glXDestroyContext(f->display, f->glx_ctx);
        f->glx_ctx = 0;
    }
    if (f->display && f->window) {
        XDestroyWindow(f->display, f->window);
        f->window = 0;
    }
    if (f->display && f->colormap) {
        XFreeColormap(f->display, f->colormap);
        f->colormap = 0;
    }
    if (f->display) {
        XCloseDisplay(f->display);
        f->display = nullptr;
    }
}

Frame frame_open(int width, int height, cstr title) {
    Frame f = {
        .title  = title,
        .width  = width,
        .height = height,
    };

    f.display = XOpenDisplay(NULL);
    if (f.display == NULL) {
        eprn("Failed to open X display");
        exit(EXIT_FAILURE);
    }

    int screen_num = DefaultScreen(f.display);

    int glx_major = 0, glx_minor = 0;
    if (!glXQueryVersion(f.display, &glx_major, &glx_minor)) {
        eprn("GLX is not available");
        frame_cleanup(&f);
        exit(EXIT_FAILURE);
    }

    int fb_count     = 0;
    int fb_attribs[] = {GLX_X_RENDERABLE,
                        True,
                        GLX_DRAWABLE_TYPE,
                        GLX_WINDOW_BIT,
                        GLX_RENDER_TYPE,
                        GLX_RGBA_BIT,
                        GLX_X_VISUAL_TYPE,
                        GLX_TRUE_COLOR,
                        GLX_RED_SIZE,
                        8,
                        GLX_GREEN_SIZE,
                        8,
                        GLX_BLUE_SIZE,
                        8,
                        GLX_ALPHA_SIZE,
                        8,
                        GLX_DEPTH_SIZE,
                        24,
                        GLX_STENCIL_SIZE,
                        8,
                        GLX_DOUBLEBUFFER,
                        True,
                        None};

    GLXFBConfig* fb_configs =
        glXChooseFBConfig(f.display, screen_num, fb_attribs, &fb_count);
    if (!fb_configs || fb_count == 0) {
        eprn("Failed to choose GLX framebuffer config");
        frame_cleanup(&f);
        exit(EXIT_FAILURE);
    }

    GLXFBConfig fb_config = fb_configs[0];
    XFree(fb_configs);

    XVisualInfo* vi = glXGetVisualFromFBConfig(f.display, fb_config);
    if (!vi) {
        eprn("Failed to get XVisualInfo for GLX config");
        frame_cleanup(&f);
        exit(EXIT_FAILURE);
    }

    f.colormap = XCreateColormap(
        f.display, RootWindow(f.display, vi->screen), vi->visual, AllocNone);

    XSetWindowAttributes swa = {0};
    swa.colormap             = f.colormap;
    swa.event_mask           = ExposureMask | KeyPressMask | KeyReleaseMask |
                     ButtonPressMask | ButtonReleaseMask | PointerMotionMask |
                     StructureNotifyMask;

    f.window = XCreateWindow(f.display,
                             RootWindow(f.display, vi->screen),
                             0,
                             0,
                             f.width,
                             f.height,
                             0,
                             vi->depth,
                             InputOutput,
                             vi->visual,
                             CWBorderPixel | CWColormap | CWEventMask,
                             &swa);

    XFree(vi);

    XStoreName(f.display, f.window, f.title);
    XSelectInput(f.display,
                 f.window,
                 ExposureMask | KeyPressMask | KeyReleaseMask |
                     ButtonPressMask | ButtonReleaseMask | PointerMotionMask);
    g_wm_delete_window = XInternAtom(f.display, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(f.display, f.window, &g_wm_delete_window, 1);

    XMapWindow(f.display, f.window);

    f.glx_ctx =
        glXCreateNewContext(f.display, fb_config, GLX_RGBA_TYPE, 0, True);
    if (!f.glx_ctx) {
        eprn("Failed to create GLX context");
        frame_cleanup(&f);
        exit(EXIT_FAILURE);
    }

    if (!glXMakeCurrent(f.display, f.window, f.glx_ctx)) {
        eprn("Failed to activate GLX context");
        frame_cleanup(&f);
        exit(EXIT_FAILURE);
    }

    if (!gfx_init()) {
        eprn("Failed to initialize graphics system");
        frame_cleanup(&f);
        exit(EXIT_FAILURE);
    }

    XFlush(f.display);

    return f;
}

bool frame_loop(Frame* f) {
    XEvent event;
    bool running = true;

    while (XPending(f->display)) {
        XNextEvent(f->display, &event);

        switch (event.type) {
        case Expose:
            break;

        case ClientMessage:
            if ((Atom)event.xclient.data.l[0] == g_wm_delete_window) {
                running = false;
            }
            break;

        case DestroyNotify:
            running = false;
            break;

        case KeyPress:
        case KeyRelease:
            // TODO: Handle key events if needed

        case ButtonPress:
        case ButtonRelease:
        case MotionNotify:
            // TODO: Handle mouse events if needed

        default:
            break;
        }
    }

    if (!running || !f->display || !f->window || !f->glx_ctx) {
        frame_cleanup(f);
        return false;
    }

    XWindowAttributes wa;
    XGetWindowAttributes(f->display, f->window, &wa);

    glXMakeCurrent(f->display, f->window, f->glx_ctx);
    gfx_render(f->layers, array_count(f->layers), wa.width, wa.height);
    glXSwapBuffers(f->display, f->window);
    return true;
}

//------------------------------------------------------------------------------
// Windows implementation

#    elif KORE_OS_WINDOWS

static void frame_cleanup(Frame* f) {
    // Destroy layers
    for (u64 i = 0; i < array_count(f->layers); ++i) {
        gfx_layer_destroy(f->layers[i]);
    }
    array_free(f->layers);
    gfx_shutdown();

    if (f->hglrc) {
        wglMakeCurrent(NULL, NULL);
        wglDeleteContext(f->hglrc);
        f->hglrc = 0;
    }
    if (f->hdc && f->hwnd) {
        ReleaseDC(f->hwnd, f->hdc);
        f->hdc = 0;
    }
    if (f->hwnd) {
        DestroyWindow(f->hwnd);
        f->hwnd = 0;
    }
}

static LRESULT CALLBACK WindowProc(HWND hwnd,
                                   UINT msg,
                                   WPARAM wParam,
                                   LPARAM lParam) {
    Frame* f = (Frame*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
    switch (msg) {
    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        EndPaint(hwnd, &ps);
    } break;

    case WM_CLOSE:
        DestroyWindow(hwnd);
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

Frame frame_open(int width, int height, const char* title) {
    Frame f = {
        .title  = title,
        .width  = width,
        .height = height,
    };

    HINSTANCE instance = GetModuleHandle(NULL);
    WNDCLASSEX wc      = {
             .cbSize        = sizeof(WNDCLASSEX),
             .style         = CS_HREDRAW | CS_VREDRAW,
             .lpfnWndProc   = WindowProc,
             .hInstance     = instance,
             .lpszClassName = f.title,
    };

    RegisterClassEx(&wc);

    RECT rc = {0, 0, f.width, f.height};
    AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);

    f.hwnd = CreateWindowEx(WS_EX_CLIENTEDGE,
                            f.title,
                            f.title,
                            WS_OVERLAPPEDWINDOW | WS_VISIBLE,
                            CW_USEDEFAULT,
                            CW_USEDEFAULT,
                            rc.right - rc.left,
                            rc.bottom - rc.top,
                            NULL,
                            NULL,
                            instance,
                            NULL);

    if (f.hwnd == NULL) {
        eprn("Failed to create window: %ld", GetLastError());
        exit(EXIT_FAILURE);
    }

    SetWindowLongPtr(f.hwnd, GWLP_USERDATA, (LONG_PTR)&f);

    f.hdc                     = GetDC(f.hwnd);

    PIXELFORMATDESCRIPTOR pfd = {
        .nSize    = sizeof(PIXELFORMATDESCRIPTOR),
        .nVersion = 1,
        .dwFlags  = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
        .iPixelType   = PFD_TYPE_RGBA,
        .cColorBits   = 32,
        .cDepthBits   = 24,
        .cStencilBits = 8,
        .iLayerType   = PFD_MAIN_PLANE,
    };
    int pf = ChoosePixelFormat(f.hdc, &pfd);
    SetPixelFormat(f.hdc, pf, &pfd);

    f.hglrc = wglCreateContext(f.hdc);
    if (!f.hglrc) {
        eprn("Failed to create OpenGL context: %ld", GetLastError());
        frame_cleanup(&f);
        exit(EXIT_FAILURE);
    }

    if (!wglMakeCurrent(f.hdc, f.hglrc)) {
        eprn("Failed to activate OpenGL context: %ld", GetLastError());
        frame_cleanup(&f);
        exit(EXIT_FAILURE);
    }

    if (!gfx_init()) {
        eprn("Failed to initialize graphics system");
        frame_cleanup(&f);
        exit(EXIT_FAILURE);
    }

    return f;
}

bool frame_loop(Frame* f) {
    MSG msg;
    while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
        if (msg.message == WM_QUIT) {
            frame_cleanup(f);
            return false;
        }
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    if (!f->hwnd) {
        frame_cleanup(f);
        return false;
    }

    RECT cr;
    GetClientRect(f->hwnd, &cr);
    int win_w = cr.right - cr.left;
    int win_h = cr.bottom - cr.top;

    // Update OpenGL context before rendering
    wglMakeCurrent(f->hdc, f->hglrc);

    gfx_render(f->layers, array_count(f->layers), win_w, win_h);
    SwapBuffers(f->hdc);
    return true;
}

//------------------------------------------------------------------------------

#    else
#        error "Unsupported OS"
#    endif // OS_WINDOWS

//------------------------------------------------------------------------------
// Common frame functions

u32* frame_add_pixels_layer(Frame* f, int width, int height) {
    u32* pixels     = KORE_ARRAY_ALLOC(u32, width * height);
    GfxLayer* layer = gfx_layer_create(width, height, pixels);
    if (!layer) {
        eprn("Failed to create graphics layer");
        free(pixels);
        return NULL;
    }
    array_push(f->layers, layer);
    return pixels;
}

f64 frame_fps(Frame* f) {
    f->frame_count++;
    TimePoint current_time = time_now();

    // Initialize on first call
    if (f->frame_count == 1) {
        f->last_time = current_time;
        f->fps       = 0.0;
        return f->fps;
    }

    // Calculate FPS based on elapsed time since last update
    TimeDuration elapsed = time_elapsed(f->last_time, current_time);
    f64 elapsed_secs     = time_secs(elapsed);

    if (elapsed_secs > 0.0) {
        // Calculate instantaneous FPS (frames since last call / elapsed time)
        f->fps = 1.0 / elapsed_secs;
    }

    f->last_time = current_time;
    return f->fps;
}

void frame_free_pixels_layer(u32* pixels) { KORE_ARRAY_FREE(pixels); }

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

#endif // KORE_IMPLEMENTATION
