//
//  Fbo.hpp
//  CSE167-Final
//
//  Created by Freddy Sauceda on 12/5/16.
//  Copyright © 2016 Alfredo Sauceda. All rights reserved.
//

#ifndef Fbo_hpp
#define Fbo_hpp

#include "Window.h"
#include <stdio.h>

class FBO{

public:
    FBO(); //Construct the fbo
    GLuint initRefl();  //Returns the fbo if depth == true then depth is tex, else its buff
    GLuint initRefr();
    void bind(GLuint);
    void unbind();
    GLuint createTexture();
    GLuint createDepth();
    GLuint createDepthBuff();
    void checkErrors(GLuint fbo);
    
    
private:
    GLuint fboRefl;
    GLuint reflTex, reflDepthBuffer;
    
    GLuint fboRefr;
    GLuint refrTex, refrDepthTexture;
};

#endif /* Fbo_hpp */
