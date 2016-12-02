#include "Window.h"

using namespace std;

const char* window_title = "CSE 167 Final Project";

// Default camera parameters
glm::vec3 cam_pos(0.0f, 10.0f, 30.0f);		// e  | Position of camera
glm::vec3 cam_look_at(0.0f, 0.0f, 0.0f);	// d  | This is where the camera looks at
glm::vec3 cam_up(0.0f, 1.0f, 0.0f);			// up | What orientation "up" is

// Declare shader programs here TODO

// define shader paths here

// static vars
int Window::width, Window::height;
double Window::mouseX = 0;
double Window::mouseY = 0;
glm::mat4 Window::P, Window::V;

// Audio related
ISoundEngine* engine;
#define SPLOSION "explosion.wav"

void Window::initialize_objects()
{
    // TODO
    
    // initialize
    engine = createIrrKlangDevice();
}

void Window::clean_up()
{
    // TODO
    // delete(skybox)
    // glDeleteProgram(shaderProgram);
    engine->drop();
}

GLFWwindow* Window::create_window(int width, int height)
{
    if (!glfwInit())
    {
        fprintf(stderr, "Failed to initialize GLFW\n");
        return NULL;
    }
    
    // 4x AntiAliasing
    glfwWindowHint(GLFW_SAMPLES, 4);
    
#ifdef __APPLE__ // Because Apple hates comforming to standards
    // Ensure that minimum OpenGL version is 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    // Enable forward compatibility and allow a modern OpenGL context
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    
    // Create GLFW window
    GLFWwindow* window = glfwCreateWindow(width, height, window_title, NULL, NULL);
    
    // Error check
    if (!window)
    {
        fprintf(stderr, "Failed to open GLFW window.\n");
        fprintf(stderr, "Either GLFW is not installed or your graphics card does not support modern OpenGL.\n");
        glfwTerminate();
        return NULL;
    }
    
    // Make the context of the window
    glfwMakeContextCurrent(window);
    
    // Set swap interval to 1
    glfwSwapInterval(1);
    
    // Get the width and height of the framebuffer to properly resize the window
    glfwGetFramebufferSize(window, &width, &height);
    
    // Call the resize callback to make sure things get drawn immediately
    Window::resize_callback(window, width, height);
    
    return window;
}

void Window::idle_callback()
{
    // TODO
}

void Window::resize_callback(GLFWwindow * window, int width, int height)
{
    
}

void Window::display_callback(GLFWwindow * window)
{
    // Clear the color and depth buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // Set the matrix mode to GL_MODELVIEW
//    glMatrixMode(GL_MODELVIEW);
    
    // Gets events, including input such as keyboard and mouse or window resizing
    glfwPollEvents();
    // Swap buffers
    glfwSwapBuffers(window);
}

void Window::mouse_button_callback(GLFWwindow *window, int key, int action, int mods)
{
    // TODO
}

void Window::scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
{
    // TODO   
}

void Window::cursor_position_callback(GLFWwindow *window, double xpos, double ypos)
{
    //TODO    
}

void Window::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
 
    if (action == GLFW_PRESS){
        if (key == GLFW_KEY_ESCAPE)
        {
            // Close the window. This causes the program to also terminate.
            glfwSetWindowShouldClose(window, GL_TRUE);
            cout << "escape button pressed" << endl;
        }
        if (key == GLFW_KEY_E){
            engine->play2D(SPLOSION);
        }
    }
}

glm::vec3 Window::track_ball_mapping(float x, float y)
{
    glm::vec3 v;    // Synthesized 3D position of th emouse location on trackball
    float d;        // the depth of the mouse location, Delta b/t the center of the trackball and the z position of the mouse
    v.x = (2.0 * x - width) / width;
    v.y = (height - 2.0 * y) / height;
    v.z = 0.0;
    d = glm::length(v);
    d = (d < 1.0) ? d : 1.0;
    v.z = glm::sqrt(1.001 - d*d);
    return glm::normalize(v);
}


