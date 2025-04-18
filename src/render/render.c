#include "render.h"

#include <stdlib.h>

#include "io/window.h"
#include "opengl/glrender.h"

void setup_renderer() {
    setup_glrender();
}

Texture* load_texture(char* path) {
    Texture* tex = malloc(sizeof(Texture));
    tex->gl_texture = load_gl_texture(path, &tex->width, &tex->height);
    tex->atlas_size = 1;
    return tex;
}

void unload_texture(Texture* tex) {
    unload_gl_texture(tex->gl_texture);
    free(tex);
}

void prepare_render() {
    int screen_width;
    int screen_height;
    get_window_size(&screen_width, &screen_height);
    float aspect_ratio = (float)screen_height / (float)screen_width;
    glrender_prepare(aspect_ratio, 480);
}

void draw_sprite(Texture* texture, float x, float y, float r) {
    glrender_draw_sprite(texture->gl_texture, 0, 1, x, y, texture->width, texture->height, r);
}

void teardown_renderer() {
    teardown_glrender();
}