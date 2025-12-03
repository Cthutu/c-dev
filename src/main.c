#define KORE_IMPLEMENTATION
#include "kore.h"
#include "term.h"
#include <stdio.h>

int main() {
    kore_init();
    term_init();

    while (term_loop()) {
        TermEvent event = term_poll_event();

        switch (event.kind) {
        case TERM_EVENT_NONE:
            // No event to process
            break;
        case TERM_EVENT_KEY:
            prn("Key pressed: %c", event.key);
            if (event.key == 'q') {
                term_done();
            }
            break;
        case TERM_EVENT_RESIZE:
            prn("Terminal resized: %dx%d", event.size.width, event.size.height);
            break;
        }
    }

    kore_done();
}
