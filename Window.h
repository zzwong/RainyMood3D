//
//  Window.h
//  CSE167-Final
//
//  Created by Aaron Wong on 11/26/16.
//  Copyright © 2016 wongsauce. All rights reserved.
//

#ifndef _WINDOW_H_
#define _WINDOW_H_

#include <iostream>

#define GLFW_INCLUDE_GLEXT
#ifdef __APPLE__
#define GLFW_INCLUDE_GLCOREARB
#else
#include <GL/glew.h>
#include <OpenGL/glext.h>
#endif

#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <iostream>

#include "glm/gtc/matrix_access.hpp"
#include "glm/ext.hpp"

#include "shader.h"
#include "TextureHandler.h"
#include "Terrain.h"

#include "SkyBox.h"
#include "OBJObject.h"
#include "Node.h"
#include "Group.h"
#include "Geode.h"
#include "Cube.h"
#include "MatrixTransform.h"
#include "Water.h"

#include "FBO.h"

#include <irrKlang.h>


using namespace irrklang;

#pragma comment(lib, "irrKlang.lib") // link with irrKlang.dll

class Window
{
public:
    static double mouseX, mouseY;
    static double currentScrollOffset;
    static int width, height;
    static glm::mat4 P, V; // Projection and View
    
    static void initialize_objects();
    static void clean_up();
    static GLFWwindow* create_window(int width, int height);
    static void resize_callback(GLFWwindow*, int width, int height);
    static void idle_callback(GLFWwindow*);
    static void display_callback(GLFWwindow*);
    static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void mouse_button_callback(GLFWwindow* window, int key, int action, int mods);
    static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);
    static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
    static glm::vec3 trackballMapping(double xpos, double ypos, int width, int height);
    static void point_callback(GLFWwindow* window);
    static void mouse_move_callback(GLFWwindow*, double, double);
    static void drawSkybox();
    static void drawObjects();
    static void drawReflection();
    static void drawRefraction();
    static void drawAll();
    static void drawWater();
};



#endif


