#include "Group.h"
#include "MatrixTransform.h"
#include "Node.h"
#include "Geode.h"
#include <iostream>

Group::Group(){
    children = std::vector<Node*>();
}

Group::~Group(){
    //Dlete all it's children
    for(auto child : children){
        delete child;
    }
}

//Add a child
void Group::addChild(Node *n){
    children.push_back(n);
}
//Remove a child
void Group::removeChild(Node *n){
    //children.remove(n);
}
//Draw all the children
void Group::draw(glm::mat4 C){
    glm::vec4 translation = C * glm::vec4(0,0,0,1);
    center = translation;
    for(auto child : children){
        child->draw(C);
    }
}
//Update because piazza said so
void Group::update() {

}

void Group::setBoundsForRobot(){
//    boundingSphere = new Sphere();
}

void Group::drawBoundingSphere(GLuint shader){
    
}

std::vector<Node*> Group::getChildrenVector(){
    return children;
}

glm::vec3 Group::getPoint(){
    return glm::vec3(center.x, center.y, center.z);
}








