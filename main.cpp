//
//  main.cpp
//  CSE167-Final
//
//  Created by Aaron Wong on 11/24/16.
//  Copyright © 2016 wongsauce. All rights reserved.
//

#include <iostream>
#include "main.h"
#include <string.h>

using namespace std;

GLFWwindow* window;

//int main(int argc, const char * argv[]) {
//    // insert code here...
//    std::cout << "Hello, World!\n";
//    return 0;
//}

void error_callback(int error, const char* description)
{
    // print error
    fputs(description, stderr);
}

void setup_callbacks()
{
    glfwSetMouseButtonCallback(window, Window::mouse_button_callback);
    glfwSetErrorCallback(error_callback);
    glfwSetKeyCallback(window, Window::key_callback);
    glfwSetCursorPosCallback(window, Window::cursor_position_callback);
    glfwSetScrollCallback(window, Window::scroll_callback);
    glfwSetFramebufferSizeCallback(window, Window::resize_callback);
}

void setup_glew()
{
#ifndef __APPLE__
    GLenum err = glewInit();
    if (GLEW_OK != err)
    {
        // glewInit failed. Serious error ocurred
        fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
        glfwTerminate();
    }
#endif
}

void setup_opengl_settings()
{
#ifndef __APPLE__
    // Setup GLEW. Don't do this on OSX systems.
    setup_glew();
#endif
    // Enable depth buffering
    glEnable(GL_DEPTH_TEST);
    // Related to shaders and z value comparisons for the depth buffer
    glDepthFunc(GL_LEQUAL);
    // Set polygon drawing mode to fill front and back of each polygon
    // You can also use the paramter of GL_LINE instead of GL_FILL to see wireframes
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    // Disable backface culling to render both sides of polygons
    glDisable(GL_CULL_FACE);
    // Set clear color
    //	glClearColor(0.05f, 0.8f, 0.85f, 1.0f);
    //    glClearColor(1.0f, 0.8f, 0.85f, 1.0f);
    glClearColor(1.0, 0.3, 0.5, 1);
}

void print_versions()
{
    // Get info of GPU and supported OpenGL version
    printf("Renderer: %s\n", glGetString(GL_RENDERER));
    printf("OpenGL version supported %s\n", glGetString(GL_VERSION));
    
    //If the shading language symbol is defined
#ifdef GL_SHADING_LANGUAGE_VERSION
    std::printf("Supported GLSL version is %s.\n", (char *)glGetString(GL_SHADING_LANGUAGE_VERSION));
#endif
}

int main(void)
{
    // Create the GLFW window
    window = Window::create_window(640, 480);
    // Print OpenGL and GLSL versions
    print_versions();
    // Setup callbacks
    setup_callbacks();
    // Setup OpenGL settings, including lighting, materials, etc.
    setup_opengl_settings();
    // Initialize objects/pointers for rendering
    Window::initialize_objects();
    
    // Loop while GLFW window should stay open
    while (!glfwWindowShouldClose(window))
    {
        // Main render display callback. Rendering of objects is done here.
        Window::display_callback(window);
        // Idle callback. Updating objects, etc. can be done here.
        Window::idle_callback();
    }
    
    Window::clean_up();
    // Destroy the window
    glfwDestroyWindow(window);
    // Terminate GLFW
    glfwTerminate();
    
    exit(EXIT_SUCCESS);
}
