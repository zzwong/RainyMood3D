#ifndef MTFORM
#define MTFORM
#include <stdio.h>
#include "Group.h"

class MatrixTransform : public Group{
public:
    MatrixTransform(glm::mat4);
    ~MatrixTransform();
    //Pass down this transformation to all children
    void draw(glm::mat4 C);
    void update(glm::mat4);
    Node* getChild();
protected:
    //Model matrix
    glm::mat4 M = glm::mat4(1.0f);
};

#endif