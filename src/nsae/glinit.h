#ifndef GLINIT_H
#define GLINIT_H
#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#define GLFW_INCLUDE_NONE
#else
#include <GL/glew.h>
#include <GL/glu.h>
#endif
#include <GLFW/glfw3.h>
#endif
