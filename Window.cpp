#include "Window.h"

using namespace std;

const char* window_title = "CSE 167 Final Project";

int tr_counter = 0;

// Default camera parameters
glm::vec3 cam_pos(0.0f, 100.0f, -70.0f);		// e  | Position of camera
glm::vec3 cam_look_at(0.0f, 0.0f, 0.0f);	// d  | This is where the camera looks at
glm::vec3 cam_up(0.0f, 1.0f, 0.0f);			// up | What orientation "up" is

// Declare shader programs here
GLint shaderProgram;
GLint skyShaderProgram;
GLint waterProgram;

// Texture
GLuint cubeMapTexture;
vector<const char*> faces;

// Mouse
int leftMouseDown;
int rightMouseDown;

// define shader paths here
#define V_SHADER_PATH "../shader.vert"
#define F_SHADER_PATH "../shader.frag"
#define SKYBOX_V_SHADER_PATH "../skyboxShader.vert"
#define SKYBOX_F_SHADER_PATH "../skyboxShader.frag"
#define W_V_SHADER_PATH "../waterShader.vert"
#define W_F_SHADER_PATH "../waterShader.frag"

// Camera movement
glm::vec3 lastPoint;
double lastX, lastY;

// static vars
int Window::width, Window::height;
double Window::mouseX = 0;
double Window::mouseY = 0;
glm::mat4 Window::P, Window::V;
bool mouseRotateMode = false;
bool mouseTranslateMode = false;
int mode = 0;

// Audio related
ISoundEngine* engine;
#define SPLOSION "explosion.wav"


// Testing Shapes >>>>>>>>> <<< >< ><> <> <>< > ><><><><>
Cube * cube;
SkyBox * skybox;

Terrain * tr;

Water * water;


void Window::initialize_objects()
{
    // Load shader programs.
    shaderProgram = LoadShaders(V_SHADER_PATH, F_SHADER_PATH);
    skyShaderProgram = LoadShaders(SKYBOX_V_SHADER_PATH, SKYBOX_F_SHADER_PATH);
    waterProgram = LoadShaders(W_V_SHADER_PATH, W_F_SHADER_PATH);

    // Textures
    faces.push_back("2rt.ppm");
    faces.push_back("2lf.ppm");
    faces.push_back("2up.ppm");
    faces.push_back("2dn.ppm");
    faces.push_back("2bk.ppm");
    faces.push_back("2ft.ppm");
    cubeMapTexture = TextureHandler::loadCubemap(faces);
    
    // initialize
    engine = createIrrKlangDevice();
    
    skybox = new SkyBox();
    cube = new Cube(shaderProgram);

    tr = new Terrain(shaderProgram, 2000, 1600, 10);
    tr->update();

    water = new Water(waterProgram);

}

void Window::clean_up()
{
    // TODO
    delete(skybox);
    delete(cube);
    delete(tr);
    engine->drop();
    glDeleteProgram(shaderProgram);
    glDeleteProgram(skyShaderProgram);
    glDeleteProgram(waterProgram);      // no more drinkable water in LA
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

void Window::idle_callback(GLFWwindow* window)
{
    cube->update();
}

void Window::resize_callback(GLFWwindow * window, int width, int height)
{
    Window::width = width;
    Window::height = height;
    // Set the viewport size. This is the only matrix that OpenGL maintains for us in modern OpenGL!
    glViewport(0, 0, width, height);
    
    if (height > 0)
    {
        P = glm::perspective(45.0f, (float)width / (float)height, 0.1f, 1000.0f);
        V = glm::lookAt(cam_pos, cam_look_at, cam_up);
    }
}

void Window::display_callback(GLFWwindow * window)
{
    tr_counter += 1;
    
//    cout << cam_pos.x << " " << cam_pos.y << " " << cam_pos.z << endl;
    // Clear the color and depth buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // Set the matrix mode to GL_MODELVIEW
//    glMatrixMode(GL_MODELVIEW);
    
    // Skybox
    glUseProgram(skyShaderProgram);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CW);
    glDepthMask(GL_FALSE);
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
    glBindVertexArray(skybox->getVAO());
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapTexture);
    skybox->draw(skyShaderProgram);
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    glDepthMask(GL_TRUE);
    
//    glDisable(GL_CULL_FACE);
    glFrontFace(GL_CCW);
    glCullFace(GL_BACK);
    glDepthMask(GL_TRUE);
    
    glUseProgram(shaderProgram);

//    cube->draw(glm::mat4(1.0f));
//    if (tr_counter % 5 == 0)
    tr->update();
    glm::mat4 transform(glm::mat4(1.0f));
    transform *= glm::rotate(glm::mat4(1.0f), glm::pi<float>()/180.0f * 90, glm::vec3(1.0, 0, 0));
    transform *= glm::translate(glm::mat4(1.0f), glm::vec3(-500.0f, -500.0f, 225.0f));
    tr->draw(transform);
//    tr->draw(glm::translate(glm::mat4(1.0f), glm::vec3(-30.0f, -5.0f, 0)));
//    tr->draw(glm::mat4(1.0f));

    //cube->draw(glm::mat4(1.0f));

    //Draw the water
    glDisable(GL_CULL_FACE);
    glUseProgram(waterProgram);
    glm::mat4 water_m(glm::mat4(1.0f));
    water_m *= glm::scale(glm::mat4(1.0f), glm::vec3(30, 1, 30));
    //water_m *= glm::rotate(glm::mat4(1.0f), .1f, glm::vec3(1.0, 0,0));
    water_m *= glm::translate(glm::mat4(1.0f), glm::vec3(0, -140, 0));
    water->draw(water_m);

    // Gets events, including input such as keyboard and mouse or window resizing
    glfwPollEvents();
    // Swap buffers
    glfwSwapBuffers(window);
}

void Window::mouse_button_callback(GLFWwindow *window, int key, int action, int mods)
{
    if(key == GLFW_MOUSE_BUTTON_RIGHT){
        rightMouseDown = (action == GLFW_PRESS);
    }
    if(key == GLFW_MOUSE_BUTTON_LEFT){
        leftMouseDown = (action == GLFW_PRESS);
        glfwGetCursorPos(window, &lastX, &lastY);
    }
}

void Window::scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
{
    if (yoffset < 0){
        // negative y offset is to zoom out
        cam_pos = cam_pos + glm::vec3(0,0, 0.5);
        V = glm::lookAt(cam_pos, cam_look_at, cam_up);

    } else {
        // pos y offset is to zoom in
        cam_pos = cam_pos - glm::vec3(0,0,0.5);
        V = glm::lookAt(cam_pos, cam_look_at, cam_up);
    }
}


void Window::mouse_move_callback(GLFWwindow* window, double xpos, double ypos){
    
    if(leftMouseDown) {
        float angle;
        angle = (float) (lastX - xpos) / 100.0f;
        cam_pos = glm::vec3(glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0.0f, 1.0f, 0.0f)) * glm::vec4(cam_pos, 1.0f));
        cam_up = glm::vec3(glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0.0f, 1.0f, 0.0f)) * glm::vec4(cam_up, 1.0f));
        //Now rotate vertically based on current orientation
        angle = (float) (ypos - lastY) / 100.0f;
        glm::vec3 axis = glm::cross((cam_pos - cam_look_at), cam_up);
        cam_pos = glm::vec3(glm::rotate(glm::mat4(1.0f), angle, axis) * glm::vec4(cam_pos, 1.0f));
        cam_up = glm::vec3(glm::rotate(glm::mat4(1.0f), angle, axis) * glm::vec4(cam_up, 1.0f));
        // Now update the camera
        V = glm::lookAt(cam_pos, cam_look_at, cam_up);
        lastX = xpos;
        lastY = ypos;
    }
    
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

glm::vec3 Window::trackballMapping(double xpos, double ypos, int width, int height){
    glm::vec3 point;
    float d;
    
    point.x = (2.0f*xpos - width)/(width);
    point.y = (height - 2.0f*ypos)/(height);
    point.z = 0.0f;
    
    d = glm::length(point);
    d = (d < 1.0f) ? d : 1.0f;
    point.z = sqrtf(1.001f - d*d);
    point = glm::normalize(point);
    
    return point;
}

void Window::point_callback(GLFWwindow* window){
    //Get mouse pos
    double lastx, lasty;
    glfwGetCursorPos(window, &lastx, &lasty);
    
    //Update last point
    lastPoint = trackballMapping(lastx, lasty, width, height);
}
