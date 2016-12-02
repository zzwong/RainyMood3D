#ifndef GEODE
#define GEODE
#include "Node.h"

class Geode : public Node{
public:
    virtual void draw(glm::mat4 C) = 0;
    virtual void update() = 0;
    
protected:
    GLuint VBO, VAO, EBO, NBO;
    GLuint uProjection, uModelview;
    GLuint shaderProgram;
    
    glm::mat4 toWorld;
};

#endif