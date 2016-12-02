#ifndef OBJOBJECT_H
#define OBJOBJECT_H

#define GLFW_INCLUDE_GLEXT
#ifdef __APPLE__
#include <OpenGL/gl3.h>
#include <OpenGL/glext.h>
#else
#include <GL/glew.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include <GLFW/glfw3.h>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <string.h>
#include <iostream>
#include <fstream>

struct Face {
    int v[3];
    int n[3];
};

class OBJObject
{
private:
    std::vector<int> indices;
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec3> colors;
    std::vector<Face> faces;           // holds faces or triangle point coordinates
    glm::mat4 toWorld;
    float angle;
    glm::mat4 spinMat;
    glm::mat4 translationMat;
    glm::mat4 rotationMat;
    glm::mat4 scaleMat;


public:
	OBJObject(const char* filepath);
    ~OBJObject();
    
    // These variables are needed for the shader program
    GLuint VBO, VAO, EBO, NBO;
    GLuint uProjection, uModelview, uModel;
    
    
    glm::mat4 getToWorldMat();
    std::vector<glm::vec3> getVertices();
    std::vector<glm::vec3> getNormals();
    std::vector<std::string> split(std::string src);
    std::vector<std::string> split(std::string src, char delim);
	void parse(const char* filepath);
	void draw(GLuint shaderProgram);
    void spin(float deg);
    void update();
    void scale(bool mode);
    void translate(int dir);
    void rotate(bool axis);
    void reset();
    void clearTransformMatrices();
    void translateXY(double xpos, double ypos);
    void translateX(double xpos);
    void translateY(double ypos);
    void rotate(float angle, glm::vec3 axis);
};

#endif