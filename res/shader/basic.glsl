--- vertex shader ---
#version 330 core

layout (location = 0) in vec2 a_position;

out vec2 v_texcoords;

uniform mat4 u_transform;

void main() {
    v_texcoords = a_position + 0.5;
    gl_Position = u_transform * vec4(a_position.x, a_position.y, 0.0, 1.0);
}

--- fragment shader ---
#version 330 core

in vec2 v_texcoords;

out vec4 out_color;

uniform sampler2D u_texture;

void main() {
    out_color = texture(u_texture, v_texcoords);
}