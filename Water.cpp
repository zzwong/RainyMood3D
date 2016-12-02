//
//  Water.cpp
//  CSE167-Final
//
//  Created by Freddy Sauceda on 12/1/16.
//  Copyright © 2016 Alfredo Sauceda. All rights reserved.
//

#include "Water.h"
#include "Window.h"

Water::Water(GLuint shader){
    this->shaderProgram = shader;
    //Create VAO objects
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    
    glBindVertexArray(VAO);
    
    //Bind the VBO
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}
Water::~Water(){
    glDeleteVertexArrays(1, &VAO);
    glDeleteVertexArrays(1, &VBO);
    glDeleteShader(shaderProgram);
}

void Water::draw(glm::mat4 C){
    this->toWorld = C;
    
    //Draw
    glm::mat4 modelview = Window::V * toWorld;
    
    uProjection = glGetUniformLocation(shaderProgram, "projection");
    uModelview = glGetUniformLocation(shaderProgram, "modelview");
    // Now send these values to the shader program
    glUniformMatrix4fv(uProjection, 1, GL_FALSE, &Window::P[0][0]);
    glUniformMatrix4fv(uModelview, 1, GL_FALSE, &modelview[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, &this->toWorld[0][0]);
    glUniform1f(glGetUniformLocation(shaderProgram, "water"), true);
    
    glBindVertexArray(VAO);
    //glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glUniform1f(glGetUniformLocation(shaderProgram, "water"), false);
    glBindVertexArray(0);
    
    
}

void Water::update(){
    
}
