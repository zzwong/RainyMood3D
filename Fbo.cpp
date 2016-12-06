//
//  Fbo.cpp
//  CSE167-Final
//
//  Created by Freddy Sauceda on 12/5/16.
//  Copyright © 2016 Alfredo Sauceda. All rights reserved.
//

#include "Fbo.h"

FBO::FBO(){
    fboRefl = initRefl();
    checkErrors(fboRefl);
    
    fboRefr = initRefr();
    checkErrors(fboRefr);
}

void FBO::checkErrors(GLuint fbo){
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    //Check for erros
    GLenum e = glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER);
    if (e != GL_FRAMEBUFFER_COMPLETE)
        printf("%x\n",e);
    else
        printf("Gucci\n");
    glBindFramebuffer(GL_FRAMEBUFFER, 0); //Return to default fbo
}

GLuint FBO::initRefl(){
    GLuint fbo;
    //Create a fbo
    glGenFramebuffers(1, &fbo);
    glDrawBuffer(GL_COLOR_ATTACHMENT0);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    
    //Create attachments
    reflTex = createTexture();
    reflDepthBuffer = createDepthBuff();
    
    return fbo;
}

GLuint FBO::initRefr(){
    GLuint fbo;
    //Create a fbo
    glGenFramebuffers(1, &fbo);
    glDrawBuffer(GL_COLOR_ATTACHMENT0);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    
    //Create attachments
    refrTex = createTexture();
    refrDepthTexture = createDepth();
    
    return fbo;
}

GLuint FBO::createTexture(){
    GLuint tex;
    //Create texture
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, Window::width, Window::height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
    //Filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    //Attach
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, tex, 0);
    
    return tex;
}

GLuint FBO::createDepth(){
    GLuint depth;
    //Create texture
    glGenTextures(1, &depth);
    glBindTexture(GL_TEXTURE_2D, depth);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, Window::width, Window::height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
    //Filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    //Attach
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depth, 0);
    
    return depth;
}

GLuint FBO::createDepthBuff(){
    GLuint depthBuff;
    //Create buffer
    glGenRenderbuffers(1, &depthBuff);
    glBindRenderbuffer(GL_RENDERBUFFER, depthBuff);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, Window::width, Window::height);
    
    //Attach with
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuff);
    
    return depthBuff;
}

void FBO::bind(GLuint framebuffer){
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    glViewport(0, 0, Window::width, Window::height);
    
    GLenum drawBuffs = GL_COLOR_ATTACHMENT0;
    glDrawBuffers(1, &drawBuffs);
    
    //Enable alpha blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void FBO::unbind(){
    glDisable(GL_BLEND);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, Window::width, Window::height);
}
GLuint FBO::getReflectionFBO(){
    return fboRefl;
}
GLuint FBO::getRefractionFBO(){
    return fboRefr;
}

GLuint FBO::getReflTex(){
    return reflTex;
}

GLuint FBO::getRefrTex(){
    return refrTex;
}

GLuint FBO::getRefrDepth(){
    return refrDepthTexture;
}

GLuint FBO::getReflDepth(){
    return reflDepthBuffer;
}
