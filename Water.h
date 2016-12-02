//
//  Water.hpp
//  CSE167-Final
//
//  Created by Freddy Sauceda on 12/1/16.
//  Copyright © 2016 Alfredo Sauceda. All rights reserved.
//

#ifndef Water_hpp
#define Water_hpp

#include <stdio.h>
#include "Geode.h"

class Water : public Geode{
public:
    Water(GLuint shader);
    ~Water();
    
    void draw(glm::mat4 C);
    void update();
protected:
    GLfloat vertices[6][3] = {
        {-30.0f,-2.0f,30.0f},
        {30.0f,-2.0f,30.0f},
        {30.0f,-2.0f,-30.0f},
        {-30.0f,-2.0f,30.0f},
        {-30.0f,-2.0f,-30.0f},
        {30.0f,-2.0f,-30.0f}
    };
    
    glm::mat4 toWorld;
};


#endif /* Water_hpp */
