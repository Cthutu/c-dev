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
#elif KORE_OS_WINDOWS
#    include <windows.h>
#    include <windowsx.h>
#endif // KORE_OS_LINUX

//------------------------------------------------------------------------------

typedef enum {
    FRAME_EVENT_NONE = 0,
    FRAME_EVENT_KEY_DOWN,
    FRAME_EVENT_KEY_UP,
    FRAME_EVENT_MOUSE_MOVE,
    FRAME_EVENT_MOUSE_BUTTON_DOWN,
    FRAME_EVENT_MOUSE_BUTTON_UP,
} FrameEventType;

typedef enum {
    MOUSE_BUTTON_LEFT   = 1,
    MOUSE_BUTTON_MIDDLE = 2,
    MOUSE_BUTTON_RIGHT  = 3,
} MouseButton;

typedef enum {
    KEY_UNKNOWN            = 0,

    KEY_A                  = 4,
    KEY_B                  = 5,
    KEY_C                  = 6,
    KEY_D                  = 7,
    KEY_E                  = 8,
    KEY_F                  = 9,
    KEY_G                  = 10,
    KEY_H                  = 11,
    KEY_I                  = 12,
    KEY_J                  = 13,
    KEY_K                  = 14,
    KEY_L                  = 15,
    KEY_M                  = 16,
    KEY_N                  = 17,
    KEY_O                  = 18,
    KEY_P                  = 19,
    KEY_Q                  = 20,
    KEY_R                  = 21,
    KEY_S                  = 22,
    KEY_T                  = 23,
    KEY_U                  = 24,
    KEY_V                  = 25,
    KEY_W                  = 26,
    KEY_X                  = 27,
    KEY_Y                  = 28,
    KEY_Z                  = 29,

    KEY_1                  = 30,
    KEY_2                  = 31,
    KEY_3                  = 32,
    KEY_4                  = 33,
    KEY_5                  = 34,
    KEY_6                  = 35,
    KEY_7                  = 36,
    KEY_8                  = 37,
    KEY_9                  = 38,
    KEY_0                  = 39,

    KEY_ENTER              = 40,
    KEY_ESCAPE             = 41,
    KEY_BACKSPACE          = 42,
    KEY_TAB                = 43,
    KEY_SPACE              = 44,
    KEY_MINUS              = 45,
    KEY_EQUALS             = 46,
    KEY_LEFTBRACKET        = 47,
    KEY_RIGHTBRACKET       = 48,
    KEY_BACKSLASH          = 49,
    KEY_NONUSHASH          = 50,
    KEY_SEMICOLON          = 51,
    KEY_APOSTROPHE         = 52,
    KEY_GRAVE              = 53,
    KEY_COMMA              = 54,
    KEY_PERIOD             = 55,
    KEY_SLASH              = 56,
    KEY_CAPSLOCK           = 57,

    KEY_F1                 = 58,
    KEY_F2                 = 59,
    KEY_F3                 = 60,
    KEY_F4                 = 61,
    KEY_F5                 = 62,
    KEY_F6                 = 63,
    KEY_F7                 = 64,
    KEY_F8                 = 65,
    KEY_F9                 = 66,
    KEY_F10                = 67,
    KEY_F11                = 68,
    KEY_F12                = 69,

    KEY_PRINTSCREEN        = 70,
    KEY_SCROLLLOCK         = 71,
    KEY_PAUSE              = 72,
    KEY_INSERT             = 73,
    KEY_HOME               = 74,
    KEY_PAGEUP             = 75,
    KEY_DELETE             = 76,
    KEY_END                = 77,
    KEY_PAGEDOWN           = 78,
    KEY_RIGHT              = 79,
    KEY_LEFT               = 80,
    KEY_DOWN               = 81,
    KEY_UP                 = 82,

    KEY_NUMLOCKCLEAR       = 83,
    KEY_KP_DIVIDE          = 84,
    KEY_KP_MULTIPLY        = 85,
    KEY_KP_MINUS           = 86,
    KEY_KP_PLUS            = 87,
    KEY_KP_ENTER           = 88,
    KEY_KP_1               = 89,
    KEY_KP_2               = 90,
    KEY_KP_3               = 91,
    KEY_KP_4               = 92,
    KEY_KP_5               = 93,
    KEY_KP_6               = 94,
    KEY_KP_7               = 95,
    KEY_KP_8               = 96,
    KEY_KP_9               = 97,
    KEY_KP_0               = 98,
    KEY_KP_PERIOD          = 99,

    KEY_NONUSBACKSLASH     = 100,
    KEY_APPLICATION        = 101,
    KEY_POWER              = 102,
    KEY_KP_EQUALS          = 103,
    KEY_F13                = 104,
    KEY_F14                = 105,
    KEY_F15                = 106,
    KEY_F16                = 107,
    KEY_F17                = 108,
    KEY_F18                = 109,
    KEY_F19                = 110,
    KEY_F20                = 111,
    KEY_F21                = 112,
    KEY_F22                = 113,
    KEY_F23                = 114,
    KEY_F24                = 115,
    KEY_EXECUTE            = 116,
    KEY_HELP               = 117,
    KEY_MENU               = 118,
    KEY_SELECT             = 119,
    KEY_STOP               = 120,
    KEY_AGAIN              = 121,
    KEY_UNDO               = 122,
    KEY_CUT                = 123,
    KEY_COPY               = 124,
    KEY_PASTE              = 125,
    KEY_FIND               = 126,
    KEY_MUTE               = 127,
    KEY_VOLUMEUP           = 128,
    KEY_VOLUMEDOWN         = 129,

    KEY_KP_COMMA           = 133,
    KEY_KP_EQUALSAS400     = 134,

    KEY_INTERNATIONAL1     = 135,
    KEY_INTERNATIONAL2     = 136,
    KEY_INTERNATIONAL3     = 137,
    KEY_INTERNATIONAL4     = 138,
    KEY_INTERNATIONAL5     = 139,
    KEY_INTERNATIONAL6     = 140,
    KEY_INTERNATIONAL7     = 141,
    KEY_INTERNATIONAL8     = 142,
    KEY_INTERNATIONAL9     = 143,
    KEY_LANG1              = 144,
    KEY_LANG2              = 145,
    KEY_LANG3              = 146,
    KEY_LANG4              = 147,
    KEY_LANG5              = 148,
    KEY_LANG6              = 149,
    KEY_LANG7              = 150,
    KEY_LANG8              = 151,
    KEY_LANG9              = 152,

    KEY_ALTERASE           = 153,
    KEY_SYSREQ             = 154,
    KEY_CANCEL             = 155,
    KEY_CLEAR              = 156,
    KEY_PRIOR              = 157,
    KEY_RETURN2            = 158,
    KEY_SEPARATOR          = 159,
    KEY_OUT                = 160,
    KEY_OPER               = 161,
    KEY_CLEARAGAIN         = 162,
    KEY_CRSEL              = 163,
    KEY_EXSEL              = 164,

    KEY_KP_00              = 176,
    KEY_KP_000             = 177,
    KEY_THOUSANDSSEPARATOR = 178,
    KEY_DECIMALSEPARATOR   = 179,
    KEY_CURRENCYUNIT       = 180,
    KEY_CURRENCYSUBUNIT    = 181,
    KEY_KP_LEFTPAREN       = 182,
    KEY_KP_RIGHTPAREN      = 183,
    KEY_KP_LEFTBRACE       = 184,
    KEY_KP_RIGHTBRACE      = 185,
    KEY_KP_TAB             = 186,
    KEY_KP_BACKSPACE       = 187,
    KEY_KP_A               = 188,
    KEY_KP_B               = 189,
    KEY_KP_C               = 190,
    KEY_KP_D               = 191,
    KEY_KP_E               = 192,
    KEY_KP_F               = 193,
    KEY_KP_XOR             = 194,
    KEY_KP_POWER           = 195,
    KEY_KP_PERCENT         = 196,
    KEY_KP_LESS            = 197,
    KEY_KP_GREATER         = 198,
    KEY_KP_AMPERSAND       = 199,
    KEY_KP_DBLAMPERSAND    = 200,
    KEY_KP_VERTICALBAR     = 201,
    KEY_KP_DBLVERTICALBAR  = 202,
    KEY_KP_COLON           = 203,
    KEY_KP_HASH            = 204,
    KEY_KP_SPACE           = 205,
    KEY_KP_AT              = 206,
    KEY_KP_EXCLAM          = 207,
    KEY_KP_MEMSTORE        = 208,
    KEY_KP_MEMRECALL       = 209,
    KEY_KP_MEMCLEAR        = 210,
    KEY_KP_MEMADD          = 211,
    KEY_KP_MEMSUBTRACT     = 212,
    KEY_KP_MEMMULTIPLY     = 213,
    KEY_KP_MEMDIVIDE       = 214,
    KEY_KP_PLUSMINUS       = 215,
    KEY_KP_CLEAR           = 216,
    KEY_KP_CLEARENTRY      = 217,
    KEY_KP_BINARY          = 218,
    KEY_KP_OCTAL           = 219,
    KEY_KP_DECIMAL         = 220,
    KEY_KP_HEXADECIMAL     = 221,

    KEY_LCTRL              = 224,
    KEY_LSHIFT             = 225,
    KEY_LALT               = 226,
    KEY_LGUI               = 227,
    KEY_RCTRL              = 228,
    KEY_RSHIFT             = 229,
    KEY_RALT               = 230,
    KEY_RGUI               = 231,

    KEY_MODE               = 257,
    KEY_AUDIONEXT          = 258,
    KEY_AUDIOPREV          = 259,
    KEY_AUDIOSTOP          = 260,
    KEY_AUDIOPLAY          = 261,
    KEY_AUDIOMUTE          = 262,
    KEY_MEDIASELECT        = 263,
    KEY_WWW                = 264,
    KEY_MAIL               = 265,
    KEY_CALCULATOR         = 266,
    KEY_COMPUTER           = 267,
    KEY_AC_SEARCH          = 268,
    KEY_AC_HOME            = 269,
    KEY_AC_BACK            = 270,
    KEY_AC_FORWARD         = 271,
    KEY_AC_STOP            = 272,
    KEY_AC_REFRESH         = 273,
    KEY_AC_BOOKMARKS       = 274,

    KEY_BRIGHTNESSDOWN     = 275,
    KEY_BRIGHTNESSUP       = 276,
    KEY_DISPLAYSWITCH      = 277,
    KEY_KBDILLUMTOGGLE     = 278,
    KEY_KBDILLUMDOWN       = 279,
    KEY_KBDILLUMUP         = 280,
    KEY_EJECT              = 281,
    KEY_SLEEP              = 282,
    KEY_APP1               = 283,
    KEY_APP2               = 284,

    KEY_AUDIOREWIND        = 285,
    KEY_AUDIOFASTFORWARD   = 286,
    KEY_SOFTLEFT           = 287,
    KEY_SOFTRIGHT          = 288,
    KEY_CALL               = 289,
    KEY_ENDCALL            = 290,
} FrameKey;

typedef enum {
    KEY_SHIFT_NONE  = 0x00,
    KEY_SHIFT_LEFT  = 0x01,
    KEY_SHIFT_RIGHT = 0x02,
    KEY_CTRL_LEFT   = 0x04,
    KEY_CTRL_RIGHT  = 0x08,
    KEY_ALT_LEFT    = 0x10,
    KEY_ALT_RIGHT   = 0x20,
} FrameKeyShift;

typedef struct {
    FrameEventType type;
    union {
        struct {
            FrameKeyShift shift;
            FrameKey keycode;
            u32 character;
        } key;
        struct {
            int x;
            int y;
            MouseButton button;
        } mouse;
    };
} FrameEvent;

typedef struct {
    // Frame parameters
    cstr title;              // Title of window
    int width;               // Width of inner part in pixels
    int height;              // Height of inner part in pixels
    Array(GfxLayer*) layers; // Graphics layers that are rendered in window
    bool done;               // True if frame is to be closed
    bool resizable; // True if you are allowed to resize the window (fullscreen
                    // always works)

    // FPS Tracking
    TimePoint last_time; // Last frame time point
    u64 frame_count;     // Total frames since open
    f64 fps;             // Most recent calculation of FPS

    // Event queue
    Array(FrameEvent) events; // Event queue

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

//
// Frame API
//

Frame frame_open(int width, int height, bool resizable, cstr title);
bool frame_loop(Frame* f);
void frame_done(Frame* f);
u32* frame_add_pixels_layer(Frame* f, int width, int height);
f64 frame_fps(Frame* f);

void frame_fullscreen(Frame* f, bool enable);

//
// Event management
//

void frame_event_enqueue(Frame* f, FrameEvent event);
void frame_event_clear(Frame* f);
FrameEvent frame_event_poll(Frame* f);

bool frame_event_is_shift_pressed(const FrameEvent* ev);
bool frame_event_is_ctrl_pressed(const FrameEvent* ev);
bool frame_event_is_alt_pressed(const FrameEvent* ev);

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
// Common helpers

internal inline void frame_update_timing(Frame* f) {
    if (!f) {
        return;
    }
    f->frame_count++;
    TimePoint now   = time_now();
    TimeDuration dt = time_elapsed(f->last_time, now);
    f64 secs        = time_secs(dt);
    if (secs > 0.0) {
        f->fps = 1.0 / secs;
    }
    f->last_time = now;
}

//------------------------------------------------------------------------------
// Linux frame implementation

#    if KORE_OS_LINUX

global_variable Atom g_wm_delete_window = None;

internal inline void frame_apply_linux_size_hints(Frame* f, bool fixed) {
    if (!f || !f->display || !f->window) {
        return;
    }

    XSizeHints hints = {0};
    if (fixed) {
        hints.flags      = PMinSize | PMaxSize;
        hints.min_width  = f->width;
        hints.max_width  = f->width;
        hints.min_height = f->height;
        hints.max_height = f->height;
    } else {
        hints.flags = 0; // Clear fixed constraints to allow fullscreen changes
    }
    XSetWMNormalHints(f->display, f->window, &hints);
}

internal inline FrameKey frame_x11_keysym_to_key(KeySym sym) {
    if (sym >= XK_a && sym <= XK_z) {
        return (FrameKey)(KEY_A + (sym - XK_a));
    }
    if (sym >= XK_A && sym <= XK_Z) {
        return (FrameKey)(KEY_A + (sym - XK_A));
    }
    if (sym >= XK_1 && sym <= XK_9) {
        return (FrameKey)(KEY_1 + (sym - XK_1));
    }
    if (sym == XK_0) {
        return KEY_0;
    }
    if (sym >= XK_F1 && sym <= XK_F24) {
        return (FrameKey)(KEY_F1 + (sym - XK_F1));
    }
    if (sym >= XK_KP_0 && sym <= XK_KP_9) {
        return (FrameKey)(KEY_KP_0 + (sym - XK_KP_0));
    }

    switch (sym) {
    case XK_Return:
        return KEY_ENTER;
    case XK_Escape:
        return KEY_ESCAPE;
    case XK_BackSpace:
        return KEY_BACKSPACE;
    case XK_Tab:
        return KEY_TAB;
    case XK_space:
        return KEY_SPACE;
    case XK_minus:
        return KEY_MINUS;
    case XK_equal:
        return KEY_EQUALS;
    case XK_bracketleft:
        return KEY_LEFTBRACKET;
    case XK_bracketright:
        return KEY_RIGHTBRACKET;
    case XK_backslash:
        return KEY_BACKSLASH;
    case XK_semicolon:
        return KEY_SEMICOLON;
    case XK_apostrophe:
        return KEY_APOSTROPHE;
    case XK_grave:
        return KEY_GRAVE;
    case XK_comma:
        return KEY_COMMA;
    case XK_period:
        return KEY_PERIOD;
    case XK_slash:
        return KEY_SLASH;

    case XK_KP_Add:
        return KEY_KP_PLUS;
    case XK_KP_Subtract:
        return KEY_KP_MINUS;
    case XK_KP_Multiply:
        return KEY_KP_MULTIPLY;
    case XK_KP_Divide:
        return KEY_KP_DIVIDE;
    case XK_KP_Enter:
        return KEY_KP_ENTER;
    case XK_KP_Decimal:
        return KEY_KP_PERIOD;
    case XK_KP_Equal:
        return KEY_KP_EQUALS;
    case XK_KP_Separator:
        return KEY_KP_COMMA;

    case XK_Caps_Lock:
        return KEY_CAPSLOCK;
    case XK_Num_Lock:
        return KEY_NUMLOCKCLEAR;
    case XK_Print:
        return KEY_PRINTSCREEN;
    case XK_Scroll_Lock:
        return KEY_SCROLLLOCK;
    case XK_Pause:
        return KEY_PAUSE;

    case XK_Insert:
        return KEY_INSERT;
    case XK_Delete:
        return KEY_DELETE;
    case XK_Home:
        return KEY_HOME;
    case XK_End:
        return KEY_END;
    case XK_Prior:
        return KEY_PAGEUP;
    case XK_Next:
        return KEY_PAGEDOWN;

    case XK_Left:
        return KEY_LEFT;
    case XK_Right:
        return KEY_RIGHT;
    case XK_Up:
        return KEY_UP;
    case XK_Down:
        return KEY_DOWN;

    case XK_Shift_L:
        return KEY_LSHIFT;
    case XK_Shift_R:
        return KEY_RSHIFT;
    case XK_Control_L:
        return KEY_LCTRL;
    case XK_Control_R:
        return KEY_RCTRL;
    case XK_Alt_L:
        return KEY_LALT;
    case XK_Alt_R:
        return KEY_RALT;
    case XK_Super_L:
        return KEY_LGUI;
    case XK_Super_R:
        return KEY_RGUI;
    case XK_Menu:
        return KEY_MENU;

    default:
        return KEY_UNKNOWN;
    }
}

internal inline FrameKeyShift frame_x11_modifiers(unsigned int state,
                                                  KeySym sym) {
    FrameKeyShift mods = 0;
    if (state & ShiftMask) {
        mods |= KEY_SHIFT_LEFT;
    }
    if (state & ControlMask) {
        mods |= KEY_CTRL_LEFT;
    }
    if (state & Mod1Mask) {
        mods |= KEY_ALT_LEFT;
    }

    switch (sym) {
    case XK_Shift_L:
        mods |= KEY_SHIFT_LEFT;
        break;
    case XK_Shift_R:
        mods |= KEY_SHIFT_RIGHT;
        break;
    case XK_Control_L:
        mods |= KEY_CTRL_LEFT;
        break;
    case XK_Control_R:
        mods |= KEY_CTRL_RIGHT;
        break;
    case XK_Alt_L:
        mods |= KEY_ALT_LEFT;
        break;
    case XK_Alt_R:
        mods |= KEY_ALT_RIGHT;
        break;
    default:
        break;
    }

    return mods;
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

// TODO: Implement the key and mouse events here and for Windows
Frame frame_open(int width, int height, bool resizable, cstr title) {
    Frame f = {
        .title       = title,
        .width       = width,
        .height      = height,
        .resizable   = resizable,
        .done        = false,
        .last_time   = 0,
        .frame_count = 0,
        .fps         = 0.0,
        .events      = nullptr,
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

    if (!f.resizable) {
        frame_apply_linux_size_hints(&f, true);
    }

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

    // Disable vsync to expose raw throughput (if supported)
    {
        typedef void (*PFNGLXSWAPINTERVALEXTPROC)(Display*, GLXDrawable, int);
        typedef int (*PFNGLXSWAPINTERVALMESAPROC)(unsigned int);
        typedef int (*PFNGLXSWAPINTERVALSGIPROC)(int);

        PFNGLXSWAPINTERVALEXTPROC glXSwapIntervalEXT =
            (PFNGLXSWAPINTERVALEXTPROC)glXGetProcAddress(
                (const GLubyte*)"glXSwapIntervalEXT");
        GLXDrawable drawable = glXGetCurrentDrawable();
        if (glXSwapIntervalEXT && drawable) {
            glXSwapIntervalEXT(f.display, drawable, 0);
        } else {
            PFNGLXSWAPINTERVALMESAPROC glXSwapIntervalMESA =
                (PFNGLXSWAPINTERVALMESAPROC)glXGetProcAddress(
                    (const GLubyte*)"glXSwapIntervalMESA");
            if (glXSwapIntervalMESA) {
                glXSwapIntervalMESA(0);
            } else {
                PFNGLXSWAPINTERVALSGIPROC glXSwapIntervalSGI =
                    (PFNGLXSWAPINTERVALSGIPROC)glXGetProcAddress(
                        (const GLubyte*)"glXSwapIntervalSGI");
                if (glXSwapIntervalSGI) {
                    glXSwapIntervalSGI(0);
                }
            }
        }
    }

    if (!gfx_init()) {
        eprn("Failed to initialize graphics system");
        frame_cleanup(&f);
        exit(EXIT_FAILURE);
    }

    XFlush(f.display);
    f.last_time   = time_now();
    f.frame_count = 0;
    f.fps         = 0.0;

    return f;
}

bool frame_loop(Frame* f) {
    if (!f || f->done) {
        if (f) {
            frame_cleanup(f);
            array_free(f->events);
        }
        return false;
    }

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
        case KeyRelease: {
            FrameEvent ev  = {.type = (event.type == KeyPress)
                                          ? FRAME_EVENT_KEY_DOWN
                                          : FRAME_EVENT_KEY_UP};
            KeySym sym     = XLookupKeysym(&event.xkey, 0);
            ev.key.keycode = frame_x11_keysym_to_key(sym);
            ev.key.shift   = frame_x11_modifiers(event.xkey.state, sym);
            ev.key.character =
                0; // Default to no character unless resolved below

            if (ev.type == FRAME_EVENT_KEY_DOWN) {
                char buf[8];
                int len = XLookupString(
                    &event.xkey, buf, (int)sizeof(buf), NULL, NULL);
                if (len > 0) {
                    ev.key.character = (u8)buf[0];
                }
            }

            frame_event_enqueue(f, ev);
        } break;

        case ButtonPress:
        case ButtonRelease: {
            FrameEvent ev   = {.type = (event.type == ButtonPress)
                                           ? FRAME_EVENT_MOUSE_BUTTON_DOWN
                                           : FRAME_EVENT_MOUSE_BUTTON_UP};
            ev.mouse.x      = event.xbutton.x;
            ev.mouse.y      = event.xbutton.y;
            ev.mouse.button = 0;

            switch (event.xbutton.button) {
            case Button1:
                ev.mouse.button = MOUSE_BUTTON_LEFT;
                break;
            case Button2:
                ev.mouse.button = MOUSE_BUTTON_MIDDLE;
                break;
            case Button3:
                ev.mouse.button = MOUSE_BUTTON_RIGHT;
                break;
            default:
                break;
            }

            frame_event_enqueue(f, ev);
        } break;

        case MotionNotify: {
            FrameEvent ev   = {.type = FRAME_EVENT_MOUSE_MOVE};
            ev.mouse.x      = event.xmotion.x;
            ev.mouse.y      = event.xmotion.y;
            ev.mouse.button = 0;
            frame_event_enqueue(f, ev);
        } break;

        default:
            break;
        }
    }

    if (!running || !f->display || !f->window || !f->glx_ctx) {
        frame_cleanup(f);
        array_free(f->events);
        return false;
    }

    XWindowAttributes wa;
    XGetWindowAttributes(f->display, f->window, &wa);

    glXMakeCurrent(f->display, f->window, f->glx_ctx);
    gfx_render(f->layers, array_count(f->layers), wa.width, wa.height);
    glXSwapBuffers(f->display, f->window);

    // Update timing/FPS after presenting
    frame_update_timing(f);
    return true;
}

void frame_fullscreen(Frame* f, bool enable) {
    if (!f || !f->display || !f->window) {
        return;
    }

    static Atom wm_state        = None;
    static Atom fullscreen_atom = None;
    if (wm_state == None) {
        wm_state = XInternAtom(f->display, "_NET_WM_STATE", False);
    }
    if (fullscreen_atom == None) {
        fullscreen_atom =
            XInternAtom(f->display, "_NET_WM_STATE_FULLSCREEN", False);
    }

    XEvent xev               = {0};
    xev.type                 = ClientMessage;
    xev.xclient.window       = f->window;
    xev.xclient.message_type = wm_state;
    xev.xclient.format       = 32;
    xev.xclient.data.l[0]    = enable ? 1 : 0; // _NET_WM_STATE_ADD / REMOVE
    xev.xclient.data.l[1]    = fullscreen_atom;
    xev.xclient.data.l[2]    = 0;
    xev.xclient.data.l[3]    = 1; // normal source

    if (!f->resizable) {
        frame_apply_linux_size_hints(f, !enable);
    }

    XSendEvent(f->display,
               DefaultRootWindow(f->display),
               False,
               SubstructureRedirectMask | SubstructureNotifyMask,
               &xev);
    XFlush(f->display);
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

internal inline FrameKey frame_win32_vk_to_key(WPARAM vk) {
    if (vk >= 'A' && vk <= 'Z') {
        return (FrameKey)(KEY_A + (vk - 'A'));
    }
    if (vk >= '0' && vk <= '9') {
        return vk == '0' ? KEY_0 : (FrameKey)(KEY_1 + (vk - '1'));
    }
    if (vk >= VK_F1 && vk <= VK_F24) {
        return (FrameKey)(KEY_F1 + (vk - VK_F1));
    }
    if (vk >= VK_NUMPAD0 && vk <= VK_NUMPAD9) {
        return (FrameKey)(KEY_KP_0 + (vk - VK_NUMPAD0));
    }

    switch (vk) {
    case VK_RETURN:
        return KEY_ENTER;
    case VK_ESCAPE:
        return KEY_ESCAPE;
    case VK_BACK:
        return KEY_BACKSPACE;
    case VK_TAB:
        return KEY_TAB;
    case VK_SPACE:
        return KEY_SPACE;

    case VK_OEM_MINUS:
        return KEY_MINUS;
    case VK_OEM_PLUS:
        return KEY_EQUALS;
    case VK_OEM_4:
        return KEY_LEFTBRACKET;
    case VK_OEM_6:
        return KEY_RIGHTBRACKET;
    case VK_OEM_5:
        return KEY_BACKSLASH;
    case VK_OEM_1:
        return KEY_SEMICOLON;
    case VK_OEM_7:
        return KEY_APOSTROPHE;
    case VK_OEM_3:
        return KEY_GRAVE;
    case VK_OEM_COMMA:
        return KEY_COMMA;
    case VK_OEM_PERIOD:
        return KEY_PERIOD;
    case VK_OEM_2:
        return KEY_SLASH;

    case VK_CAPITAL:
        return KEY_CAPSLOCK;
    case VK_SNAPSHOT:
        return KEY_PRINTSCREEN;
    case VK_SCROLL:
        return KEY_SCROLLLOCK;
    case VK_PAUSE:
        return KEY_PAUSE;
    case VK_INSERT:
        return KEY_INSERT;
    case VK_DELETE:
        return KEY_DELETE;
    case VK_HOME:
        return KEY_HOME;
    case VK_END:
        return KEY_END;
    case VK_PRIOR:
        return KEY_PAGEUP;
    case VK_NEXT:
        return KEY_PAGEDOWN;

    case VK_LEFT:
        return KEY_LEFT;
    case VK_RIGHT:
        return KEY_RIGHT;
    case VK_UP:
        return KEY_UP;
    case VK_DOWN:
        return KEY_DOWN;

    case VK_NUMLOCK:
        return KEY_NUMLOCKCLEAR;
    case VK_DIVIDE:
        return KEY_KP_DIVIDE;
    case VK_MULTIPLY:
        return KEY_KP_MULTIPLY;
    case VK_SUBTRACT:
        return KEY_KP_MINUS;
    case VK_ADD:
        return KEY_KP_PLUS;
    case VK_DECIMAL:
        return KEY_KP_PERIOD;

    case VK_LSHIFT:
        return KEY_LSHIFT;
    case VK_RSHIFT:
        return KEY_RSHIFT;
    case VK_LCONTROL:
        return KEY_LCTRL;
    case VK_RCONTROL:
        return KEY_RCTRL;
    case VK_LMENU:
        return KEY_LALT;
    case VK_RMENU:
        return KEY_RALT;
    case VK_LWIN:
        return KEY_LGUI;
    case VK_RWIN:
        return KEY_RGUI;
    case VK_APPS:
        return KEY_MENU;

    default:
        return KEY_UNKNOWN;
    }
}

internal inline FrameKeyShift frame_win32_modifiers(void) {
    FrameKeyShift mods = 0;
    if (GetKeyState(VK_LSHIFT) & 0x8000) {
        mods |= KEY_SHIFT_LEFT;
    }
    if (GetKeyState(VK_RSHIFT) & 0x8000) {
        mods |= KEY_SHIFT_RIGHT;
    }
    if (GetKeyState(VK_LCONTROL) & 0x8000) {
        mods |= KEY_CTRL_LEFT;
    }
    if (GetKeyState(VK_RCONTROL) & 0x8000) {
        mods |= KEY_CTRL_RIGHT;
    }
    if (GetKeyState(VK_LMENU) & 0x8000) {
        mods |= KEY_ALT_LEFT;
    }
    if (GetKeyState(VK_RMENU) & 0x8000) {
        mods |= KEY_ALT_RIGHT;
    }
    return mods;
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

    case WM_KEYDOWN:
    case WM_SYSKEYDOWN:
    case WM_KEYUP:
    case WM_SYSKEYUP:
        if (!f) {
            return DefWindowProc(hwnd, msg, wParam, lParam);
        }
        {
            FrameEvent ev = {.type = (msg == WM_KEYDOWN || msg == WM_SYSKEYDOWN)
                                         ? FRAME_EVENT_KEY_DOWN
                                         : FRAME_EVENT_KEY_UP};
            ev.key.keycode   = frame_win32_vk_to_key(wParam);
            ev.key.shift     = frame_win32_modifiers();
            ev.key.character = 0;

            if (ev.type == FRAME_EVENT_KEY_DOWN) {
                UINT ch = MapVirtualKeyA((UINT)wParam, MAPVK_VK_TO_CHAR);
                if (ch && !(ch & 0x80000000)) {
                    ev.key.character = ch & 0xFFFF;
                }
            }
            frame_event_enqueue(f, ev);
        }
        return 0;

    case WM_MOUSEMOVE:
    case WM_LBUTTONDOWN:
    case WM_LBUTTONUP:
    case WM_RBUTTONDOWN:
    case WM_RBUTTONUP:
    case WM_MBUTTONDOWN:
    case WM_MBUTTONUP:
        if (!f) {
            return DefWindowProc(hwnd, msg, wParam, lParam);
        }
        {
            FrameEvent ev   = {0};
            ev.mouse.x      = GET_X_LPARAM(lParam);
            ev.mouse.y      = GET_Y_LPARAM(lParam);
            ev.mouse.button = 0;

            switch (msg) {
            case WM_MOUSEMOVE:
                ev.type = FRAME_EVENT_MOUSE_MOVE;
                break;
            case WM_LBUTTONDOWN:
            case WM_LBUTTONUP:
                ev.type         = (msg == WM_LBUTTONDOWN)
                                      ? FRAME_EVENT_MOUSE_BUTTON_DOWN
                                      : FRAME_EVENT_MOUSE_BUTTON_UP;
                ev.mouse.button = MOUSE_BUTTON_LEFT;
                break;
            case WM_RBUTTONDOWN:
            case WM_RBUTTONUP:
                ev.type         = (msg == WM_RBUTTONDOWN)
                                      ? FRAME_EVENT_MOUSE_BUTTON_DOWN
                                      : FRAME_EVENT_MOUSE_BUTTON_UP;
                ev.mouse.button = MOUSE_BUTTON_RIGHT;
                break;
            case WM_MBUTTONDOWN:
            case WM_MBUTTONUP:
                ev.type         = (msg == WM_MBUTTONDOWN)
                                      ? FRAME_EVENT_MOUSE_BUTTON_DOWN
                                      : FRAME_EVENT_MOUSE_BUTTON_UP;
                ev.mouse.button = MOUSE_BUTTON_MIDDLE;
                break;
            default:
                break;
            }

            if (ev.type != FRAME_EVENT_NONE) {
                frame_event_enqueue(f, ev);
            }
        }
        return 0;

    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

Frame frame_open(int width, int height, bool resizable, const char* title) {
    Frame f = {
        .title       = title,
        .width       = width,
        .height      = height,
        .resizable   = resizable,
        .last_time   = 0,
        .frame_count = 0,
        .fps         = 0.0,
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

    DWORD style = f.resizable ? (WS_OVERLAPPEDWINDOW | WS_VISIBLE)
                              : (WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU |
                                 WS_MINIMIZEBOX | WS_VISIBLE);

    RECT rc = {0, 0, f.width, f.height};
    AdjustWindowRect(&rc, style & ~WS_VISIBLE, FALSE);

    f.hwnd = CreateWindowEx(WS_EX_CLIENTEDGE,
                            f.title,
                            f.title,
                            style,
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

    // Disable vsync to expose raw throughput (if supported)
    {
        typedef BOOL(WINAPI * PFNWGLSWAPINTERVALEXTPROC)(int);
        PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT =
            (PFNWGLSWAPINTERVALEXTPROC)wglGetProcAddress("wglSwapIntervalEXT");
        if (wglSwapIntervalEXT) {
            wglSwapIntervalEXT(0);
        }
    }

    if (!gfx_init()) {
        eprn("Failed to initialize graphics system");
        frame_cleanup(&f);
        exit(EXIT_FAILURE);
    }

    f.last_time   = time_now();
    f.frame_count = 0;
    f.fps         = 0.0;

    return f;
}

bool frame_loop(Frame* f) {
    if (!f || f->done) {
        if (f) {
            frame_cleanup(f);
            array_free(f->events);
        }
        return false;
    }

    if (f->hwnd) {
        SetWindowLongPtr(f->hwnd, GWLP_USERDATA, (LONG_PTR)f);
    }

    MSG msg;
    while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
        if (msg.message == WM_QUIT) {
            frame_cleanup(f);
            array_free(f->events);
            return false;
        }
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    if (!f->hwnd) {
        frame_cleanup(f);
        array_free(f->events);
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

    frame_update_timing(f);
    return true;
}

void frame_fullscreen(Frame* f, bool enable) {
    if (!f || !f->hwnd) {
        return;
    }

    static WINDOWPLACEMENT prev_wp = {.length = sizeof(WINDOWPLACEMENT)};
    static DWORD prev_style        = 0;
    static DWORD prev_ex_style     = 0;
    static bool is_fullscreen      = false;

    if (enable && !is_fullscreen) {
        prev_style     = (DWORD)GetWindowLongPtr(f->hwnd, GWL_STYLE);
        prev_ex_style  = (DWORD)GetWindowLongPtr(f->hwnd, GWL_EXSTYLE);
        prev_wp.length = sizeof(prev_wp);
        GetWindowPlacement(f->hwnd, &prev_wp);

        MONITORINFO mi = {.cbSize = sizeof(mi)};
        if (GetMonitorInfo(MonitorFromWindow(f->hwnd, MONITOR_DEFAULTTONEAREST),
                           &mi)) {
            SetWindowLongPtr(
                f->hwnd, GWL_STYLE, prev_style & ~WS_OVERLAPPEDWINDOW);
            SetWindowLongPtr(
                f->hwnd, GWL_EXSTYLE, prev_ex_style & ~WS_EX_CLIENTEDGE);
            SetWindowPos(f->hwnd,
                         HWND_TOP,
                         mi.rcMonitor.left,
                         mi.rcMonitor.top,
                         mi.rcMonitor.right - mi.rcMonitor.left,
                         mi.rcMonitor.bottom - mi.rcMonitor.top,
                         SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
            is_fullscreen = true;
        }
    } else if (!enable && is_fullscreen) {
        SetWindowLongPtr(f->hwnd, GWL_STYLE, prev_style);
        SetWindowLongPtr(f->hwnd, GWL_EXSTYLE, prev_ex_style);
        SetWindowPlacement(f->hwnd, &prev_wp);
        SetWindowPos(f->hwnd,
                     NULL,
                     0,
                     0,
                     0,
                     0,
                     SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER |
                         SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
        is_fullscreen = false;
    }
}

//------------------------------------------------------------------------------

#    else
#        error "Unsupported OS"
#    endif // OS_WINDOWS

//------------------------------------------------------------------------------
// Common frame functions

void frame_done(Frame* f) {
    if (!f) {
        return;
    }
    f->done = true;
}

u32* frame_add_pixels_layer(Frame* f, int width, int height) {
    GfxLayer* layer = gfx_layer_create(width, height, NULL);
    if (!layer) {
        eprn("Failed to create graphics layer");
        return NULL;
    }
    array_push(f->layers, layer);
    return (u32*)gfx_layer_get_pixels(layer);
}

f64 frame_fps(Frame* f) { return f->fps; }

void frame_free_pixels_layer(u32* pixels) {
    KORE_UNUSED(pixels); // Layer teardown happens in frame_cleanup
}

//------------------------------------------------------------------------------
// Event functions

void frame_event_enqueue(Frame* f, FrameEvent event) {
    if (!f) {
        return;
    }
    array_push(f->events, event);
}

void frame_event_clear(Frame* f) {
    if (!f) {
        return;
    }
    array_clear(f->events);
}

FrameEvent frame_event_poll(Frame* f) {
    FrameEvent ev = {.type = FRAME_EVENT_NONE};
    if (array_count(f->events) == 0) {
        return ev;
    }
    ev = f->events[0];
    // Remove the event from the queue
    for (u64 i = 1; i < array_count(f->events); ++i) {
        f->events[i - 1] = f->events[i];
    }
    array_pop(f->events);
    return ev;
}

bool frame_event_is_shift_pressed(const FrameEvent* ev) {
    if (!ev) {
        return false;
    }
    return (ev->key.shift & (KEY_SHIFT_LEFT | KEY_SHIFT_RIGHT)) != 0;
}

bool frame_event_is_ctrl_pressed(const FrameEvent* ev) {
    if (!ev) {
        return false;
    }
    return (ev->key.shift & (KEY_CTRL_LEFT | KEY_CTRL_RIGHT)) != 0;
}

bool frame_event_is_alt_pressed(const FrameEvent* ev) {
    if (!ev) {
        return false;
    }
    return (ev->key.shift & (KEY_ALT_LEFT | KEY_ALT_RIGHT)) != 0;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

#endif // KORE_IMPLEMENTATION
