#include "window.h"

#include <stdio.h>
#include <stdlib.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "event/event.h"

GLFWwindow* window;

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
    Event resize_event;
    resize_event.type = EVENT_TYPE_WINDOW_RESIZE;
    resize_event.field1 = width;
    resize_event.field2 = height;
    raise_event(&resize_event);
}

void close_callback(GLFWwindow* window) {
    Event close_event;
    close_event.type = EVENT_TYPE_WINDOW_CLOSE;
    raise_event(&close_event);
}

void setup_window_and_gl_ctx(int width, int height, char* name) {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    window = glfwCreateWindow(width, height, name, NULL, NULL);
    if (window == NULL) {
        printf("GLFW window creation failed\n");
        glfwTerminate();
        exit(1);
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        printf("Failed to initialize GLAD\n");
        glfwTerminate();
        exit(1);
    }

    glViewport(0, 0, width, height);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetWindowCloseCallback(window, close_callback);
}

void get_window_size(int* width, int* height) {
    glfwGetWindowSize(window, width, height);
}

void window_update() {
    glfwSwapBuffers(window);
    glfwPollEvents();
}

void teardown_window() {
    glfwDestroyWindow(window);
    glfwTerminate();
}