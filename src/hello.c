//> use: core term

#include <term/term.h>

int run(int argc, char** argv)
{
    UNUSED(argc);
    UNUSED(argv);

    term_init();

    while (term_loop()) {
        TermEvent event = term_poll_event();
        switch (event.kind) {
        case TERM_EVENT_KEY:
            if (event.key == 'q') {
                term_done();
            }
            break;
        case TERM_EVENT_RESIZE:
            term_fb_cls(term_rgb(255, 255, 255), term_rgb(0, 0, 0));
            term_fb_format(1,
                           1,
                           "Terminal resized to %ux%u",
                           event.size.width,
                           event.size.height);
            break;
        default:
            break;
        }

        term_fb_present();
    }

    return 0;
}
