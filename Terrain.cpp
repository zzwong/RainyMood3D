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
    noise_gen = new FastNoise();
    
    update();
    
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
//    glGenBuffers(1, &EBO);
//    glGenBuffers(1, &NBO);
    
    // Bind VAO
    glBindVertexArray(VAO);
    
    // Bind VBO
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices)*vertices.size(), vertices.data(), GL_STATIC_DRAW);
    
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
    
    // Bind EBO
//    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
//    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices_), indices_, GL_STATIC_DRAW);
    
    // Bind the normals
//    glBindBuffer(GL_ARRAY_BUFFER, NBO);
//    glBufferData(GL_ARRAY_BUFFER, sizeof(normals_), normals_, GL_STATIC_DRAW);
    
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3*sizeof(GLfloat), (GLvoid*)0);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

Terrain::~Terrain(){
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
//    glDeleteBuffers(1, &NBO);
//    glDeleteBuffers(1, &EBO);
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
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
//    glDrawElements(GL_TRIANGLE_STRIP, (int)vertices.size(), GL_UNSIGNED_INT, 0);
//    std::cout << glGetError() << std::endl;
    glUniform1f( glGetUniformLocation(shaderProgram, "wire"), true);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, (int)vertices.size());
//    std::cout << glGetError() << std::endl;
    
//    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    //glDrawArrays(GL_POINTS, 0, 24);
    
//    vertices.clear();
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    
    glUniform1f( glGetUniformLocation(shaderProgram, "wire"), false);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, (int)vertices.size());
    
    
    glBindVertexArray(0);
}

float map(float val, float istart, float istop, float ostart, float ostop){
    return ostart + (ostop-ostart) * ((val - istart) / (istop - istart));
}

void Terrain::update(){
//    glBindVertexArray(VAO);
    vertices.clear();
    flying -= 0.1;
    
    float yoff = flying;
    for(int y = 0; y < rows; y++){
        float xoff = 0;
        for (int x = 0; x < cols; x++){
            // terrain[x][y] = map(noise(xoff, yoff), 0, 1, -100, 100);
            // 0--> 1... * [-100-->100]

            terrain[x][y] = map(noise_gen->GetValueFractal(xoff, yoff), 0, 1, -100, 100);
//            terrain[x][y] = noise_gen->GetGradient(xoff, yoff) * 50;
            xoff += 2.5;
        }
        yoff += 2.5;
    }
    
    for (int y = 0; y < rows-1; y++){
        for (int x = 0; x < cols; x++){
            vertices.push_back(glm::vec3(x*scale, y*scale, terrain[x][y]  ));
            vertices.push_back(glm::vec3(x*scale, (y+1)*scale, terrain[x][y+1]));
            
        }
    }
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(vertices), vertices.data());
}