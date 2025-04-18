#pragma once

void setup_glrender();
unsigned int load_gl_texture(char* path, int* width, int* height);
void unload_gl_texture(unsigned int tex);
void glrender_prepare(float aspect, int desired_height_px);
void glrender_draw_sprite(unsigned int texture, int atlas_position, int atlas_size, float x, float y, float sx, float sy, float r);
void teardown_glrender();