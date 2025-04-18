#pragma once

typedef struct {
    union {
        unsigned int gl_texture;
    };
    int atlas_size;
    int width;
    int height;
} Texture;

void setup_renderer();
Texture* load_texture(char* path);
void unload_texture(Texture* tex);
void prepare_render();
void draw_sprite(Texture* texture, float x, float y, float r);
void teardown_renderer();