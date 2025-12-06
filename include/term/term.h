
//------------------------------------------------------------------------------
// Terminal module
//
// Copyright (C)2025 Matt Davies, all rights reserved
//------------------------------------------------------------------------------

#pragma once

//------------------------------------------------------------------------------

#include <kore/kore.h>

//------------------------------------------------------------------------------
// Terminal information
//------------------------------------------------------------------------------

typedef struct {
    u16 width;
    u16 height;
} TermSize;

TermSize term_size_get(void);

//------------------------------------------------------------------------------
// Terminal interaction API
//------------------------------------------------------------------------------

typedef enum {
    TERM_EVENT_NONE,
    TERM_EVENT_KEY,
    TERM_EVENT_RESIZE,
} TermEventKind;

typedef struct {
    TermEventKind kind;
    union {
        char key;
        TermSize size;
    };
} TermEvent;

typedef struct Term {
    TermSize size;
    Array(TermEvent) event_queue;
    bool initialised;
    bool running;
} Term;

void term_init();
void term_done();
bool term_loop();
TermEvent term_poll_event();

void term_cursor_show();
void term_cursor_hide();

void term_cursor_goto(int x, int y);
void term_cursor_move(int dx, int dy);
void term_cursor_up(int delta);
void term_cursor_down(int delta);
void term_cursor_right(int delta);
void term_cursor_left(int delta);
void term_cursor_home(void);

//------------------------------------------------------------------------------
// Terminal frame buffer API
//------------------------------------------------------------------------------

typedef struct {
    u16 x;
    u16 y;
    u16 width;
    u16 height;
} TermRect;

void term_fb_cls(u32 ink, u32 paper);
void term_fb_clip_rect(TermRect rect,
                       TermRect* out_clipped_rect,
                       TermRect* out_local_rect);

//
// Colouring
//

u32 term_rgb(u8 r, u8 g, u8 b);
u32 term_rgba(u8 r, u8 g, u8 b, u8 a);
u32 term_blend(u32 dest, u32 src, f32 alpha);

// Individual layers
void term_fb_rect_ink(TermRect rect, u32 colour);
void term_fb_rect_paper(TermRect rect, u32 colour);
void term_fb_rect_colour(TermRect rect, u32 ink, u32 paper);
void term_fb_rect_char(TermRect rect, u32 ch);

// Character painting
void term_fb_rect(TermRect rect, u32 ch, u32 ink, u32 paper);

// Writing strings
void term_fb_write(u16 x, u16 y, cstr string);
void term_fb_formatv(u16 x, u16 y, cstr fmt, va_list args);
void term_fb_format(u16 x, u16 y, cstr fmt, ...);

//
// Presentation
//

void term_fb_present();

//------------------------------------------------------------------------------
// Terminal information dumping
//------------------------------------------------------------------------------

void dump_term_size();

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// I M P L E M E N T A T I O N
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

#ifdef KORE_IMPLEMENTATION

#    include <locale.h>
#    include <wchar.h>

global_variable Term g_term;
global_variable bool g_cursor_visible      = true;

global_variable Array(u32) g_term_fb_chars = NULL;
global_variable Array(u32) g_term_fb_ink   = NULL;
global_variable Array(u32) g_term_fb_paper = NULL;
global_variable Array(u8) g_term_fb_dirty  = NULL;
global_variable TermSize g_term_fb_size    = {0};
global_variable Arena g_term_arena;

enum { TERM_FB_CHAR_WIDE_TAIL = 0xFFFFFFFFu };

internal void _term_queue_event(TermEvent event);
internal void _term_alt_enter();
internal void _term_alt_leave();
internal void _term_raw_enter();
internal void _term_raw_leave();

internal void _term_fb_resize(u16 width, u16 height);
internal void _term_fb_done();

internal void _term_start(void);
internal void _term_stop(void);

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// WINDOWS IMPLEMENTATION
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

#    if KORE_OS_WINDOWS

global_variable HANDLE g_term_console_input       = INVALID_HANDLE_VALUE;
global_variable DWORD g_term_console_input_mode   = 0;
global_variable bool g_term_console_mode_captured = false;

//------------------------------------------------------------------------------
// Windows implementation of wcwidth

typedef struct {
    int first;
    int last;
} TermInterval;

internal int _term_bisearch(wchar_t ucs, const TermInterval* table, int max) {
    int min = 0;
    int mid;

    if (ucs < table[0].first || ucs > table[max].last) {
        return 0;
    }
    while (max >= min) {
        mid = (min + max) / 2;
        if (ucs > table[mid].last) {
            min = mid + 1;
        } else if (ucs < table[mid].first) {
            max = mid - 1;
        } else {
            return 1;
        }
    }

    return 0;
}

int wcwidth(u32 ucs) {
    /* sorted list of non-overlapping intervals of non-spacing characters */
    /* generated by "uniset +cat=Me +cat=Mn +cat=Cf -00AD +1160-11FF +200B c" */
    static const TermInterval combining[] = {
        {0x0300, 0x036F},   {0x0483, 0x0486},   {0x0488, 0x0489},
        {0x0591, 0x05BD},   {0x05BF, 0x05BF},   {0x05C1, 0x05C2},
        {0x05C4, 0x05C5},   {0x05C7, 0x05C7},   {0x0600, 0x0603},
        {0x0610, 0x0615},   {0x064B, 0x065E},   {0x0670, 0x0670},
        {0x06D6, 0x06E4},   {0x06E7, 0x06E8},   {0x06EA, 0x06ED},
        {0x070F, 0x070F},   {0x0711, 0x0711},   {0x0730, 0x074A},
        {0x07A6, 0x07B0},   {0x07EB, 0x07F3},   {0x0901, 0x0902},
        {0x093C, 0x093C},   {0x0941, 0x0948},   {0x094D, 0x094D},
        {0x0951, 0x0954},   {0x0962, 0x0963},   {0x0981, 0x0981},
        {0x09BC, 0x09BC},   {0x09C1, 0x09C4},   {0x09CD, 0x09CD},
        {0x09E2, 0x09E3},   {0x0A01, 0x0A02},   {0x0A3C, 0x0A3C},
        {0x0A41, 0x0A42},   {0x0A47, 0x0A48},   {0x0A4B, 0x0A4D},
        {0x0A70, 0x0A71},   {0x0A81, 0x0A82},   {0x0ABC, 0x0ABC},
        {0x0AC1, 0x0AC5},   {0x0AC7, 0x0AC8},   {0x0ACD, 0x0ACD},
        {0x0AE2, 0x0AE3},   {0x0B01, 0x0B01},   {0x0B3C, 0x0B3C},
        {0x0B3F, 0x0B3F},   {0x0B41, 0x0B43},   {0x0B4D, 0x0B4D},
        {0x0B56, 0x0B56},   {0x0B82, 0x0B82},   {0x0BC0, 0x0BC0},
        {0x0BCD, 0x0BCD},   {0x0C3E, 0x0C40},   {0x0C46, 0x0C48},
        {0x0C4A, 0x0C4D},   {0x0C55, 0x0C56},   {0x0CBC, 0x0CBC},
        {0x0CBF, 0x0CBF},   {0x0CC6, 0x0CC6},   {0x0CCC, 0x0CCD},
        {0x0CE2, 0x0CE3},   {0x0D41, 0x0D43},   {0x0D4D, 0x0D4D},
        {0x0DCA, 0x0DCA},   {0x0DD2, 0x0DD4},   {0x0DD6, 0x0DD6},
        {0x0E31, 0x0E31},   {0x0E34, 0x0E3A},   {0x0E47, 0x0E4E},
        {0x0EB1, 0x0EB1},   {0x0EB4, 0x0EB9},   {0x0EBB, 0x0EBC},
        {0x0EC8, 0x0ECD},   {0x0F18, 0x0F19},   {0x0F35, 0x0F35},
        {0x0F37, 0x0F37},   {0x0F39, 0x0F39},   {0x0F71, 0x0F7E},
        {0x0F80, 0x0F84},   {0x0F86, 0x0F87},   {0x0F90, 0x0F97},
        {0x0F99, 0x0FBC},   {0x0FC6, 0x0FC6},   {0x102D, 0x1030},
        {0x1032, 0x1032},   {0x1036, 0x1037},   {0x1039, 0x1039},
        {0x1058, 0x1059},   {0x1160, 0x11FF},   {0x135F, 0x135F},
        {0x1712, 0x1714},   {0x1732, 0x1734},   {0x1752, 0x1753},
        {0x1772, 0x1773},   {0x17B4, 0x17B5},   {0x17B7, 0x17BD},
        {0x17C6, 0x17C6},   {0x17C9, 0x17D3},   {0x17DD, 0x17DD},
        {0x180B, 0x180D},   {0x18A9, 0x18A9},   {0x1920, 0x1922},
        {0x1927, 0x1928},   {0x1932, 0x1932},   {0x1939, 0x193B},
        {0x1A17, 0x1A18},   {0x1B00, 0x1B03},   {0x1B34, 0x1B34},
        {0x1B36, 0x1B3A},   {0x1B3C, 0x1B3C},   {0x1B42, 0x1B42},
        {0x1B6B, 0x1B73},   {0x1DC0, 0x1DCA},   {0x1DFE, 0x1DFF},
        {0x200B, 0x200F},   {0x202A, 0x202E},   {0x2060, 0x2063},
        {0x206A, 0x206F},   {0x20D0, 0x20EF},   {0x302A, 0x302F},
        {0x3099, 0x309A},   {0xA806, 0xA806},   {0xA80B, 0xA80B},
        {0xA825, 0xA826},   {0xFB1E, 0xFB1E},   {0xFE00, 0xFE0F},
        {0xFE20, 0xFE23},   {0xFEFF, 0xFEFF},   {0xFFF9, 0xFFFB},
        {0x10A01, 0x10A03}, {0x10A05, 0x10A06}, {0x10A0C, 0x10A0F},
        {0x10A38, 0x10A3A}, {0x10A3F, 0x10A3F}, {0x1D167, 0x1D169},
        {0x1D173, 0x1D182}, {0x1D185, 0x1D18B}, {0x1D1AA, 0x1D1AD},
        {0x1D242, 0x1D244}, {0xE0001, 0xE0001}, {0xE0020, 0xE007F},
        {0xE0100, 0xE01EF}};

    /* test for 8-bit control characters */
    if (ucs == 0) {
        return 0;
    }
    if (ucs < 32 || (ucs >= 0x7f && ucs < 0xa0)) {
        return -1;
    }

    /* binary search in table of non-spacing characters */
    if (_term_bisearch(
            ucs, combining, sizeof(combining) / sizeof(TermInterval) - 1)) {
        return 0;
    }

    /* if we arrive here, ucs is not a combining or C0/C1 control character */

    return 1 +
           (ucs >= 0x1100 &&
            (ucs <= 0x115f || /* Hangul Jamo init. consonants */
             ucs == 0x2329 || ucs == 0x232a ||
             (ucs >= 0x2e80 && ucs <= 0xa4cf &&
              ucs != 0x303f) ||                  /* CJK ... Yi */
             (ucs >= 0xac00 && ucs <= 0xd7a3) || /* Hangul Syllables */
             (ucs >= 0xf900 &&
              ucs <= 0xfaff) || /* CJK Compatibility Ideographs */
             (ucs >= 0xfe10 && ucs <= 0xfe19) || /* Vertical forms */
             (ucs >= 0xfe30 && ucs <= 0xfe6f) || /* CJK Compatibility Forms */
             (ucs >= 0xff00 && ucs <= 0xff60) || /* Fullwidth Forms */
             (ucs >= 0xffe0 && ucs <= 0xffe6) ||
             (ucs >= 0x20000 && ucs <= 0x2fffd) ||
             (ucs >= 0x30000 && ucs <= 0x3fffd)));
}
//------------------------------------------------------------------------------

TermSize term_size_get(void) {
    TermSize term_size = {0};

#        if KORE_OS_WINDOWS
    CONSOLE_SCREEN_BUFFER_INFO info;
    if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &info)) {
        term_size.width  = info.srWindow.Right - info.srWindow.Left + 1;
        term_size.height = info.srWindow.Bottom - info.srWindow.Top + 1;
    } else {
        eprn("Failed to get terminal size on Windows");
        abort();
    }
#        elif KORE_OS_POSIX
    struct winsize w;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == 0) {
        term_size.width  = w.ws_col;
        term_size.height = w.ws_row;
    } else {
        eprn("Failed to get terminal size on Unix");
        abort();
    }
#        else
    eprn("Terminal size detection not implemented for this OS");
    abort();
#        endif

    return term_size;
}

//------------------------------------------------------------------------------

internal void _term_platform_init() {
    HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
    if (console != INVALID_HANDLE_VALUE) {
        DWORD mode;
        if (GetConsoleMode(console, &mode)) {
            SetConsoleMode(console, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
        }
    }

    TermSize size = term_size_get();
    g_term.size   = size;
    _term_fb_resize(size.width, size.height);

    TermEvent event;
    event.kind = TERM_EVENT_RESIZE;
    event.size = size;
    _term_queue_event(event);
}

//------------------------------------------------------------------------------

bool term_loop() {
    if (g_term.running) {

        // Windows-specific event polling can be added here
        // We need to check any console events and handle them
        HANDLE console = GetStdHandle(STD_INPUT_HANDLE);
        if (console != INVALID_HANDLE_VALUE) {
            DWORD events;
            GetNumberOfConsoleInputEvents(console, &events);
            while (events > 0) {
                INPUT_RECORD record;
                DWORD read;
                ReadConsoleInputA(console, &record, 1, &read);

                switch (record.EventType) {
                case WINDOW_BUFFER_SIZE_EVENT: {
                    g_term.size = term_size_get();
                    TermEvent event;
                    event.kind = TERM_EVENT_RESIZE;
                    event.size = g_term.size;
                    _term_queue_event(event);
                    _term_fb_resize(g_term.size.width, g_term.size.height);
                } break;

                case KEY_EVENT:
                    if (record.Event.KeyEvent.bKeyDown) {
                        TermEvent event;
                        event.kind = TERM_EVENT_KEY;
                        event.key  = record.Event.KeyEvent.uChar.AsciiChar;
                        _term_queue_event(event);
                    }
                    break;
                }
                events--;
            }
        }
        return true;
    } else {
        _term_stop();
        return false;
    }
}

//------------------------------------------------------------------------------

internal void _term_raw_enter(void) {
    HANDLE input = GetStdHandle(STD_INPUT_HANDLE);
    if (input == INVALID_HANDLE_VALUE) {
        eprn("Failed to get console input handle");
        abort();
    }

    DWORD mode = 0;
    if (!GetConsoleMode(input, &mode)) {
        eprn("Failed to get console input mode");
        abort();
    }

    g_term_console_input         = input;
    g_term_console_input_mode    = mode;
    g_term_console_mode_captured = true;

    DWORD raw_mode               = mode;
    raw_mode &=
        ~(ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT | ENABLE_PROCESSED_INPUT);
    raw_mode |= (ENABLE_WINDOW_INPUT | ENABLE_EXTENDED_FLAGS);
    raw_mode &= ~(ENABLE_QUICK_EDIT_MODE | ENABLE_INSERT_MODE);

    if (!SetConsoleMode(input, raw_mode)) {
        eprn("Failed to set raw console input mode");
        abort();
    }

    FlushConsoleInputBuffer(input);
}

internal void _term_raw_leave(void) {
    if (!g_term_console_mode_captured ||
        g_term_console_input == INVALID_HANDLE_VALUE) {
        return;
    }

    if (!SetConsoleMode(g_term_console_input, g_term_console_input_mode)) {
        eprn("Failed to restore console input mode");
        abort();
    }

    g_term_console_mode_captured = false;
}

//------------------------------------------------------------------------------

#    endif // KORE_OS_WINDOWS

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// LINUX/MACOSX IMPLEMENTATION
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

#    if KORE_OS_POSIX

#        include <signal.h>
#        include <string.h>
#        include <sys/ioctl.h>
#        include <termios.h>

// Signals when the terminal resizes
global_variable volatile sig_atomic_t g_term_resize_signal = 0;
global_variable struct termios g_term_original_tios;

//------------------------------------------------------------------------------

TermSize term_size_get(void) {
    TermSize term_size = {0};

    struct winsize w;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == 0) {
        term_size.width  = w.ws_col;
        term_size.height = w.ws_row;
    } else {
        eprn("Failed to get terminal size on Unix");
        abort();
    }

    return term_size;
}

//------------------------------------------------------------------------------

internal void _term_raw_enter(void) {
    tcgetattr(STDIN_FILENO, &g_term_original_tios);
    struct termios raw_tios = g_term_original_tios;

    raw_tios.c_lflag &= ~(ECHO | ICANON | ISIG | IEXTEN);
    raw_tios.c_iflag &= ~(IXON | ICRNL | BRKINT | INPCK | ISTRIP);
    raw_tios.c_oflag &= ~(OPOST);
    raw_tios.c_cflag |= (CS8);

    // Set read timeout
    raw_tios.c_cc[VMIN]  = 0;
    raw_tios.c_cc[VTIME] = 0;

    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw_tios);
}

internal void _term_raw_leave(void) {
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &g_term_original_tios);
}

internal void _term_raw_key(void) {
    char c;
    int nread = read(STDIN_FILENO, &c, 1);
    if (nread == 1) {
        TermEvent event;
        event.kind = TERM_EVENT_KEY;
        event.key  = c;
        _term_queue_event(event);
    }
}

//------------------------------------------------------------------------------

internal void _term_on_winch(int sig) {
    KORE_UNUSED(sig);
    g_term_resize_signal = 1;
}

internal void _term_install_resize_handler(void) {
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = _term_on_winch;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    sigaction(SIGWINCH, &sa, NULL);
}

internal void _term_remove_resize_handler(void) {
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = SIG_DFL;
    sigaction(SIGWINCH, &sa, NULL);
}

//------------------------------------------------------------------------------

internal void _term_platform_init(void) {
    _term_install_resize_handler();

    // Force the sending of at least one resize event so we can react to the
    // initial size.  Windows does this already, we make POSIX systems do the
    // same.
    g_term_resize_signal = 1;
}

//------------------------------------------------------------------------------

bool term_loop(void) {
    if (g_term.running) {

        // Check for terminal resize signal
        if (g_term_resize_signal) {
            g_term_resize_signal = 0;
            TermSize new_size    = term_size_get();
            if (new_size.width != g_term.size.width ||
                new_size.height != g_term.size.height) {
                g_term.size = new_size;
                TermEvent event;
                event.kind = TERM_EVENT_RESIZE;
                event.size = g_term.size;
                _term_queue_event(event);
                _term_fb_resize(new_size.width, new_size.height);
            }
        }
        _term_raw_key();
        return true;
    } else {
        // Unregister the signal handler
        _term_remove_resize_handler();
        _term_stop();
        return false;
    }
}

//------------------------------------------------------------------------------

#    endif // KORE_OS_POSIX

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// S H A R E D   I M P L E M E N T A T I O N
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

void dump_term_size(void) {
    prn(ANSI_YELLOW "┌──────────────────┬──────────┐");
    pr(ANSI_YELLOW "│ " ANSI_CYAN "Terminal Columns");
    prn(ANSI_YELLOW " │ " ANSI_GREEN "%5u " ANSI_YELLOW "   │",
        g_term.size.width);
    pr(ANSI_YELLOW "│ " ANSI_CYAN "Terminal Rows");
    prn(ANSI_YELLOW "    │ " ANSI_GREEN "%5u " ANSI_YELLOW "   │",
        g_term.size.height);
    prn(ANSI_YELLOW "└──────────────────┴──────────┘");
}

//------------------------------------------------------------------------------

internal void _term_queue_event(TermEvent event) {
    array_push(g_term.event_queue, event);
}

//------------------------------------------------------------------------------

internal void _term_alt_enter(void) { pr("\x1b[?1049h"); }

internal void _term_alt_leave(void) { pr("\x1b[?1049l"); }

//------------------------------------------------------------------------------

internal void _term_start(void) {
    _term_platform_init();
    _term_alt_enter();
    _term_raw_enter();
}

internal void _term_stop(void) {
    array_free(g_term.event_queue);
    _term_fb_done();
    if (!g_cursor_visible) {
        term_cursor_show();
    }
    _term_alt_leave();
    _term_raw_leave();
    arena_done(&g_term_arena);
}

//------------------------------------------------------------------------------

void term_init(void) {
    if (g_term.initialised) {
        return;
    }

    setlocale(LC_CTYPE, "");

    // Initialise the terminal, ready for a loop
    g_term.size        = (TermSize){0};
    g_term.running     = true;
    g_term.initialised = true;

    arena_init(&g_term_arena, .reserved_size = KORE_MB(128), .grow_rate = 1);

    _term_start();
}

//------------------------------------------------------------------------------

void term_done(void) { g_term.running = false; }

//------------------------------------------------------------------------------

void term_cls(void) { pr("\x1b[2J\x1b[3J\x1b[H"); }

//------------------------------------------------------------------------------

TermEvent term_poll_event(void) {
    TermEvent event;
    if (array_count(g_term.event_queue) > 0) {
        event = g_term.event_queue[0];
        array_delete(g_term.event_queue, 0);
    } else {
        event.kind = TERM_EVENT_NONE;
    }
    return event;
}

//------------------------------------------------------------------------------

void term_cursor_show(void) {
    pr("\x1b[?25h");
    g_cursor_visible = true;
}

void term_cursor_hide(void) {
    pr("\x1b[?25l");
    g_cursor_visible = false;
}

//------------------------------------------------------------------------------

void term_cursor_goto(int x, int y) {
    if (y < 0 || x < 0) {
        TermSize size = term_size_get();
        if (y < 0) {
            y = size.height + y;
        }
        if (x < 0) {
            x = size.width + x;
        }
    }
    ++x;
    ++y;
    pr("\x1b[%d;%dH", y, x);
}

void term_cursor_move(int dx, int dy) {
    if (dy > 0) {
        term_cursor_down(dy);
    } else if (dy < 0) {
        term_cursor_up(-dy);
    }

    if (dx > 0) {
        term_cursor_right(dx);
    } else if (dx < 0) {
        term_cursor_left(-dx);
    }
}

void term_cursor_home(void) { pr("\x1b[H"); }

void term_cursor_up(int delta) {
    if (delta < 0) {
        term_cursor_down(-delta);
    } else {
        pr("\x1b[%dA", delta);
    }
}

void term_cursor_down(int delta) {
    if (delta < 0) {
        term_cursor_up(-delta);
    } else {
        pr("\x1b[%dB", delta);
    }
}

void term_cursor_right(int delta) {
    if (delta < 0) {
        term_cursor_left(-delta);
    } else {
        pr("\x1b[%dC", delta);
    }
}

void term_cursor_left(int delta) {
    if (delta < 0) {
        term_cursor_right(-delta);
    } else {
        pr("\x1b[%dD", delta);
    }
}

//------------------------------------------------------------------------------

internal void _term_fb_resize(u16 width, u16 height) {
    TermSize size              = g_term_fb_size;
    usize current_num_elements = size.width * size.height;
    usize num_elements         = width * height;

    if (num_elements > current_num_elements) {
        // Need to allocate more memory
        array_reserve(g_term_fb_chars, num_elements);
        array_reserve(g_term_fb_ink, num_elements);
        array_reserve(g_term_fb_paper, num_elements);
        array_reserve(g_term_fb_dirty, num_elements);
    }

    // If the width or height as reduced, we need to truncate by repositioning
    // the rows in the array. If the have grown, we need to pad the rows out.
    // Either way, we should reorganise the elements in the array so that the 2D
    // rectangle is either grown or truncated.
    for (u16 y = 0; y < height; y++) {
        for (u16 x = 0; x < width; x++) {
            usize new_index = y * width + x;
            usize old_index = y * size.width + x;
            if (x < size.width && y < size.height) {
                // Copy existing data
                g_term_fb_chars[new_index] = g_term_fb_chars[old_index];
                g_term_fb_ink[new_index]   = g_term_fb_ink[old_index];
                g_term_fb_paper[new_index] = g_term_fb_paper[old_index];
                g_term_fb_dirty[new_index] = g_term_fb_dirty[old_index];
            } else {
                // New area, clear it
                g_term_fb_chars[new_index] = ' ';
                g_term_fb_ink[new_index]   = term_rgba(255, 255, 255, 255);
                g_term_fb_paper[new_index] = term_rgba(0, 0, 0, 255);
                g_term_fb_dirty[new_index] = 1;
            }
        }
    }

    // Update the size
    g_term_fb_size.width  = width;
    g_term_fb_size.height = height;
}

internal void _term_fb_done(void) {
    array_free(g_term_fb_chars);
    array_free(g_term_fb_ink);
    array_free(g_term_fb_paper);
    array_free(g_term_fb_dirty);
}

//------------------------------------------------------------------------------

u32 term_rgb(u8 r, u8 g, u8 b) {
    return (0xFF << 24) | (r << 16) | (g << 8) | (b << 0);
}

u32 term_rgba(u8 r, u8 g, u8 b, u8 a) {
    return (a << 24) | (r << 16) | (g << 8) | (b << 0);
}

u32 term_blend(u32 dest, u32 src, f32 alpha) {
    u8 dest_r = (dest >> 16) & 0xFF;
    u8 dest_g = (dest >> 8) & 0xFF;
    u8 dest_b = (dest >> 0) & 0xFF;

    u8 src_r  = (src >> 16) & 0xFF;
    u8 src_g  = (src >> 8) & 0xFF;
    u8 src_b  = (src >> 0) & 0xFF;

    u8 out_r  = (u8)((src_r * alpha) + (dest_r * (1.0f - alpha)));
    u8 out_g  = (u8)((src_g * alpha) + (dest_g * (1.0f - alpha)));
    u8 out_b  = (u8)((src_b * alpha) + (dest_b * (1.0f - alpha)));

    return term_rgb(out_r, out_g, out_b);
}

//------------------------------------------------------------------------------

void term_fb_cls(u32 ink, u32 paper) {
    TermSize size = g_term_fb_size;
    TermRect rect = {0, 0, size.width, size.height};
    term_fb_rect(rect, ' ', ink, paper);
}

void term_fb_clip_rect(TermRect rect,
                       TermRect* out_clipped_rect,
                       TermRect* out_local_rect) {
    TermSize size = g_term_fb_size;

    // Clip the rectangle to the framebuffer size
    u16 x0        = rect.x;
    u16 y0        = rect.y;
    u16 x1        = rect.x + rect.width;
    u16 y1        = rect.y + rect.height;

    if (x0 < 0) {
        x0 = 0;
    }
    if (y0 < 0) {
        y0 = 0;
    }
    if (x1 > size.width) {
        x1 = size.width;
    }
    if (y1 > size.height) {
        y1 = size.height;
    }

    // Output the clipped rectangle
    out_clipped_rect->x      = x0;
    out_clipped_rect->y      = y0;
    out_clipped_rect->width  = x1 - x0;
    out_clipped_rect->height = y1 - y0;

    // Output the local rectangle
    out_local_rect->x        = x0 - rect.x;
    out_local_rect->y        = y0 - rect.y;
    out_local_rect->width    = out_clipped_rect->width;
    out_local_rect->height   = out_clipped_rect->height;
}

void term_fb_rect_ink(TermRect rect, u32 colour) {
    TermRect clipped_rect, local_rect;
    term_fb_clip_rect(rect, &clipped_rect, &local_rect);

    for (u16 y = 0; y < clipped_rect.height; y++) {
        for (u16 x = 0; x < clipped_rect.width; x++) {
            usize index = (clipped_rect.y + y) * g_term_fb_size.width +
                          (clipped_rect.x + x);
            g_term_fb_ink[index]   = colour;
            g_term_fb_dirty[index] = 1;
        }
    }
}

void term_fb_rect_paper(TermRect rect, u32 colour) {
    TermRect clipped_rect, local_rect;
    term_fb_clip_rect(rect, &clipped_rect, &local_rect);

    for (u16 y = 0; y < clipped_rect.height; y++) {
        for (u16 x = 0; x < clipped_rect.width; x++) {
            usize index = (clipped_rect.y + y) * g_term_fb_size.width +
                          (clipped_rect.x + x);
            g_term_fb_paper[index] = colour;
            g_term_fb_dirty[index] = 1;
        }
    }
}

void term_fb_rect_colour(TermRect rect, u32 ink, u32 paper) {
    TermRect clipped_rect, local_rect;
    term_fb_clip_rect(rect, &clipped_rect, &local_rect);

    for (u16 y = 0; y < clipped_rect.height; y++) {
        for (u16 x = 0; x < clipped_rect.width; x++) {
            usize index = (clipped_rect.y + y) * g_term_fb_size.width +
                          (clipped_rect.x + x);
            g_term_fb_ink[index]   = ink;
            g_term_fb_paper[index] = paper;
            g_term_fb_dirty[index] = 1;
        }
    }
}

void term_fb_rect_char(TermRect rect, u32 ch) {
    TermRect clipped_rect, local_rect;
    term_fb_clip_rect(rect, &clipped_rect, &local_rect);

    for (u16 y = 0; y < clipped_rect.height; y++) {
        for (u16 x = 0; x < clipped_rect.width; x++) {
            usize index = (clipped_rect.y + y) * g_term_fb_size.width +
                          (clipped_rect.x + x);
            g_term_fb_chars[index] = ch;
            g_term_fb_dirty[index] = 1;
        }
    }
}

void term_fb_rect(TermRect rect, u32 ch, u32 ink, u32 paper) {
    TermRect clipped_rect, local_rect;
    term_fb_clip_rect(rect, &clipped_rect, &local_rect);

    for (u16 y = 0; y < clipped_rect.height; y++) {
        for (u16 x = 0; x < clipped_rect.width; x++) {
            usize index = (clipped_rect.y + y) * g_term_fb_size.width +
                          (clipped_rect.x + x);
            g_term_fb_chars[index] = ch;
            g_term_fb_ink[index]   = ink;
            g_term_fb_paper[index] = paper;
            g_term_fb_dirty[index] = 1;
        }
    }
}

void term_utf8_next(cstr* s,
                    u32* out_char,
                    usize* out_bytes,
                    usize* out_width) {
    const u8* ptr = (const u8*)(*s);
    u8 b0         = ptr[0];

    if (b0 == '\0') {
        *out_char  = 0;
        *out_bytes = 0;
        *out_width = 0;
        return;
    }

    u32 ch      = 0;
    usize bytes = 1;

    if (b0 < 0x80) {
        ch = b0;
    } else if ((b0 & 0xE0) == 0xC0) {
        u8 b1 = ptr[1];
        if (b1 == '\0' || (b1 & 0xC0) != 0x80) {
            goto invalid;
        }
        ch    = ((u32)(b0 & 0x1F) << 6) | (u32)(b1 & 0x3F);
        bytes = 2;
        if (ch < 0x80) {
            goto invalid;
        }
    } else if ((b0 & 0xF0) == 0xE0) {
        u8 b1 = ptr[1];
        u8 b2 = ptr[2];
        if (b1 == '\0' || b2 == '\0' || (b1 & 0xC0) != 0x80 ||
            (b2 & 0xC0) != 0x80) {
            goto invalid;
        }
        ch = ((u32)(b0 & 0x0F) << 12) | ((u32)(b1 & 0x3F) << 6) |
             (u32)(b2 & 0x3F);
        bytes = 3;
        if (ch < 0x800 || (ch >= 0xD800 && ch <= 0xDFFF)) {
            goto invalid;
        }
    } else if ((b0 & 0xF8) == 0xF0) {
        u8 b1 = ptr[1];
        u8 b2 = ptr[2];
        u8 b3 = ptr[3];
        if (b1 == '\0' || b2 == '\0' || b3 == '\0' || (b1 & 0xC0) != 0x80 ||
            (b2 & 0xC0) != 0x80 || (b3 & 0xC0) != 0x80) {
            goto invalid;
        }
        ch = ((u32)(b0 & 0x07) << 18) | ((u32)(b1 & 0x3F) << 12) |
             ((u32)(b2 & 0x3F) << 6) | (u32)(b3 & 0x3F);
        bytes = 4;
        if (ch < 0x10000 || ch > 0x10FFFF) {
            goto invalid;
        }
    } else {
        goto invalid;
    }

    *out_char  = ch;
    *out_bytes = bytes;

    int width  = wcwidth((wchar_t)ch);
    if (width <= 0) {
        width = 1;
    }
    *out_width = (usize)width;

    (*s) += bytes;
    return;

invalid:
    *out_char  = (u32)' ';
    *out_bytes = 1;
    *out_width = 1;
    (*s)++;
}

void term_fb_write(u16 x, u16 y, cstr string) {
    TermSize size = g_term_fb_size;
    u16 fb_width  = size.width;
    u16 fb_height = size.height;
    u16 cx        = x;
    u16 cy        = y;

    if (fb_width == 0 || fb_height == 0) {
        return;
    }

    while (*string != '\0' && cy < fb_height) {
        u32 ch;
        usize bytes;
        usize width;
        term_utf8_next(&string, &ch, &bytes, &width);

        if (ch == '\n') {
            cx = x;
            cy += 1;
            continue;
        }

        if (width == 0) {
            width = 1;
        } else if (width > fb_width) {
            width = fb_width;
        }

        if (cx >= fb_width) {
            cx = x;
            cy += 1;
        }

        if (cy >= fb_height) {
            break;
        }

        if (width > (usize)(fb_width - cx)) {
            cx = x;
            cy += 1;
            if (cy >= fb_height) {
                break;
            }
            if (width > (usize)(fb_width - cx)) {
                continue;
            }
        }

        usize row_start        = (usize)cy * fb_width;
        usize index            = row_start + cx;
        g_term_fb_chars[index] = ch;
        g_term_fb_dirty[index] = 1;

        for (usize cell = 1; cell < width; ++cell) {
            u16 tail_x = (u16)(cx + cell);
            if (tail_x >= fb_width) {
                break;
            }
            usize tail_index            = row_start + tail_x;
            g_term_fb_chars[tail_index] = TERM_FB_CHAR_WIDE_TAIL;
            g_term_fb_dirty[tail_index] = 1;
        }

        cx += (u16)width;
        if (cx >= fb_width) {
            cx = x;
            cy += 1;
        }
    }
}

void term_fb_formatv(u16 x, u16 y, cstr fmt, va_list args) {
    arena_reset(&g_term_arena);
    arena_formatv(&g_term_arena, fmt, args);
    cstr output = (cstr)g_term_arena.memory;
    term_fb_write(x, y, output);
}

void term_fb_format(u16 x, u16 y, cstr fmt, ...) {
    va_list args;
    va_start(args, fmt);
    term_fb_formatv(x, y, fmt, args);
    va_end(args);
}

//------------------------------------------------------------------------------
// Presentation

void term_fb_present(void) {
    TermSize size = g_term_fb_size;
    arena_reset(&g_term_arena);

    // We go through each row to detect sequence of dirty characters and
    // consecutive ink and paper colours.
    //
    // Each sequence starts with a CURSOR_GOTO or CURSOR_RIGHT code so that
    // the cursor is at the beginning of the changed area.  Then for a
    // single sequence of same ink and paper, the ANSI attributes are
    // written to set the colours followed by the characters.
    //
    // The u32 characters are encoded into a UTF-8 sequence.
    //
    // When a row is complete, the next row is done.

    u16 last_x = 0;
    u16 last_y = 0;

    if (g_cursor_visible) {
        term_cursor_hide();
    }

    // Write home code
    arena_format(&g_term_arena, "\x1b[H");

    for (u16 y = 0; y < size.height; ++y) {
        u16 x          = 0;
        u16 base_index = y * size.width;
        while (x < size.width) {
            // Find the start of the next dirty section
            if (g_term_fb_dirty[base_index + x] == 0) {
                x++;
                continue;
            }

            // Found the start of a dirty section
            if (x != last_x || y != last_y) {
                // Move the cursor to this position
                arena_format(&g_term_arena, "\x1b[%d;%dH", y + 1, x + 1);
            }
            last_x    = x;
            last_y    = y;

            // Get the ink and paper for this section
            u32 ink   = g_term_fb_ink[base_index + x];
            u32 paper = g_term_fb_paper[base_index + x];
            arena_format(&g_term_arena,
                         "\x1b[38;2;%u;%u;%um",
                         (ink >> 16) & 0xFF,
                         (ink >> 8) & 0xFF,
                         (ink >> 0) & 0xFF);
            arena_format(&g_term_arena,
                         "\x1b[48;2;%u;%u;%um",
                         (paper >> 16) & 0xFF,
                         (paper >> 8) & 0xFF,
                         (paper >> 0) & 0xFF);

            // Output characters until the ink/paper changes or we hit a
            // clean section
            while (x < size.width && g_term_fb_dirty[base_index + x] != 0 &&
                   g_term_fb_ink[base_index + x] == ink &&
                   g_term_fb_paper[base_index + x] == paper) {
                u32 ch = g_term_fb_chars[base_index + x];

                if (ch != (u32)TERM_FB_CHAR_WIDE_TAIL) {
                    if (ch <= 0x7F) {
                        arena_format(&g_term_arena, "%c", (char)(ch & 0x7F));
                    } else if (ch <= 0x7FF) {
                        arena_format(&g_term_arena,
                                     "%c%c",
                                     (char)(0xC0 | ((ch >> 6) & 0x1F)),
                                     (char)(0x80 | (ch & 0x3F)));
                    } else if (ch <= 0xFFFF) {
                        arena_format(&g_term_arena,
                                     "%c%c%c",
                                     (char)(0xE0 | ((ch >> 12) & 0x0F)),
                                     (char)(0x80 | ((ch >> 6) & 0x3F)),
                                     (char)(0x80 | (ch & 0x3F)));
                    } else {
                        arena_format(&g_term_arena,
                                     "%c%c%c%c",
                                     (char)(0xF0 | ((ch >> 18) & 0x07)),
                                     (char)(0x80 | ((ch >> 12) & 0x3F)),
                                     (char)(0x80 | ((ch >> 6) & 0x3F)),
                                     (char)(0x80 | (ch & 0x3F)));
                    }
                }

                g_term_fb_dirty[base_index + x] = 0;
                x++;
                last_x++;
            }
        }
    }

    arena_null_terminate(&g_term_arena);
    cstr output = (cstr)g_term_arena.memory;
    if (g_cursor_visible) {
        term_cursor_show();
    }

    pr("%s", output);
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

#endif // KORE_IMPLEMENTATION
