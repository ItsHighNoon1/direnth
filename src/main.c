#include "event/event.h"
#include "io/window.h"
#include "render/render.h"

int should_run;

int close_handler(Event* e) {
    should_run = 0;
    return 0;
}

int main() {
    should_run = 1;

    setup_event_manager();
    register_event_handler(EVENT_TYPE_WINDOW_CLOSE, close_handler);
    setup_window_and_gl_ctx(853, 480, "direnth");
    setup_renderer();
    Texture* texture = load_texture("res/test.png");

    while (should_run) {
        prepare_render();
        draw_sprite(texture, 0.0f, 0.0f, 0.0f);
        window_update();
    }

    unload_texture(texture);
    teardown_renderer();
    teardown_window();
    teardown_event_manager();
    return 0;
}