#ifndef TextureHandler_h
#define TextureHandler_h

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <iostream>
#include <vector>

#include <GLFW/glfw3.h>


class TextureHandler {
public:
    static unsigned char* loadPPM(const char* filename, int& height, int& width);
    static GLuint loadCubemap(std::vector<const char*> faces);
};

#endif /* TextureHandler_h */
