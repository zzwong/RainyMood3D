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
    
    void createFrameBuffer();
    GLuint getReflectionFBO();
    GLuint getRefractionFBO();
    GLuint getReflectionTex();
    GLuint getRefractionTex();
    void bindFrameBuffer(GLuint fbo, int width, int height);
    void unbindFrameBuffer();
    void getLocations();
    void connectTex();
    GLuint getVAO();
    
protected:
    GLfloat vertices[6][3] = {
        {-150.0f,-2.0f,150.0f},
        {150.0f,-2.0f,150.0f},
        {150.0f,-2.0f,-150.0f},
        {-150.0f,-2.0f,150.0f},
        {-150.0f,-2.0f,-150.0f},
        {150.0f,-2.0f,-150.0f}
    };
    
    glm::mat4 toWorld;
    
    GLuint refractionBuffer, reflectionBuffer;
    GLuint reflectionTexture, reflectionDepth;
    GLuint refractionTexture, refractionDepth;
    
    GLuint reflectionTextLoc, refractionTexLoc;
};


#endif /* Water_hpp */
