#ifndef _GL_UTILS_H_
#define _GL_UTILS_H_

#include <GL/glew.h> // include GLEW and new version of GL on Windows
#include <GLFW/glfw3.h> // GLFW helper library
#include <stdio.h>
#include <stdlib.h>

// keep track of window size for things like the viewport and the mouse cursor
extern int g_gl_width;
extern int g_gl_height;
extern GLFWwindow* g_window;

bool start_gl();

void glfw_error_callback(int error, const char* description);

// a call-back function
void glfw_window_size_callback(GLFWwindow* window, int width, int height);

void _update_fps_counter(GLFWwindow* window);

bool parse_file_into_str(const char* file_name, char* shader_str, int max_len);

#endif
