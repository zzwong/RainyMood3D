#include "Window.h"

using namespace std;

const char* window_title = "CSE 167 Final Project";

int tr_counter = 0;

// Default camera parameters
glm::vec3 cam_pos(0.0f, 0.0f, -70.0f);		// e  | Position of camera
glm::vec3 cam_look_at(0.0f, 0.0f, 0.0f);	// d  | This is where the camera looks at
glm::vec3 cam_up(0.0f, 1.0f, 0.0f);			// up | What orientation "up" is

// Declare shader programs here
GLint shaderProgram;
GLint skyShaderProgram;
GLint waterProgram;
GLint terrainProgram;
GLuint clipPlaneW, clipPlaneN;

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
#define T_V_SHADER_PATH "../terrain.vert"
#define T_F_SHADER_PATH "../terrain.frag"

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

//Pause the terrain
bool pause_key = false;

// San Diego Height Map
#define SD_HEIGHT_MAP "SanDiegoTerrain.ppm"

// dudvMap path
#define DU_DV_MAP "waterdudv.ppm"
#define NORMAL_MAP "waternormal.ppm"
float WAVE_SPEED = 0.12f;
float move_factor = 0.0f;

time_t b_frame = 0.0f;
time_t e_frame = 0.0f;
time_t d_frame = 0.0f;

// Testing Shapes >>>>>>>>> <<< >< ><> <> <>< > ><><><><>
Cube * cube;
SkyBox * skybox;

// Terrain
Terrain * tr;
glm::mat4 trn(1.0f);   // Used for calculating terrain id->matrix
Terrain * hm; // height map...

// Heightmaps
#define SIMPLE_HEIGHT_MAP "fft-terrain.ppm"

// Terrain Textures
#define GRASS "grass.ppm"
#define SNOW "snow.ppm"
#define ROCKS "rocks.ppm"
#define FIELDS "fields.ppm"
GLuint grassTex, snowTex, rockTex, fieldsTex;
GLuint loc_grass, loc_snow, loc_rock, loc_fields;

Water * water;
glm::mat4 water_m(1.0f);

// FBOS
FBO * waterFBO;
//location of textures in shader
GLuint loc_reflection, loc_refraction, loc_dudv, loc_move_factor, loc_cam_pos, loc_normal_map, loc_depth_map;
//Location of textures
GLuint dudvTex, normalTex;

Water* water2;

void Window::initialize_objects()
{
    // Load shader programs.
    shaderProgram = LoadShaders(V_SHADER_PATH, F_SHADER_PATH);
    skyShaderProgram = LoadShaders(SKYBOX_V_SHADER_PATH, SKYBOX_F_SHADER_PATH);
    waterProgram = LoadShaders(W_V_SHADER_PATH, W_F_SHADER_PATH);
    terrainProgram = LoadShaders(T_V_SHADER_PATH, T_F_SHADER_PATH);

    // Textures
//    faces.push_back("2rt.ppm");
//    faces.push_back("2lf.ppm");
//    faces.push_back("2up.ppm");
//    faces.push_back("2dn.ppm");
//    faces.push_back("2bk.ppm");
//    faces.push_back("2ft.ppm");

    faces.push_back("rt.ppm");
    faces.push_back("lf.ppm");
    faces.push_back("up.ppm");
    faces.push_back("dn.ppm");
    faces.push_back("bk.ppm");
    faces.push_back("ft.ppm");
    
    cubeMapTexture = TextureHandler::loadCubemap(faces);
    
    // initialize
    engine = createIrrKlangDevice();
    
    skybox = new SkyBox();
    cube = new Cube(shaderProgram);

    tr = new Terrain(shaderProgram, 2000, 1600, 10);
//    tr = new Terrain(terrainProgram, 2000, 1600, 10);
    tr->update();
    
    // Generate the terrain textures
    glGenTextures(1, &grassTex);
    glBindTexture(GL_TEXTURE_2D, grassTex);
    int grassW, grassH;
    unsigned char* grass_img = TextureHandler::loadPPM(GRASS, grassH, grassW);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, grassW, grassH, 0, GL_RGBA, GL_UNSIGNED_BYTE, grass_img);
    // Filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    glGenTextures(1, &snowTex);
    glBindTexture(GL_TEXTURE_2D, snowTex);
    int snowW, snowH;
    unsigned char* snow_img = TextureHandler::loadPPM(SNOW, snowH, snowW);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, snowW, snowH, 0, GL_RGBA, GL_UNSIGNED_BYTE, snow_img);
    //Filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    glGenTextures(1, &rockTex);
    glBindTexture(GL_TEXTURE_2D, rockTex);
    int rockW, rockH;
    unsigned char* rock_img = TextureHandler::loadPPM(ROCKS, rockH, rockW);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, rockW, rockH, 0, GL_RGBA, GL_UNSIGNED_BYTE, rock_img);
    //Filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    glGenTextures(1, &fieldsTex);
    glBindTexture(GL_TEXTURE_2D, fieldsTex);
    int fieldsW, fieldsH;
    unsigned char* fields_img = TextureHandler::loadPPM(FIELDS, fieldsH, fieldsW);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, rockW, rockH, 0, GL_RGBA, GL_UNSIGNED_BYTE, fields_img);
    //Filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    // Get the locations
    loc_grass = glGetUniformLocation(terrainProgram, "grassTex");
    loc_snow  = glGetUniformLocation(terrainProgram, "snowTex");
    loc_rock  = glGetUniformLocation(terrainProgram, "snowTex");
    loc_fields= glGetUniformLocation(terrainProgram, "fieldsTex");
    
    
    // Done with terrain textures
    
//    hm = new Terrain(shaderProgram, SIMPLE_HEIGHT_MAP, 10);

    //Create the water FBO
    waterFBO = new FBO();
    glUseProgram(waterProgram);
    water = new Water(waterProgram);
//    water2 = new Water(waterProgram);
    //Get the locations
    loc_reflection = glGetUniformLocation(waterProgram, "reflectionTex");
    loc_refraction = glGetUniformLocation(waterProgram, "refractionTex");
    loc_dudv = glGetUniformLocation(waterProgram, "dudvMap");
    loc_move_factor = glGetUniformLocation(waterProgram, "moveFactor");
    loc_cam_pos = glGetUniformLocation(waterProgram, "camPos");
    loc_normal_map = glGetUniformLocation(waterProgram, "normalMap");
    loc_depth_map = glGetUniformLocation(waterProgram, "depthMap");
    
    //Generate dudvTexture
    glGenTextures(1, &dudvTex);
    glBindTexture(GL_TEXTURE_2D, dudvTex);
    int dudvW, dudvH;
    unsigned char* dudv_image = TextureHandler::loadPPM(DU_DV_MAP, dudvH, dudvW);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, dudvW, dudvH, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, dudv_image);
    //Filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    //Generate normalmap
    glGenTextures(1, &normalTex);
    glBindTexture(GL_TEXTURE_2D, normalTex);
    int normW, normH;
    unsigned char* norm_image = TextureHandler::loadPPM(NORMAL_MAP, normH, normW);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, dudvW, dudvH, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, norm_image);
    //Filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    
    //For terrain
    trn *= glm::rotate(glm::mat4(1.0f), glm::pi<float>()/180.0f * 90, glm::vec3(1.0, 0, 0));
    trn *= glm::translate(glm::mat4(1.0f), glm::vec3(-500.0f, -500.0f, 225.0f));
   
    //For water
    water_m *= glm::scale(glm::mat4(1.0f), glm::vec3(30, 1, 30));
    water_m *= glm::translate(glm::mat4(1.0f), glm::vec3(0, -140, 0));
    
    clipPlaneW = glGetUniformLocation(waterProgram, "plane");
    clipPlaneN = glGetUniformLocation(shaderProgram, "plane");
}

void Window::clean_up()
{
    // TODO
    delete(skybox);
    delete(cube);
    delete(tr);
    delete(hm);
    engine->drop();
    glDeleteProgram(shaderProgram);
    glDeleteProgram(skyShaderProgram);
    glDeleteProgram(waterProgram);      // no more drinkable water in LA
    glDeleteProgram(terrainProgram);
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
    
    //Get time (end)
    e_frame = time(NULL);
    d_frame = b_frame - e_frame;
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
        //0.1f is near
        //1000.0f is far
        V = glm::lookAt(cam_pos, cam_look_at, cam_up);
    }
}

void Window::invertPitch(){
    float pitch = glm::asin(cam_look_at.y);
    float yaw = glm::acos(cam_look_at.x/glm::cos(pitch));
//    std::cout << pitch << " : P\n";
//    std::cout << yaw << " : Y\n";
    float invertPitch = -pitch;
    
    cam_look_at.x = glm::cos(invertPitch)*glm::cos(yaw);
    cam_look_at.y = glm::sin(invertPitch);
    cam_look_at.z = glm::cos(invertPitch)*glm::sin(yaw);
}

void Window::drawSkybox(){
    // Skybox
    glUseProgram(skyShaderProgram);
//    V = glm::mat4(glm::mat3(Window::V));
    glEnable(GL_CULL_FACE);
//    glCullFace(GL_BACK);
    glFrontFace(GL_CW);
    glDepthMask(GL_FALSE);
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
    glBindVertexArray(skybox->getVAO());
    glActiveTexture(GL_TEXTURE0);
    glUniform1i(glGetUniformLocation(skyShaderProgram, "skybox"), 0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapTexture);
    skybox->draw(skyShaderProgram);
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    glDepthMask(GL_TRUE);
//  Set things back to normal
    glFrontFace(GL_CCW);
    glCullFace(GL_BACK);
    glDepthMask(GL_TRUE);
    glDisable(GL_CULL_FACE);
}

void Window::drawObjects(){
    drawSkybox();
    
    glUseProgram(shaderProgram);
//    glUseProgram(terrainProgram);
    glEnable(GL_TEXTURE_2D);
    glBindVertexArray(tr->getVAO());
    glActiveTexture(GL_TEXTURE9);
    glUniform1i(glGetUniformLocation(shaderProgram, "terrain"), 0);
//    glUniform1i(glGetUniformLocation(terrainProgram, "terrain"), 9);
    glBindTexture(GL_TEXTURE_2D, grassTex);
    
    tr->draw(trn);
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    
    
    
    
    cube->draw(glm::mat4(1.0f));
}
void Window::drawReflection(){
    //Render everything above water (reflection)
    glUniform4f(clipPlaneN, 0.0f,1.0f,0.0f,-140.0f);
    //gotta move camera down 2*distance_to_water -70 - (-140) = 70*2 = 210 ayyy
    float distance = 2*(70);
    cam_pos.y -= distance;
    invertPitch();
    
//    tr->draw(trn);
//    cube->draw(glm::mat4(1.0f));
    waterFBO->bind(waterFBO->getReflectionFBO());
    
    //Clear colors
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //Draw things here <<<<<<<<
    drawObjects();

    //Move camera back
    invertPitch();
    cam_pos.y += distance;
}
void Window::drawRefraction(){
    glUniform4f(clipPlaneN, 0.0f, -1.0f, 0.0f, 140.0f);
    waterFBO->bind(waterFBO->getRefractionFBO());
    //Clear colors
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //Draw things here <<<<<<<<
    drawObjects();
}

//<<<<<<< HEAD
//    
//    //Render everything below water (refraction)
//    glUniform4f(clipPlaneW, 0.0f, 1.0f, 0.0f, 140.0f);
//    water->bindFrameBuffer(water->getRefractionFBO(), width, height);
//    
////    tr->draw(trn);
//    cube->draw(glm::mat4(1.0f));
//    
//    //Render everything
//    glUniform4f(clipPlaneN, 0.0f, 0.0f,0.0f,-140.0f);
//    glUseProgram(waterProgram);
//    glUniform4f(clipPlaneW, 0.0f, 0.0f, 0.0f, 140.0f);
//    water->unbindFrameBuffer();
//    glUseProgram(shaderProgram);
//    tr->draw(trn);
////    hm->draw(trn);
//    cube->draw(glm::mat4(1.0f));
//=======
void Window::drawAll(){
    glUniform4f(clipPlaneN, 0.0f, 0.0f,0.0f, 0.0f);
    glUseProgram(waterProgram);
    glUniform4f(clipPlaneW, 0.0f, 0.0f, 0.0f, 0.0f);
    waterFBO->unbind();
    
    //Draw things here <<<<<<<<
    drawObjects();

}

void Window::drawWater(){
    glUseProgram(waterProgram);
    //Move the waves
    move_factor += (WAVE_SPEED*d_frame)+1.000005f;
    move_factor = fmod(move_factor, 1.0f);
    glUniform1f(loc_move_factor, move_factor);
    
    //Send camera position
    glUniform3f(loc_cam_pos, cam_pos.x, cam_pos.y, cam_pos.z);
    
    //Connect the texture units
    glUniform1i(loc_reflection, 4); //texunit 4 REFLECTION
    glUniform1i(loc_refraction, 5); //texunit 5 REFRADTION
    glUniform1i(loc_dudv, 6); //texunit 6 dudv
    glUniform1i(loc_normal_map, 7);
    glUniform1i(loc_depth_map, 8);
    
    //Bind vertex array for the water
    glBindVertexArray(water->getVAO());
    glEnableVertexAttribArray(0);
    glEnable(GL_TEXTURE_2D);
    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, waterFBO->getReflTex());
    glActiveTexture(GL_TEXTURE5);
    glBindTexture(GL_TEXTURE_2D, waterFBO->getRefrTex());
    glActiveTexture(GL_TEXTURE6);
    glBindTexture(GL_TEXTURE_2D, dudvTex);
    glActiveTexture(GL_TEXTURE7);
    glBindTexture(GL_TEXTURE_2D, normalTex);
    glActiveTexture(GL_TEXTURE8);
    glBindTexture(GL_TEXTURE_2D, waterFBO->getRefrDepth());
    
    
    glDisable(GL_CULL_FACE);
    water->draw(water_m);
}

void Window::display_callback(GLFWwindow * window)
{
    tr_counter += 1;

    //Get time (seconds)
    b_frame = time(NULL);
    
    // Clear the color and depth buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // Set the matrix mode to GL_MODELVIEW
    drawSkybox();
    
    glUseProgram(shaderProgram);
    

    if(!pause_key)
        tr->update();
    //Draw reflecions
    drawReflection();
    
    //Render everything below water (refraction)
    drawRefraction();
    
    //Render everything
    drawAll();

    //Draw the water
    drawWater();
    
    // Gets events, including input such as keyboard and mouse or window resizinh
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
        cam_pos = cam_pos - glm::vec3(0,0,-0.5);
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
        
        if (key == GLFW_KEY_P){
            pause_key = !pause_key;
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
