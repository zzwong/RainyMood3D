#include "MatrixTransform.h"

//Constructor
MatrixTransform::MatrixTransform(glm::mat4 C){
    this->M = C * M;
}
//Destructor
MatrixTransform::~MatrixTransform(){
    for(auto child : children){
        delete child;
    }
    Group::~Group();
}

//Update the MT
void MatrixTransform::update(glm::mat4 update){
    this->M = update * M;
}

//Pass down this transformation to all children
void MatrixTransform::draw(glm::mat4 C){
    glm::mat4 M_new = C * M;
        
    for(auto child : children){
        child->draw(M_new);
    }
}

//Get the child beneath this MT
Node* MatrixTransform::getChild(){
    return this->children[0];
}