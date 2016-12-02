#include "Window.h"

using namespace std;

const char* window_title = "CSE 167 Final Project";

// Default camera parameters
glm::vec3 cam_pos(0.0f, 10.0f, 40.0f);		// e  | Position of camera
glm::vec3 cam_look_at(0.0f, 0.0f, 0.0f);	// d  | This is where the camera looks at
glm::vec3 cam_up(0.0f, 1.0f, 0.0f);			// up | What orientation "up" is

// Declare shader programs here
GLint shaderProgram;
GLint skyShaderProgram;

// Texture
GLuint cubeMapTexture;
vector<const char*> faces;

// define shader paths here
#define V_SHADER_PATH "../shader.vert"
#define F_SHADER_PATH "../shader.frag"
#define SKYBOX_V_SHADER_PATH "../skyboxShader.vert"
#define SKYBOX_F_SHADER_PATH "../skyboxShader.frag"

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

    tr = new Terrain(shaderProgram, 1000, 800, 5);
    tr->update();

//    water = new Water(shaderProgram);

}

void Window::clean_up()
{
    // TODO
    // delete(skybox)
    delete(cube);
    engine->drop();
    glDeleteProgram(shaderProgram);
    glDeleteProgram(skyShaderProgram);
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
//    skybox->draw(skyShaderProgram);
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    glDepthMask(GL_TRUE);
    
    glDisable(GL_CULL_FACE);
    
    glUseProgram(shaderProgram);

//    cube->draw(glm::mat4(1.0f));
    
//    tr->update();
    tr->draw(glm::mat4(1.0f));

    //cube->draw(glm::mat4(1.0f));

//    water->draw(glm::mat4(1.0f));

    
    
    // Gets events, including input such as keyboard and mouse or window resizing
    glfwPollEvents();
    // Swap buffers
    glfwSwapBuffers(window);
}

void Window::mouse_button_callback(GLFWwindow *window, int key, int action, int mods)
{
    if (key == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS){
        mouseRotateMode = true;
        glfwGetCursorPos(window, &mouseX, &mouseY);
        cout << "pressed" << endl;
    }
    if (key == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE){
        mouseRotateMode = false;
        glfwGetCursorPos(window, &mouseX, &mouseY);
        cout << "released" << endl;
    }
    if (mods == GLFW_MOD_SHIFT){
        if (key == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS){
            mouseTranslateMode = true;
            glfwGetCursorPos(window, &mouseX, &mouseY);
            cout << "shift pressed" << endl;
            
        }
        if (key == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE){
            mouseTranslateMode = false;
            cout << "shift press release" << endl;
        }
    } else {
        mouseTranslateMode = false;
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

void Window::cursor_position_callback(GLFWwindow *window, double xpos, double ypos)
{
    // Rotate Model
    if (mouseRotateMode){
        
        //glfwGetCursorPos(window, &currX, &currY);
        glm::vec3 curr_pos = track_ball_mapping((float) mouseX, (float)mouseY);
        //glm::vec3 curr_pos = track_ball_mapping(currX, currY);
        glm::vec3 destination = track_ball_mapping((float)xpos, (float)ypos);
        
        //glm::vec3 rot_vec = glm::cross(curr_pos, destination);
        float rot_angle = glm::acos( glm::dot(curr_pos, destination) / (glm::length(curr_pos) * glm::length(destination)));
        

        glm::vec3 rotAxis = glm::cross(curr_pos, destination);
        //float rotAngle = 0.01f;
        glm::mat4 rotMat = glm::rotate(glm::mat4(1.0f), glm::radians(rot_angle+1), rotAxis);
        glm::vec4 position(cam_pos, 1.0f);
        glm::vec4 newCamPos = rotMat * position;
        glm::vec4 upCam(cam_up, 1.0f);
        glm::vec4 newCamUp = rotMat * upCam;
        cam_pos.x = newCamPos.x;
        cam_pos.y = newCamPos.y;
        cam_pos.z = newCamPos.z;
        cam_up.x = newCamUp.x;
        cam_up.y = newCamUp.y;
        cam_up.z = newCamUp.z;
        V = glm::lookAt(cam_pos, cam_look_at, cam_up);
    }
    // Translate Model
    if (mouseTranslateMode){
        //        currentObj->translateX((-.05f)*(newX - mouseX));
        //        currentObj->translateY((-.05f)*(newY - mouseY));
        cout << "translating" << endl;
        //float xdelta = xpos - mouseX;// - xpos;
        //float ydelta = ypos-mouseY;// - ypos;
        //        cout << xdelta << " " << ydelta << endl;
        mouseX = xpos;
        mouseY = ypos;
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


