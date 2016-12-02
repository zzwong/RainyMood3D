#ifndef NODE
#define NODE
#include "glm/glm.hpp"
#define GLFW_INCLUDE_GLEXT
#ifdef __APPLE__
#define GLFW_INCLUDE_GLCOREARB
#else
#include <GL/glew.h>
#endif
#include <GLFW/glfw3.h>


class Node {
public:
    virtual void draw(glm::mat4 C) = 0; //Used to draw the geodes
    virtual void update() = 0; //Used to update the bounding sphere
   
};

#endif