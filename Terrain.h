#ifndef Terrain_h
#define Terrain_h

#include "Geode.h"


class Terrain : public Geode {
    
public:
    Terrain(GLuint shader, int c, int r, int scl);
    ~Terrain();
    
    void draw(glm::mat4 C);
    void update();
    
protected:
    int cols, rows;
    int scale;
    int width, height;
    float flying = 0;
    
    glm::mat4 toWorld;
    
};


#endif /* Terrain_h */
