#include <stdio.h>
#include "Terrain.h"
#include "Window.h"
#include <random>


float map(float val, float istart, float istop, float ostart, float ostop);

Terrain::Terrain(GLuint shader, int w, int h, int scl){
    this->shaderProgram = shader;
    width = w;
    height = h;
    cols = w / scl;
    rows = h / scl;
    scale = scl;
    noise_gen = new FastNoise(3);
    
    
    update(); // Generate the height map at time of 0
    
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    
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

Terrain::Terrain(GLuint shader, const char* heightmap, int scl){
    this->shaderProgram = shader;
    hmap = TextureHandler::loadPPM(heightmap, width, height);
    cols = width / scl;
    rows = height / scl;
    scale = scl;
    
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    
    glBindVertexArray(VAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(heightmap), heightmap, GL_STATIC_DRAW);
    
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(GLfloat), (GLvoid*)0);
    
    glBindBuffer(GL_ARRAY_BUFFER,0);
    glBindVertexArray(0);
    
}

Terrain::~Terrain(){
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
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
    
    glBindVertexArray(VAO);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // Show lines

    glUniform1f( glGetUniformLocation(shaderProgram, "wire"), true);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, (int)vertices.size());

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    
    // Tell shader to fill triangle strip with color
    glUniform1f( glGetUniformLocation(shaderProgram, "wire"), false);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, (int)vertices.size());
    
    
    glBindVertexArray(0);
}

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
//            terrain[x][y] = map(noise_gen->GetValueFractal(xoff, yoff), 0, 1, -100, 100);
            terrain[x][y] = map(noise_gen->GetGradientFractal(xoff, yoff), 0, 1, -100, 150);
            
//            terrain[x][y] = noise_gen->GetGradient(xoff, yoff) * 50;
            xoff += 1.5f;
        }
        yoff += 1.5f;
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
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(vertices), vertices.data());
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    
}
