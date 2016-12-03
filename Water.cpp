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
    glDeleteFramebuffers(1, &reflectionBuffer);
    glDeleteFramebuffers(1, &refractionBuffer);
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
    
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glUniform1f(glGetUniformLocation(shaderProgram, "water"), false);
    
    //glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    
    glBindVertexArray(0);
    
    
}

void Water::createFrameBuffer(){
    
    //Create the two FBOS
    glGenBuffers(1, &reflectionBuffer);
    glGenBuffers(1, &refractionBuffer);
    
    //Start with reflection
    glBindFramebuffer(GL_FRAMEBUFFER, reflectionBuffer);
    //Create texture
    glGenTextures(1, &reflectionTexture);
    //Bind texture
    glBindTexture(GL_TEXTURE_2D, reflectionTexture);
    //Give empty image(for now)
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 320, 180, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
    //Filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    //Attach image to framebuffer
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, reflectionTexture, 0);
    //Create the reflection depth
    glGenRenderbuffers(1, &reflectionDepth);
    glBindRenderbuffer(GL_RENDERBUFFER, reflectionDepth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, 320, 180);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, reflectionDepth);
    
    //Now on to refraction
    glBindFramebuffer(GL_FRAMEBUFFER, refractionBuffer);
    glGenTextures(1, &refractionTexture);
    glBindTexture(GL_TEXTURE_2D, refractionTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1280, 720, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, refractionTexture, 0);
    glGenRenderbuffers(1, &refractionDepth);
    glBindRenderbuffer(GL_RENDERBUFFER, refractionDepth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, 1280, 720);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, refractionDepth);
}

GLuint Water::getReflectionFBO(){
    return reflectionBuffer;
}

GLuint Water::getRefractionFBO(){
    return refractionBuffer;
}

GLuint Water::getReflectionTex(){
    return reflectionTexture;
}

GLuint Water::getRefractionTex(){
    return refractionTexture;
}

void Water::bindFrameBuffer(GLuint fbo, int width, int height){
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glViewport(0, 0, width, height);
}

void Water::unbindFrameBuffer(){
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, Window::width, Window::height);
}

void Water::update(){
    
}
