//
//  FullscreenQuad.cpp
//  CSE167-Final
//
//  Created by Freddy Sauceda on 12/7/16.
//  Copyright © 2016 Alfredo Sauceda. All rights reserved.
//

#include "FullscreenQuad.h"

FullscreenQuad::FullscreenQuad(GLuint shader){
    this->shaderProgram = shader;
    
    static const GLfloat quad_vert[] = {
        -1.0f, -1.0f, 0.0f,
        1.0f, -1.0f, 0.0f,
        -1.0f,  1.0f, 0.0f,
        -1.0f,  1.0f, 0.0f,
        1.0f, -1.0f, 0.0f,
        1.0f,  1.0f, 0.0f,
    };
    
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    
    glBindVertexArray(VAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quad_vert), quad_vert, GL_STATIC_DRAW);
    
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    
    
}
void FullscreenQuad::draw(){
    glUseProgram(shaderProgram);
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

GLuint FullscreenQuad::getVAO(){
    return VAO;
}

void FullscreenQuad::draw(glm::mat4 C){} //DUMMY

void FullscreenQuad::update(){} //DUMMY
