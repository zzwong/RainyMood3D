


#include <stdio.h>
#include "Terrain.h"
#include "Window.h"
#include <random>


float map(float val, float istart, float istop, float ostart, float ostop);

void Terrain::updateOctaves(float amt){
    octaves += amt;
    noise_gen->SetFractalOctaves(octaves);
}

Terrain::Terrain(GLuint shader, int w, int h, int scl){
    this->shaderProgram = shader;
    width = w;
    height = h;
    cols = w / scl;
    rows = h / scl;
    scale = scl;
    octaves = 8;
    noise_gen = new FastNoise(10);
    noise_gen->SetFractalLacunarity(2.0f);
    noise_gen->SetFractalOctaves(octaves);
//    noise_gen->SetFractalGain(.5f);
    
//    genTextures();
    std::cout << "cols: " << cols << "rows: "<< rows << " widthxheight "<<width<< " "<<height<<std::endl;
    
    update(); // Generate the height map at time of 0
    
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    
    // Bind VAO
    glBindVertexArray(VAO);
    
    // Bind VBO
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices)*vertices.size(), vertices.data(), GL_STATIC_DRAW);
    
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}


/**
 * Constructor for loading terrain from image file. Currently takes in a PPM file as input.
 * Populates the heightmap vertices.
 */
Terrain::Terrain(GLuint shader, const char* filename, int scl){
    this->shaderProgram = shader;
    hmap = TextureHandler::loadPPM(filename, width, height);
    cols = width;// / scl;
    rows = height;// / scl;
    scale = scl;
    
//    genTextures();
    
    // Turn a buffer (unsigned char*) of height values to 2D heightmap (terr[x][y])
    for( int row = 0; row < height; row++){
        for (int col = 0; col < width; col++){
//            std::cout << hmap[(row*width + col) * 3] << " " << hmap[(row*width + col) *3+1] << " " << hmap[(row*width + col)*3+2] << std::endl;
            terrain[row][col] = (float) hmap[(row*width + col)*3] + 100;
//            std::cout << terrain[row][col] << std::endl;
        }
    }
    bool flip = true;
    for(int y = 0; y < rows-1; y++){
        if (flip){
            for(int x = 0; x < cols; x++){
                vertices.push_back(glm::vec3(x*scale, y*scale, terrain[x][y]));
                vertices.push_back(glm::vec3(x*scale, (y+1)*scale, terrain[x][y+1]));
            }
            flip = !flip;
        } else {
            for(int x = cols; x >= 0; x--){
                vertices.push_back(glm::vec3(x*scale, (y+1)*scale, terrain[x][y+1]));
                vertices.push_back(glm::vec3(x*scale, y*scale, terrain[x][y]));
            }
            flip = !flip;
        }
    }
    
    std::cout << "number of vertices..." << vertices.size() << std::endl;
//    std::cout << "cols: " << cols << "rows: "<< rows << " widthxheight "<<width<< " "<<height<<std::endl;
    
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    
    glBindVertexArray(VAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices) * vertices.size(), vertices.data(), GL_STATIC_DRAW);
    
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(GLfloat), (GLvoid*)0);
    
    glBindBuffer(GL_ARRAY_BUFFER,0);
    glBindVertexArray(0);
    
}

Terrain::~Terrain(){
    vertices.clear();
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    vertices = std::vector<glm::vec3>();
}

void Terrain::genTextures(){
    glGenTextures(1, &snowTex);
    glBindTexture(GL_TEXTURE_2D, snowTex);
    int snowW, snowH;
    unsigned char* snow_img = TextureHandler::loadPPM(SNOW, snowH, snowW);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, snowW, snowH, 0, GL_RGBA, GL_UNSIGNED_BYTE, snow_img);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    
    glGenTextures(1, &rockTex);
    glBindTexture(GL_TEXTURE_2D, rockTex);
    int rockW, rockH;
    unsigned char* rock_img = TextureHandler::loadPPM(ROCKS, rockH, rockW);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, rockW, rockH, 0, GL_RGBA, GL_UNSIGNED_BYTE, rock_img);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    
    glGenTextures(1, &fieldsTex);
    glBindTexture(GL_TEXTURE_2D, fieldsTex);
    int fieldsW, fieldsH;
    unsigned char* fields_img = TextureHandler::loadPPM(FIELDS, fieldsH, fieldsW);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, fieldsW, fieldsH, 0, GL_RGBA, GL_UNSIGNED_BYTE, fields_img);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    
    // Get the locations
    //    loc_grass = glGetUniformLocation(shaderProgram, "grassTex");
    loc_snow   = glGetUniformLocation(shaderProgram, "snowTex");
    loc_rock   = glGetUniformLocation(shaderProgram, "rockTex");
    loc_fields = glGetUniformLocation(shaderProgram, "fieldsTex");
}

void Terrain::draw(glm::mat4 C){
    toWorld = C;
    glm::mat4 modelview = Window::V * toWorld;
    
    uProjection = glGetUniformLocation(shaderProgram, "projection");
    uModelview = glGetUniformLocation(shaderProgram, "modelview");
    // Now send these values to the shader program
    glUniformMatrix4fv(uProjection, 1, GL_FALSE, &Window::P[0][0]);
    glUniformMatrix4fv(uModelview, 1, GL_FALSE, &modelview[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, &this->toWorld[0][0]);
    glUniform1f(glGetUniformLocation(shaderProgram, "TERRAIN_WIDTH"), width);
    
    glBindVertexArray(VAO);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // Show lines

    glUniform1f( glGetUniformLocation(shaderProgram, "wire"), true);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, (int)vertices.size());

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    
    // Tell shader to fill triangle strip with color
    glUniform1f( glGetUniformLocation(shaderProgram, "wire"), false);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, (int)vertices.size());
    

    /*
     * Texture code snippet here...
     */
    glUniform1i(glGetUniformLocation(shaderProgram, "texturize"), true);
    glEnable(GL_TEXTURE_2D);
//    glUniform1i(loc_fields, 11);
    glActiveTexture(GL_TEXTURE11);
    glBindTexture(GL_TEXTURE_2D, fieldsTex);
    
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, (int) vertices.size());
    glUniform1i(glGetUniformLocation(shaderProgram, "texturize"), false);
    
    
    glBindVertexArray(0);
}

/**
 * Maps a value that is in the range of istart to istop into the range of ostart to ostop
 * Example: mapping .64 in range 0 to 1 to a value in -100 to 100
 */
float map(float val, float istart, float istop, float ostart, float ostop){
    return ostart + (ostop-ostart) * ((val - istart) / (istop - istart));
}

/**
 * Update vertices each time display_callback is called.
 * When update is caled we are going forward in time
 */
void Terrain::update(){
    vertices.clear();
    flying -= 0.1;
    
    float yoff = flying;
    for(int y = 0; y < rows; y++){
        float xoff = 0;
        for (int x = 0; x < cols; x++){
            // terrain[x][y] = map(noise(xoff, yoff), 0, 1, -100, 100);
            // 0--> 1... * [-100-->100]
            terrain[x][y] = map(noise_gen->GetValueFractal(xoff, yoff), 0, 1, -100, 140);
//            terrain[x][y] = map(noise_gen->GetGradientFractal(xoff, yoff), 0, 1, -100, 150);
            
//            terrain[x][y] = noise_gen->GetGradient(xoff, yoff) * 50;
            xoff += 1.5f;
        }
        yoff += 1.5f;
    }
    bool flip = true;
    
    for(int y = 0; y < rows-1; y++){
        if (flip){
            for(int x = 0; x < cols; x++){
                glm::vec3 a(x*scale, y*scale, terrain[x][y]);
                glm::vec3 b(x*scale, (y+1)*scale, terrain[x][y+1]);
                vertices.push_back(a);
                vertices.push_back(b);
                
                normals.push_back(glm::cross(b, a));
//                vertices.push_back(glm::vec3(x*scale, y*scale, terrain[x][y]));
//                vertices.push_back(glm::vec3(x*scale, (y+1)*scale, terrain[x][y+1]));
            }
            flip = !flip;
        } else {
            for(int x = cols; x >= 0; x--){
                glm::vec3 b(x*scale, y*scale, terrain[x][y]);
                glm::vec3 a(x*scale, (y+1)*scale, terrain[x][y+1]);
                vertices.push_back(a);
                vertices.push_back(b);
                normals.push_back(glm::cross(b, a));
                
//                vertices.push_back(glm::vec3(x*scale, (y+1)*scale, terrain[x][y+1]));
//                vertices.push_back(glm::vec3(x*scale, y*scale, terrain[x][y]));
                
            }
            flip = !flip;
        }
    }
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(vertices), vertices.data());
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    
}

GLuint Terrain::getVAO(){
    return VAO;
}

