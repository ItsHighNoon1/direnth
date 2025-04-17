#include "event/event.h"
#include "io/window.h"

int should_run;

int close_handler(Event* e) {
    should_run = 0;
    return 0;
}

int main() {
    should_run = 1;

    setup_event_manager();
    register_event_handler(EVENT_TYPE_WINDOW_CLOSE, close_handler);
    setup_window_and_gl_ctx(800, 500, "direnth");

    while (should_run) {
        window_update();
    }

    teardown_window();
    teardown_event_manager();
    return 0;
}