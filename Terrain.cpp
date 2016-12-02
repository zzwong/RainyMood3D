#include <stdio.h>
#include "Terrain.h"
#include "Window.h"

Terrain::Terrain(GLuint shader, int w, int h, int scl){
    this->shaderProgram = shader;
    cols = w / scl;
    rows = h / scl;
    scale = scl;
    
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glGenBuffers(1, &NBO);
    
    // Bind VAO
    glBindVertexArray(VAO);
    
    // Bind VBO
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
//    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_), vertices_, GL_STATIC_DRAW);
    
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
    
    // Bind EBO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
//    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices_), indices_, GL_STATIC_DRAW);
    
    // Bind the normals
    glBindBuffer(GL_ARRAY_BUFFER, NBO);
//    glBufferData(GL_ARRAY_BUFFER, sizeof(normals_), normals_, GL_STATIC_DRAW);
    
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3*sizeof(GLfloat), (GLvoid*)0);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

Terrain::~Terrain(){
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &NBO);
    glDeleteBuffers(1, &EBO);
}

void Terrain::draw(glm::mat4 C){
    flying -= 0.1;
    
    glm::mat4 modelview = Window::V * toWorld;
    
    uProjection = glGetUniformLocation(shaderProgram, "projection");
    uModelview = glGetUniformLocation(shaderProgram, "modelview");
    // Now send these values to the shader program
    glUniformMatrix4fv(uProjection, 1, GL_FALSE, &Window::P[0][0]);
    glUniformMatrix4fv(uModelview, 1, GL_FALSE, &modelview[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, &this->toWorld[0][0]);
    
    glBindVertexArray(VAO);
//    glDrawElements(GL_TRIANGLE_STRIP, 36, GL_UNSIGNED_INT, 0);
    
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    //glDrawArrays(GL_POINTS, 0, 24);
    glBindVertexArray(0);
}

void Terrain::update(){
    
}