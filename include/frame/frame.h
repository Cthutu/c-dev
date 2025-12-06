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
#    include <X11/keysym.h>
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
bool frame_loop(Frame* f);
void frame_done(Frame* f);
u32* frame_add_pixels_layer(Frame* w, int width, int height);
f64 frame_fps(Frame* f);

//------------------------------------------------------------------------------
// Clean up API

void frame_free_pixels_layer(u32* pixels);

//------------------------------------------------------------------------------
// Event system

typedef enum {
    FRAME_KEY_UNKNOWN       = -1,

    // Printable keys map to ASCII values for direct comparisons
    FRAME_KEY_SPACE         = 32,
    FRAME_KEY_APOSTROPHE    = 39, // '
    FRAME_KEY_COMMA         = 44,
    FRAME_KEY_MINUS         = 45,
    FRAME_KEY_PERIOD        = 46,
    FRAME_KEY_SLASH         = 47,
    FRAME_KEY_0             = 48,
    FRAME_KEY_1             = 49,
    FRAME_KEY_2             = 50,
    FRAME_KEY_3             = 51,
    FRAME_KEY_4             = 52,
    FRAME_KEY_5             = 53,
    FRAME_KEY_6             = 54,
    FRAME_KEY_7             = 55,
    FRAME_KEY_8             = 56,
    FRAME_KEY_9             = 57,
    FRAME_KEY_SEMICOLON     = 59,
    FRAME_KEY_EQUAL         = 61,
    FRAME_KEY_A             = 65,
    FRAME_KEY_B             = 66,
    FRAME_KEY_C             = 67,
    FRAME_KEY_D             = 68,
    FRAME_KEY_E             = 69,
    FRAME_KEY_F             = 70,
    FRAME_KEY_G             = 71,
    FRAME_KEY_H             = 72,
    FRAME_KEY_I             = 73,
    FRAME_KEY_J             = 74,
    FRAME_KEY_K             = 75,
    FRAME_KEY_L             = 76,
    FRAME_KEY_M             = 77,
    FRAME_KEY_N             = 78,
    FRAME_KEY_O             = 79,
    FRAME_KEY_P             = 80,
    FRAME_KEY_Q             = 81,
    FRAME_KEY_R             = 82,
    FRAME_KEY_S             = 83,
    FRAME_KEY_T             = 84,
    FRAME_KEY_U             = 85,
    FRAME_KEY_V             = 86,
    FRAME_KEY_W             = 87,
    FRAME_KEY_X             = 88,
    FRAME_KEY_Y             = 89,
    FRAME_KEY_Z             = 90,
    FRAME_KEY_LEFT_BRACKET  = 91,
    FRAME_KEY_BACKSLASH     = 92,
    FRAME_KEY_RIGHT_BRACKET = 93,
    FRAME_KEY_GRAVE_ACCENT  = 96,
    FRAME_KEY_WORLD_1       = 161, // Non-US #1
    FRAME_KEY_WORLD_2       = 162, // Non-US #2

    FRAME_KEY_ESCAPE        = 256,
    FRAME_KEY_ENTER         = 257,
    FRAME_KEY_TAB           = 258,

    FRAME_KEY_BACKSPACE     = 259,
    FRAME_KEY_INSERT        = 260,
    FRAME_KEY_DELETE        = 261,
    FRAME_KEY_RIGHT         = 262,
    FRAME_KEY_LEFT          = 263,
    FRAME_KEY_DOWN          = 264,
    FRAME_KEY_UP            = 265,
    FRAME_KEY_PAGE_UP       = 266,
    FRAME_KEY_PAGE_DOWN     = 267,
    FRAME_KEY_HOME          = 268,
    FRAME_KEY_END           = 269,

    FRAME_KEY_CAPS_LOCK     = 280,
    FRAME_KEY_SCROLL_LOCK   = 281,
    FRAME_KEY_NUM_LOCK      = 282,
    FRAME_KEY_PRINT_SCREEN  = 283,
    FRAME_KEY_PAUSE         = 284,

    FRAME_KEY_F1            = 290,
    FRAME_KEY_F2            = 291,
    FRAME_KEY_F3            = 292,
    FRAME_KEY_F4            = 293,
    FRAME_KEY_F5            = 294,
    FRAME_KEY_F6            = 295,
    FRAME_KEY_F7            = 296,
    FRAME_KEY_F8            = 297,
    FRAME_KEY_F9            = 298,
    FRAME_KEY_F10           = 299,
    FRAME_KEY_F11           = 300,
    FRAME_KEY_F12           = 301,
    FRAME_KEY_F13           = 302,
    FRAME_KEY_F14           = 303,
    FRAME_KEY_F15           = 304,
    FRAME_KEY_F16           = 305,
    FRAME_KEY_F17           = 306,
    FRAME_KEY_F18           = 307,
    FRAME_KEY_F19           = 308,
    FRAME_KEY_F20           = 309,
    FRAME_KEY_F21           = 310,
    FRAME_KEY_F22           = 311,
    FRAME_KEY_F23           = 312,
    FRAME_KEY_F24           = 313,
    FRAME_KEY_F25           = 314,

    FRAME_KEY_KP_0          = 320,
    FRAME_KEY_KP_1          = 321,
    FRAME_KEY_KP_2          = 322,
    FRAME_KEY_KP_3          = 323,
    FRAME_KEY_KP_4          = 324,
    FRAME_KEY_KP_5          = 325,
    FRAME_KEY_KP_6          = 326,
    FRAME_KEY_KP_7          = 327,
    FRAME_KEY_KP_8          = 328,
    FRAME_KEY_KP_9          = 329,
    FRAME_KEY_KP_DECIMAL    = 330,
    FRAME_KEY_KP_DIVIDE     = 331,
    FRAME_KEY_KP_MULTIPLY   = 332,
    FRAME_KEY_KP_SUBTRACT   = 333,
    FRAME_KEY_KP_ADD        = 334,
    FRAME_KEY_KP_ENTER      = 335,
    FRAME_KEY_KP_EQUAL      = 336,

    FRAME_KEY_LEFT_SHIFT    = 340,
    FRAME_KEY_LEFT_CONTROL  = 341,
    FRAME_KEY_LEFT_ALT      = 342,
    FRAME_KEY_LEFT_SUPER    = 343,
    FRAME_KEY_RIGHT_SHIFT   = 344,
    FRAME_KEY_RIGHT_CONTROL = 345,
    FRAME_KEY_RIGHT_ALT     = 346,
    FRAME_KEY_RIGHT_SUPER   = 347,
    FRAME_KEY_MENU          = 348,
} FrameKeyCode;

typedef enum {
    FRAME_EVENT_NONE,            // No event to process
    FRAME_EVENT_KEYDOWN,         // Key was pressed down
    FRAME_EVENT_KEYUP,           // Key was released
    FRAME_EVENT_MOUSEMOVE,       // Mouse moved within the bounds of the window
    FRAME_EVENT_MOUSEBUTTONDOWN, // A mouse button was pressed
    FRAME_EVENT_MOUSEBUTTONUP,   // A mouse button was released
    FRAME_EVENT_RESIZE,          // Window was resized
    FRAME_EVENT_SUSPEND,         // Window was minimized or lost focus
    FRAME_EVENT_RESUME,          // Window was restored or gained focus
} FrameEventType;

typedef struct {
    FrameEventType type;
    union {
        struct {
            int x;
            int y;
        } mouse_move;
        struct {
            int button;
            int x;
            int y;
        } mouse_button;
        struct {
            FrameKeyCode keycode;
        } key;
        struct {
            int width;
            int height;
        } resize;
    };
} FrameEvent;

FrameEvent frame_poll_event(Frame* f);

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// I M P L E M E N T A T I O N
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

#ifdef KORE_IMPLEMENTATION

global_variable bool g_frame_done = false;

//------------------------------------------------------------------------------
// Linux frame implementation

#    if KORE_OS_LINUX

global_variable Atom g_wm_delete_window = None;

#        define FRAME_EVENT_QUEUE_CAPACITY 64

typedef struct {
    FrameEvent events[FRAME_EVENT_QUEUE_CAPACITY];
    u32 head;
    u32 tail;
} FrameEventQueue;

global_variable FrameEventQueue g_frame_event_queue = {0};

internal inline void frame_event_enqueue(FrameEvent event) {
    u32 next = (g_frame_event_queue.tail + 1) % FRAME_EVENT_QUEUE_CAPACITY;
    if (next == g_frame_event_queue.head) {
        g_frame_event_queue.head =
            (g_frame_event_queue.head + 1) % FRAME_EVENT_QUEUE_CAPACITY;
    }

    g_frame_event_queue.events[g_frame_event_queue.tail] = event;
    g_frame_event_queue.tail                             = next;
}

internal inline FrameEvent frame_event_dequeue(void) {
    if (g_frame_event_queue.head == g_frame_event_queue.tail) {
        return (FrameEvent){.type = FRAME_EVENT_NONE};
    }

    FrameEvent event = g_frame_event_queue.events[g_frame_event_queue.head];
    g_frame_event_queue.head =
        (g_frame_event_queue.head + 1) % FRAME_EVENT_QUEUE_CAPACITY;
    return event;
}

internal FrameKeyCode frame_translate_keysym(KeySym sym) {
    if (sym >= XK_a && sym <= XK_z) {
        sym = sym - XK_a + XK_A;
    }
    if (sym >= XK_space && sym <= XK_asciitilde) {
        return (FrameKeyCode)sym;
    }

    switch (sym) {
    case XK_Escape:
        return FRAME_KEY_ESCAPE;
    case XK_Return:
        return FRAME_KEY_ENTER;
    case XK_Tab:
        return FRAME_KEY_TAB;
    case XK_BackSpace:
        return FRAME_KEY_BACKSPACE;
    case XK_Insert:
        return FRAME_KEY_INSERT;
    case XK_Delete:
        return FRAME_KEY_DELETE;
    case XK_Right:
        return FRAME_KEY_RIGHT;
    case XK_Left:
        return FRAME_KEY_LEFT;
    case XK_Down:
        return FRAME_KEY_DOWN;
    case XK_Up:
        return FRAME_KEY_UP;
    case XK_Page_Up:
        return FRAME_KEY_PAGE_UP;
    case XK_Page_Down:
        return FRAME_KEY_PAGE_DOWN;
    case XK_Home:
        return FRAME_KEY_HOME;
    case XK_End:
        return FRAME_KEY_END;
    case XK_Caps_Lock:
        return FRAME_KEY_CAPS_LOCK;
    case XK_Scroll_Lock:
        return FRAME_KEY_SCROLL_LOCK;
    case XK_Num_Lock:
        return FRAME_KEY_NUM_LOCK;
    case XK_Print:
        return FRAME_KEY_PRINT_SCREEN;
    case XK_Pause:
        return FRAME_KEY_PAUSE;
    case XK_F1:
        return FRAME_KEY_F1;
    case XK_F2:
        return FRAME_KEY_F2;
    case XK_F3:
        return FRAME_KEY_F3;
    case XK_F4:
        return FRAME_KEY_F4;
    case XK_F5:
        return FRAME_KEY_F5;
    case XK_F6:
        return FRAME_KEY_F6;
    case XK_F7:
        return FRAME_KEY_F7;
    case XK_F8:
        return FRAME_KEY_F8;
    case XK_F9:
        return FRAME_KEY_F9;
    case XK_F10:
        return FRAME_KEY_F10;
    case XK_F11:
        return FRAME_KEY_F11;
    case XK_F12:
        return FRAME_KEY_F12;
    case XK_F13:
        return FRAME_KEY_F13;
    case XK_F14:
        return FRAME_KEY_F14;
    case XK_F15:
        return FRAME_KEY_F15;
    case XK_F16:
        return FRAME_KEY_F16;
    case XK_F17:
        return FRAME_KEY_F17;
    case XK_F18:
        return FRAME_KEY_F18;
    case XK_F19:
        return FRAME_KEY_F19;
    case XK_F20:
        return FRAME_KEY_F20;
    case XK_F21:
        return FRAME_KEY_F21;
    case XK_F22:
        return FRAME_KEY_F22;
    case XK_F23:
        return FRAME_KEY_F23;
    case XK_F24:
        return FRAME_KEY_F24;
    case XK_F25:
        return FRAME_KEY_F25;
    case XK_KP_0:
        return FRAME_KEY_KP_0;
    case XK_KP_1:
        return FRAME_KEY_KP_1;
    case XK_KP_2:
        return FRAME_KEY_KP_2;
    case XK_KP_3:
        return FRAME_KEY_KP_3;
    case XK_KP_4:
        return FRAME_KEY_KP_4;
    case XK_KP_5:
        return FRAME_KEY_KP_5;
    case XK_KP_6:
        return FRAME_KEY_KP_6;
    case XK_KP_7:
        return FRAME_KEY_KP_7;
    case XK_KP_8:
        return FRAME_KEY_KP_8;
    case XK_KP_9:
        return FRAME_KEY_KP_9;
    case XK_KP_Decimal:
        return FRAME_KEY_KP_DECIMAL;
    case XK_KP_Divide:
        return FRAME_KEY_KP_DIVIDE;
    case XK_KP_Multiply:
        return FRAME_KEY_KP_MULTIPLY;
    case XK_KP_Subtract:
        return FRAME_KEY_KP_SUBTRACT;
    case XK_KP_Add:
        return FRAME_KEY_KP_ADD;
    case XK_KP_Enter:
        return FRAME_KEY_KP_ENTER;
    case XK_KP_Equal:
        return FRAME_KEY_KP_EQUAL;
    case XK_Shift_L:
        return FRAME_KEY_LEFT_SHIFT;
    case XK_Control_L:
        return FRAME_KEY_LEFT_CONTROL;
    case XK_Alt_L:
        return FRAME_KEY_LEFT_ALT;
    case XK_Super_L:
        return FRAME_KEY_LEFT_SUPER;
    case XK_Shift_R:
        return FRAME_KEY_RIGHT_SHIFT;
    case XK_Control_R:
        return FRAME_KEY_RIGHT_CONTROL;
    case XK_Alt_R:
        return FRAME_KEY_RIGHT_ALT;
    case XK_Super_R:
        return FRAME_KEY_RIGHT_SUPER;
    case XK_Menu:
        return FRAME_KEY_MENU;
    default:
        break;
    }

    return FRAME_KEY_UNKNOWN;
}

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
                     ButtonPressMask | ButtonReleaseMask | PointerMotionMask |
                     StructureNotifyMask | FocusChangeMask);
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
    if (g_frame_done) {
        frame_cleanup(f);
        return false;
    }

    XEvent event;
    bool running = true;

    while (XPending(f->display)) {
        XNextEvent(f->display, &event);

        switch (event.type) {
        case ClientMessage:
            if ((Atom)event.xclient.data.l[0] == g_wm_delete_window) {
                running = false;
            }
            break;

        case DestroyNotify:
            running = false;
            break;

        case KeyPress: {
            FrameKeyCode keycode =
                frame_translate_keysym(XLookupKeysym(&event.xkey, 0));
            if (keycode != FRAME_KEY_UNKNOWN) {
                frame_event_enqueue((FrameEvent){.type = FRAME_EVENT_KEYDOWN,
                                                 .key.keycode = keycode});
            }
        } break;

        case KeyRelease: {
            FrameKeyCode keycode =
                frame_translate_keysym(XLookupKeysym(&event.xkey, 0));
            if (keycode != FRAME_KEY_UNKNOWN) {
                frame_event_enqueue((FrameEvent){.type = FRAME_EVENT_KEYUP,
                                                 .key.keycode = keycode});
            }
        } break;

        case ButtonPress: {
            FrameEvent evt          = {.type = FRAME_EVENT_MOUSEBUTTONDOWN};
            evt.mouse_button.button = event.xbutton.button;
            evt.mouse_button.x      = event.xbutton.x;
            evt.mouse_button.y      = event.xbutton.y;
            frame_event_enqueue(evt);
        } break;

        case ButtonRelease: {
            FrameEvent evt          = {.type = FRAME_EVENT_MOUSEBUTTONUP};
            evt.mouse_button.button = event.xbutton.button;
            evt.mouse_button.x      = event.xbutton.x;
            evt.mouse_button.y      = event.xbutton.y;
            frame_event_enqueue(evt);
        } break;

        case MotionNotify: {
            FrameEvent evt   = {.type = FRAME_EVENT_MOUSEMOVE};
            evt.mouse_move.x = event.xmotion.x;
            evt.mouse_move.y = event.xmotion.y;
            frame_event_enqueue(evt);
        } break;

        case ConfigureNotify: {
            if (event.xconfigure.width != f->width ||
                event.xconfigure.height != f->height) {
                f->width  = event.xconfigure.width;
                f->height = event.xconfigure.height;
                frame_event_enqueue((FrameEvent){
                    .type          = FRAME_EVENT_RESIZE,
                    .resize.width  = f->width,
                    .resize.height = f->height,
                });
            }
        } break;

        case FocusOut:
            frame_event_enqueue((FrameEvent){.type = FRAME_EVENT_SUSPEND});
            break;

        case FocusIn:
            frame_event_enqueue((FrameEvent){.type = FRAME_EVENT_RESUME});
            break;

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

FrameEvent frame_poll_event(Frame* f) {
    KORE_UNUSED(f);
    return frame_event_dequeue();
}

//------------------------------------------------------------------------------
// Windows implementation

#    elif KORE_OS_WINDOWS

#        define FRAME_EVENT_QUEUE_CAPACITY 64

typedef struct {
    FrameEvent events[FRAME_EVENT_QUEUE_CAPACITY];
    u32 head;
    u32 tail;
} FrameEventQueue;

global_variable FrameEventQueue g_frame_event_queue = {0};

internal inline void frame_event_enqueue(FrameEvent event) {
    u32 next = (g_frame_event_queue.tail + 1) % FRAME_EVENT_QUEUE_CAPACITY;
    if (next == g_frame_event_queue.head) {
        g_frame_event_queue.head =
            (g_frame_event_queue.head + 1) % FRAME_EVENT_QUEUE_CAPACITY;
    }

    g_frame_event_queue.events[g_frame_event_queue.tail] = event;
    g_frame_event_queue.tail                             = next;
}

internal inline FrameEvent frame_event_dequeue(void) {
    if (g_frame_event_queue.head == g_frame_event_queue.tail) {
        return (FrameEvent){.type = FRAME_EVENT_NONE};
    }

    FrameEvent event = g_frame_event_queue.events[g_frame_event_queue.head];
    g_frame_event_queue.head =
        (g_frame_event_queue.head + 1) % FRAME_EVENT_QUEUE_CAPACITY;
    return event;
}

internal FrameKeyCode frame_translate_virtual_key(WPARAM vk) {
    if (vk >= '0' && vk <= '9') {
        return (FrameKeyCode)vk;
    }
    if (vk >= 'A' && vk <= 'Z') {
        return (FrameKeyCode)vk;
    }

    switch (vk) {
    case VK_SPACE:
        return FRAME_KEY_SPACE;
    case VK_OEM_7:
        return FRAME_KEY_APOSTROPHE;
    case VK_OEM_COMMA:
        return FRAME_KEY_COMMA;
    case VK_OEM_MINUS:
        return FRAME_KEY_MINUS;
    case VK_OEM_PERIOD:
        return FRAME_KEY_PERIOD;
    case VK_OEM_2:
        return FRAME_KEY_SLASH;
    case VK_OEM_1:
        return FRAME_KEY_SEMICOLON;
    case VK_OEM_PLUS:
        return FRAME_KEY_EQUAL;
    case VK_OEM_3:
        return FRAME_KEY_GRAVE_ACCENT;
    case VK_OEM_4:
        return FRAME_KEY_LEFT_BRACKET;
    case VK_OEM_5:
        return FRAME_KEY_BACKSLASH;
    case VK_OEM_6:
        return FRAME_KEY_RIGHT_BRACKET;
    case VK_ESCAPE:
        return FRAME_KEY_ESCAPE;
    case VK_RETURN:
        return FRAME_KEY_ENTER;
    case VK_TAB:
        return FRAME_KEY_TAB;
    case VK_BACK:
        return FRAME_KEY_BACKSPACE;
    case VK_INSERT:
        return FRAME_KEY_INSERT;
    case VK_DELETE:
        return FRAME_KEY_DELETE;
    case VK_RIGHT:
        return FRAME_KEY_RIGHT;
    case VK_LEFT:
        return FRAME_KEY_LEFT;
    case VK_DOWN:
        return FRAME_KEY_DOWN;
    case VK_UP:
        return FRAME_KEY_UP;
    case VK_PRIOR:
        return FRAME_KEY_PAGE_UP;
    case VK_NEXT:
        return FRAME_KEY_PAGE_DOWN;
    case VK_HOME:
        return FRAME_KEY_HOME;
    case VK_END:
        return FRAME_KEY_END;
    case VK_CAPITAL:
        return FRAME_KEY_CAPS_LOCK;
    case VK_SCROLL:
        return FRAME_KEY_SCROLL_LOCK;
    case VK_NUMLOCK:
        return FRAME_KEY_NUM_LOCK;
    case VK_SNAPSHOT:
        return FRAME_KEY_PRINT_SCREEN;
    case VK_PAUSE:
        return FRAME_KEY_PAUSE;
    case VK_F1:
        return FRAME_KEY_F1;
    case VK_F2:
        return FRAME_KEY_F2;
    case VK_F3:
        return FRAME_KEY_F3;
    case VK_F4:
        return FRAME_KEY_F4;
    case VK_F5:
        return FRAME_KEY_F5;
    case VK_F6:
        return FRAME_KEY_F6;
    case VK_F7:
        return FRAME_KEY_F7;
    case VK_F8:
        return FRAME_KEY_F8;
    case VK_F9:
        return FRAME_KEY_F9;
    case VK_F10:
        return FRAME_KEY_F10;
    case VK_F11:
        return FRAME_KEY_F11;
    case VK_F12:
        return FRAME_KEY_F12;
    case VK_F13:
        return FRAME_KEY_F13;
    case VK_F14:
        return FRAME_KEY_F14;
    case VK_F15:
        return FRAME_KEY_F15;
    case VK_F16:
        return FRAME_KEY_F16;
    case VK_F17:
        return FRAME_KEY_F17;
    case VK_F18:
        return FRAME_KEY_F18;
    case VK_F19:
        return FRAME_KEY_F19;
    case VK_F20:
        return FRAME_KEY_F20;
    case VK_F21:
        return FRAME_KEY_F21;
    case VK_F22:
        return FRAME_KEY_F22;
    case VK_F23:
        return FRAME_KEY_F23;
    case VK_F24:
        return FRAME_KEY_F24;
    case VK_NUMPAD0:
        return FRAME_KEY_KP_0;
    case VK_NUMPAD1:
        return FRAME_KEY_KP_1;
    case VK_NUMPAD2:
        return FRAME_KEY_KP_2;
    case VK_NUMPAD3:
        return FRAME_KEY_KP_3;
    case VK_NUMPAD4:
        return FRAME_KEY_KP_4;
    case VK_NUMPAD5:
        return FRAME_KEY_KP_5;
    case VK_NUMPAD6:
        return FRAME_KEY_KP_6;
    case VK_NUMPAD7:
        return FRAME_KEY_KP_7;
    case VK_NUMPAD8:
        return FRAME_KEY_KP_8;
    case VK_NUMPAD9:
        return FRAME_KEY_KP_9;
    case VK_DECIMAL:
        return FRAME_KEY_KP_DECIMAL;
    case VK_DIVIDE:
        return FRAME_KEY_KP_DIVIDE;
    case VK_MULTIPLY:
        return FRAME_KEY_KP_MULTIPLY;
    case VK_SUBTRACT:
        return FRAME_KEY_KP_SUBTRACT;
    case VK_ADD:
        return FRAME_KEY_KP_ADD;
    case VK_LSHIFT:
        return FRAME_KEY_LEFT_SHIFT;
    case VK_LCONTROL:
        return FRAME_KEY_LEFT_CONTROL;
    case VK_LMENU:
        return FRAME_KEY_LEFT_ALT;
    case VK_LWIN:
        return FRAME_KEY_LEFT_SUPER;
    case VK_RSHIFT:
        return FRAME_KEY_RIGHT_SHIFT;
    case VK_RCONTROL:
        return FRAME_KEY_RIGHT_CONTROL;
    case VK_RMENU:
        return FRAME_KEY_RIGHT_ALT;
    case VK_RWIN:
        return FRAME_KEY_RIGHT_SUPER;
    case VK_APPS:
        return FRAME_KEY_MENU;
    default:
        break;
    }

    return FRAME_KEY_UNKNOWN;
}

internal FrameEvent frame_translate_message(const MSG* msg) {
    FrameEvent event = {.type = FRAME_EVENT_NONE};
    switch (msg->message) {
    case WM_MOUSEMOVE:
        event.type         = FRAME_EVENT_MOUSEMOVE;
        event.mouse_move.x = (int)(short)LOWORD(msg->lParam);
        event.mouse_move.y = (int)(short)HIWORD(msg->lParam);
        break;
    case WM_LBUTTONDOWN:
        event.type                = FRAME_EVENT_MOUSEBUTTONDOWN;
        event.mouse_button.button = 1;
        event.mouse_button.x      = (int)(short)LOWORD(msg->lParam);
        event.mouse_button.y      = (int)(short)HIWORD(msg->lParam);
        break;
    case WM_LBUTTONUP:
        event.type                = FRAME_EVENT_MOUSEBUTTONUP;
        event.mouse_button.button = 1;
        event.mouse_button.x      = (int)(short)LOWORD(msg->lParam);
        event.mouse_button.y      = (int)(short)HIWORD(msg->lParam);
        break;
    case WM_MBUTTONDOWN:
        event.type                = FRAME_EVENT_MOUSEBUTTONDOWN;
        event.mouse_button.button = 2;
        event.mouse_button.x      = (int)(short)LOWORD(msg->lParam);
        event.mouse_button.y      = (int)(short)HIWORD(msg->lParam);
        break;
    case WM_MBUTTONUP:
        event.type                = FRAME_EVENT_MOUSEBUTTONUP;
        event.mouse_button.button = 2;
        event.mouse_button.x      = (int)(short)LOWORD(msg->lParam);
        event.mouse_button.y      = (int)(short)HIWORD(msg->lParam);
        break;
    case WM_RBUTTONDOWN:
        event.type                = FRAME_EVENT_MOUSEBUTTONDOWN;
        event.mouse_button.button = 3;
        event.mouse_button.x      = (int)(short)LOWORD(msg->lParam);
        event.mouse_button.y      = (int)(short)HIWORD(msg->lParam);
        break;
    case WM_RBUTTONUP:
        event.type                = FRAME_EVENT_MOUSEBUTTONUP;
        event.mouse_button.button = 3;
        event.mouse_button.x      = (int)(short)LOWORD(msg->lParam);
        event.mouse_button.y      = (int)(short)HIWORD(msg->lParam);
        break;
    case WM_KEYDOWN:
    case WM_SYSKEYDOWN: {
        FrameKeyCode keycode = frame_translate_virtual_key(msg->wParam);
        if (keycode != FRAME_KEY_UNKNOWN) {
            event.type        = FRAME_EVENT_KEYDOWN;
            event.key.keycode = keycode;
        }
    } break;
    case WM_KEYUP:
    case WM_SYSKEYUP: {
        FrameKeyCode keycode = frame_translate_virtual_key(msg->wParam);
        if (keycode != FRAME_KEY_UNKNOWN) {
            event.type        = FRAME_EVENT_KEYUP;
            event.key.keycode = keycode;
        }
    } break;
    case WM_SIZE:
        event.type          = FRAME_EVENT_RESIZE;
        event.resize.width  = (int)LOWORD(msg->lParam);
        event.resize.height = (int)HIWORD(msg->lParam);
        break;
    case WM_ACTIVATE:
        event.type = (LOWORD(msg->wParam) == WA_INACTIVE) ? FRAME_EVENT_SUSPEND
                                                          : FRAME_EVENT_RESUME;
        break;
    default:
        break;
    }

    return event;
}

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
