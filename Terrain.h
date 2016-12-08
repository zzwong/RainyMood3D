#ifndef Terrain_h
#define Terrain_h

#include "Geode.h"
#include "FastNoise.h"
#include <vector>


#define MAX_COL 1920
#define MAX_ROW 1080

#define FIELDS "fields.ppm"
#define ROCKS  "rocks.ppm"
#define SNOW   "snow.ppm"


class Terrain : public Geode {
    
public:
    Terrain(GLuint shader, int c, int r, int scl);
    Terrain(GLuint shader, const char* heightmap, int scl);
    ~Terrain();
    
    void draw(glm::mat4 C);
    void update();
    GLuint getVAO();
    void updateOctaves(float amt);
    void updateNormals();
    void setScale(int scl);
protected:
    FastNoise * noise_gen;
    float octaves;
    int cols, rows;
    int scale;
    int width, height;
    float flying = 0;
    float yTerrain = 0;
    unsigned char* hmap;
    GLuint snowTex, rockTex, fieldsTex;
    GLuint loc_snow, loc_rock, loc_fields;
    
    glm::mat4 toWorld;
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec3> normals;
    std::vector<GLuint> indices;
    float terrain[MAX_COL][MAX_ROW]; // height map
    
    void genTextures();
    
};


#endif /* Terrain_h */
