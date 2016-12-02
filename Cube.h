
#ifndef CUBE
#define CUBE
#include "Geode.h"

class Cube : public Geode{
public:
    Cube(GLuint shader);
    ~Cube();
    
    float angle;
    
    void draw(glm::mat4 C);
    void update();
    
protected:
    GLfloat vertices_[8][3] = {
        // "Front" vertices
        {-1.0, -1.0,  1.0},
        {1.0, -1.0,  1.0},
        {1.0,  1.0,  1.0},
        {-1.0,  1.0,  1.0},
        // "Back" vertices
        {-1.0, -1.0, -1.0},
        {1.0, -1.0, -1.0},
        {1.0,  1.0, -1.0},
        {-1.0,  1.0, -1.0}
    };
    
    GLfloat normals_[8][3] = {
        // "Front"
        {-1.0f/3, -1.0f/3, 1.0f/3},
        {1.0f/3, -1.0f/3, 1.0f/3},
        {1.0f/3, 1.0f/3, 1.0f/3},
        {0.0, 1.0, 0.0},
        // "Back"
        {-1.0f/3, -1.0f/3, -1.0f/3},
        {1.0f/3, -1.0f/3, -1.0f/3},
        {1.0f/3, 1.0f/3, -1.0f/3},
        {-1.0f/3,  1.0f/3, -1.0f/3}
    };
    
    GLuint indices_[6][6] = {
        // Front face
        {0, 1, 2, 2, 3, 0},
        // Top face
        {1, 5, 6, 6, 2, 1},
        // Back face
        {7, 6, 5, 5, 4, 7},
        // Bottom face
        {4, 0, 3, 3, 7, 4},
        // Left face
        {4, 5, 1, 1, 0, 4},
        // Right face
        {3, 2, 6, 6, 7, 3}
    };
    
    glm::mat4 toWorld;
};

#endif