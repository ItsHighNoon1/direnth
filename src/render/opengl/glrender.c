#include "glrender.h"

#include <cglm/affine.h>
#include <cglm/mat4.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <cglm/cglm.h>
#include <glad/glad.h>
#include <stb/stb_image.h>

const float QUAD_VERTS[] = {
    -0.5f, -0.5f,
     0.5f, -0.5f,
    -0.5f,  0.5f,
     0.5f,  0.5f
};

GLuint vao;
GLuint vbo;
float aspect_ratio;
int screen_height;
mat4 proj_matrix;

GLuint basic_program;
GLint uniform_transform;

static GLuint compile_shader(const char* path) {
    FILE* fp = fopen(path, "r");
    if (fp == NULL) {
        printf("Failed to open shader file %.64s\n", path);
        return -1;
    }

    fseek(fp, 0l, SEEK_END);
    long file_len = ftell(fp);
    fseek(fp, 0l, SEEK_SET);
    char* file_buf = malloc(file_len + 1);
    file_buf[file_len] = '\0';
    long bytes_read = fread(file_buf, 1, file_len, fp);
    if (bytes_read != file_len) {
        printf("Warning: read %ld bytes but ftell reported %ld bytes for file %.64s\n", bytes_read, file_len, path);
    }

    GLuint program = glCreateProgram();
    GLuint shaders[10];
    int shader_count = 0;
    
    char* file_end = file_buf + file_len;
    char* shader_start = NULL;
    char* scan_ptr = file_buf;
    GLenum next_shader;
    while (scan_ptr < file_end) {
        if (strncmp("--- ", scan_ptr, 4) == 0) {
            if (shader_start != NULL) {
                GLuint shader = glCreateShader(next_shader);
                int shader_len = scan_ptr - shader_start;
                glShaderSource(shader, 1, (const char**)&shader_start, &shader_len);
                glCompileShader(shader);
                glAttachShader(program, shader);
            
                GLint rc;
                glGetShaderiv(shader, GL_COMPILE_STATUS, &rc);
                if (!rc) {
                    char log_buf[512];
                    glGetShaderInfoLog(shader, 512, NULL, log_buf);
                    printf("Shader compile error in %.64s:\n%.512s\n", path, log_buf);
                }

                shaders[shader_count++] = shader;
            }

            char* line_start = scan_ptr;
            for (; scan_ptr < file_end && *scan_ptr != '\n'; scan_ptr++) {}
            scan_ptr++;
            shader_start = scan_ptr;

            if (strncmp("vertex", line_start + 4, 4) == 0) {
                next_shader = GL_VERTEX_SHADER;
            } else if (strncmp("fragment", line_start + 4, 4) == 0) {
                next_shader = GL_FRAGMENT_SHADER;
            } else {
                printf("Invalid shader split line %.*s in file %.64s\n", (int)(scan_ptr - line_start), line_start, path);
            }
        }
        for (; scan_ptr < file_end && *scan_ptr != '\n'; scan_ptr++) {}
        scan_ptr++;
    }
    if (shader_start != NULL) {
        GLuint shader = glCreateShader(next_shader);
        int shader_len = scan_ptr - shader_start;
        glShaderSource(shader, 1, (const char**)&shader_start, &shader_len);
        glCompileShader(shader);
        glAttachShader(program, shader);
    
        GLint rc;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &rc);
        if (!rc) {
            char log_buf[512];
            glGetShaderInfoLog(shader, 512, NULL, log_buf);
            printf("Shader compile error in %.64s:\n%.512s\n", path, log_buf);
        }

        shaders[shader_count++] = shader;
    }

    if (shader_count > (sizeof(shaders) / sizeof(GLint))) {
        printf("Exceeded maximum number of shaders in file %.64s\n", path);
    }
    for (int shader_idx = 0; shader_idx < shader_count; shader_idx++) {
        glDeleteShader(shaders[shader_idx]);
    }

    free(file_buf);

    GLint rc;
    glLinkProgram(program);
    glGetProgramiv(program, GL_LINK_STATUS, &rc);
    if (!rc) {
        char log_buf[512];
        glGetProgramInfoLog(program, 512, NULL, log_buf);
        printf("Shader link error in %.64s:\n%.512s\n", path, log_buf);
    }
    return program;
}

void setup_glrender() {
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(QUAD_VERTS), QUAD_VERTS, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), NULL);
    glEnableVertexAttribArray(0);

    basic_program = compile_shader("res/shader/basic.glsl");
    uniform_transform = glGetUniformLocation(basic_program, "u_transform");
}

GLuint load_gl_texture(char* path, int* width, int* height) {
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float border_color[] = { 0.0f, 0.0f, 0.0f, 0.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border_color);
    
    stbi_set_flip_vertically_on_load(1);
    int width2;
    int height2;
    int n_channels;
    unsigned char* data = stbi_load(path, &width2, &height2, &n_channels, 0);
    if (data != NULL) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width2, height2, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        stbi_image_free(data);
    } else {
        printf("Failed to load texture %.64s\n", path);
        return -1;
    }

    if (width != NULL) {
        *width = width2;
    }
    if (height != NULL) {
        *height = height2;
    }
    
    return texture;
}

void unload_gl_texture(GLuint tex) {
    glDeleteTextures(1, &tex);
}

void glrender_prepare(float aspect, int desired_height_px) {
    aspect_ratio = aspect;
    screen_height = desired_height_px;
    float proj_scale[] = { aspect / desired_height_px, 1.0f / desired_height_px, 1.0f };
    glm_scale_make(proj_matrix, proj_scale);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  
    glDisable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT);
    glUseProgram(basic_program);
    glBindVertexArray(vao);
}

void glrender_draw_sprite(GLuint texture, int atlas_position, int atlas_size, float x, float y, float sx, float sy, float r) {
    glBindTexture(GL_TEXTURE_2D, texture);
    if (atlas_size <= 1) {
        
    } else {
        
    }

    float translation[] = { x, y, 0.0f };
    float scale[] = { sx, sy, 0.0f };
    mat4 transform;
    glm_translate_make(transform, translation);
    glm_scale(transform, scale);
    glm_rotate_z(transform, r, transform);

    glm_mat4_mul(proj_matrix, transform, transform);
    
    glUniformMatrix4fv(uniform_transform, 1, GL_FALSE, transform[0]);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void teardown_glrender() {
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteProgram(basic_program);
}