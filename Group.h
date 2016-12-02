#ifndef GROUP
#define GROUP
#include "Node.h"
#include <glm/gtc/matrix_transform.hpp>
#include <list>
#include <vector>
//#include "Sphere.h"

class Group : public Node{
public:
    Group();
    ~Group();
    //Add a child
    void addChild(Node *n);
    //Remove a child
    void removeChild(Node *n);
    //Draw all the children
    virtual void draw(glm::mat4 C);
    //Update because piazza said so
    void update();
    
    void setBoundsForRobot();
    void drawBoundingSphere(GLuint);
    glm::vec3 getPoint();
    
    std::vector<Node*> getChildrenVector();
    
protected:
    std::vector<Node*> children;
    
//    Sphere* boundingSphere;
    glm::vec4 center;
    float radius = 1.75f;
};

#endif