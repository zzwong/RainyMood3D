//
//  FullscreenQuad.hpp
//  CSE167-Final
//
//  Created by Freddy Sauceda on 12/7/16.
//  Copyright © 2016 Alfredo Sauceda. All rights reserved.
//

#ifndef FullscreenQuad_hpp
#define FullscreenQuad_hpp

#include <stdio.h>
#include "Geode.h"

class FullscreenQuad : public Geode {
    
public:
    FullscreenQuad(GLuint shader);
    void draw();
    void update();
    GLuint getVAO();
    
    void draw(glm::mat4);
    
private:
    
    
};

#endif /* FullscreenQuad_hpp */
