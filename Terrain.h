#ifndef Terrain_h
#define Terrain_h

#include "Geode.h"
#include "FastNoise.h"
#include <vector>


#define MAX_COL 1920
#define MAX_ROW 1080

class Terrain : public Geode {
    
public:
    Terrain(GLuint shader, int c, int r, int scl);
    ~Terrain();
    
    void draw(glm::mat4 C);
    void update();
    
protected:
    FastNoise * noise_gen;
    int cols, rows;
    int scale;
    int width, height;
    float flying = 0;
    float yTerrain = 0;
    
    glm::mat4 toWorld;
    std::vector<glm::vec3> vertices;
    float terrain[MAX_COL][MAX_ROW]; // height map
    
    
};


#endif /* Terrain_h */
