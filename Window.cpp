#include "Window.h"

using namespace std;

const char* window_title = "CSE 167 Final Project";

int tr_counter = 0;

// Default camera parameters
glm::vec3 cam_pos(0.0f, 0.0f, -100.0f);		// e  | Position of camera
glm::vec3 cam_look_at(0.0f, 0.0f, 0.0f);	// d  | This is where the camera looks at
glm::vec3 cam_up(0.0f, 1.0f, 0.0f);			// up | What orientation "up" is

// Declare shader programs here
GLint shaderProgram;
GLint skyShaderProgram;
GLint waterProgram;
GLint terrainProgram;
GLint particleProgram;
GLint fullScreenShader;
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
#define P_SHADER_V "../particleShader.vert"
#define P_SHADER_F "../particleShader.frag"
#define FS_SHADER_V "../fullScreenShader.vert"
#define FS_SHADER_F "../fullScreenShader.frag"


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
#define RAIN "rain.wav"
#define RAIN_1 "rain1.wav"
#define THUNDER_1 "thunder1.wav"
#define THUNDER_2 "thunder2.wav"
#define THUNDER_3 "thunder3.wav"
#define THUNDER_4 "thunder4.wav"


// Key control
bool pause_key = false;
bool hmap = false;

// dudvMap path
#define DU_DV_MAP "waterdudv.ppm"
#define NORMAL_MAP "waternormal.ppm"
//float WAVE_SPEED = 0.001875f;
float WAVE_SPEED = 0.0009375f;
float move_factor = 0.0f;


// Testing Shapes >>>>>>>>> <<< >< ><> <> <>< > ><><><><>
Cube * cube;
SkyBox * skybox;

// Terrain
Terrain * tr;
glm::mat4 trn(1.0f);   // Used for calculating terrain id->matrix
glm::mat4 hm_mat(1.0f);
Terrain * hm; // height map...

// Heightmaps
#define SIMPLE_HEIGHT_MAP "fft-terrain.ppm"
#define SD_HEIGHT_MAP "SanDiegoTerrain.ppm"

// Terrain Textures
//#define GRASS "grass.ppm"
#define SNOW "snow.ppm"
#define ROCKS "rocks.ppm"
#define FIELDS "fields.ppm"
GLuint snowTex, rockTex, fieldsTex;
GLuint loc_snow, loc_rock, loc_fields;

Water * water;
glm::mat4 water_m(1.0f);

// FBOS
FBO * waterFBO;
//location of textures in shader
GLuint loc_reflection, loc_refraction, loc_dudv, loc_move_factor, loc_cam_pos, loc_normal_map, loc_depth_map;
//Location of textures
GLuint dudvTex, normalTex;
//Post processing locations
GLuint loc_gauss, loc_neon;
bool gauss_on = false, neon_on = false;

//Time
clock_t timer;
double delta_time;
int thunder_timer = 0;
bool thunder = false;
int sampledAt = 0;

//Particles
ParticleGen * generator;
int Window::parts_drawn = 0;
bool parts = false;

//Post-processing
FullscreenQuad * screen;
GLuint loc_screen;
FBO * screenFBO;

void Window::initialize_objects()
{
    // Load shader programs.
    shaderProgram = LoadShaders(V_SHADER_PATH, F_SHADER_PATH);
    skyShaderProgram = LoadShaders(SKYBOX_V_SHADER_PATH, SKYBOX_F_SHADER_PATH);
    waterProgram = LoadShaders(W_V_SHADER_PATH, W_F_SHADER_PATH);
    terrainProgram = LoadShaders(T_V_SHADER_PATH, T_F_SHADER_PATH);
    particleProgram = LoadShaders(P_SHADER_V, P_SHADER_F);
    fullScreenShader = LoadShaders(FS_SHADER_V, FS_SHADER_F);
    
    //Particles
    generator = new ParticleGen(10000, 0, particleProgram);
    
    //New full screen quad
    screen = new FullscreenQuad(fullScreenShader);
    
    loc_screen = glGetUniformLocation(fullScreenShader, "screen");
    loc_gauss = glGetUniformLocation(fullScreenShader, "gauss");
    loc_neon = glGetUniformLocation(fullScreenShader, "neon");

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
    engine->play2D(RAIN_1, true);
    
    skybox = new SkyBox();
    cube = new Cube(shaderProgram);

    
    hm = new Terrain(shaderProgram, SIMPLE_HEIGHT_MAP, 10);
    tr = new Terrain(shaderProgram, 2000, 1600, 10);
//    tr = new Terrain(terrainProgram, 2000, 1600, 10);
    tr->update();
    
    // Generate the terrain textures
//    glGenTextures(1, &grassTex);
//    glBindTexture(GL_TEXTURE_2D, grassTex);
//    int grassW, grassH;
//    unsigned char* grass_img = TextureHandler::loadPPM(GRASS, grassH, grassW);
//    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, grassW, grassH, 0, GL_RGBA, GL_UNSIGNED_BYTE, grass_img);
//    // Filtering
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
//    hm = new Terrain(shaderProgram, SIMPLE_HEIGHT_MAP, 10);

    //Create the water FBO
    waterFBO = new FBO();
    //Create the screen FBO;
    screenFBO = new FBO();
    
    glUseProgram(waterProgram);
    water = new Water(waterProgram);

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
    trn = glm::translate(trn, glm::vec3(-550.0f, -150.0f, -110.0f));
    trn = glm::rotate(trn, glm::pi<float>()/180.0f * 90, glm::vec3(1.0, 0, 0));
    
    hm_mat = glm::translate(hm_mat, glm::vec3(-250.0f, 200.0f, -110.0f));
    hm_mat = glm::rotate(hm_mat, glm::pi<float>()/180.0f * 90, glm::vec3(1.0, 0, 0));
//    trn = glm::translate(trn, glm::vec3(-550.0f, -550.0f, -110.0f));
   
    //For water
    water_m = glm::scale(water_m, glm::vec3(3.0f, 3.0f, 3.0f));
    water_m = glm::translate(water_m, glm::vec3(.0f, .0f, .0f));
    
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
    generator->update(delta_time, 200);
    
    thunder_timer = (int) clock() / CLOCKS_PER_SEC;
    
    if (thunder_timer % 11 == 0 && thunder_timer != 0 && !thunder){
        float which_one = (rand() / (double)RAND_MAX) ;
//        int which_one = (rand() / 4);
        cout << which_one << endl;
        
//        engine->play2D((char*) thunders[which_one]);
        if (which_one > 0.25 && which_one <= 0.50){
            engine->play2D(THUNDER_1);
            cout<<"playing first sample" <<endl;
        }
        else if (which_one <= 0.25){
            engine->play2D(THUNDER_2);
            cout<<"playing 2nd sample" <<endl;
        }
        else if (which_one > 0.75){
            engine->play2D(THUNDER_3);
            cout << "playing 3rd sample" <<endl;
        } else {
            engine->play2D(THUNDER_4);
            cout << "playing 4th sample" <<endl;
        }
        thunder = true;
        cout << "hurrah" << endl;
        sampledAt = thunder_timer;
    }
    if (thunder_timer > sampledAt)
        thunder = false;
    
    

    
//    if (delta_time % )
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
    float invertPitch = -pitch;
    
    cam_look_at.x = glm::cos(invertPitch)*glm::cos(yaw);
    cam_look_at.y = glm::sin(invertPitch);
    cam_look_at.z = glm::cos(invertPitch)*glm::sin(yaw);
}

void Window::drawSkybox(){
    // Skybox
    glUseProgram(skyShaderProgram);
    glEnable(GL_CULL_FACE);
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

    generator->draw(glm::mat4(1.0f));
    
    //Toggle showing particles drawn
    if(parts)
        std::cout << "Particles drawn this frame: " << parts_drawn << "\n";
    
    glUseProgram(shaderProgram);
//    glUniform1i(loc_fields, 9);
//    glUniform1i(glGetUniformLocation(shaderProgram, "texturize"), true);
////    glUseProgram(terrainProgram);
//    glEnable(GL_TEXTURE_2D);
//    glBindVertexArray(tr->getVAO());
//    glActiveTexture(GL_TEXTURE9);
//    glUniform1i(glGetUniformLocation(shaderProgram, "terrain"), 9);
////    glUniform1i(glGetUniformLocation(terrainProgram, "terrain"), 9);
//    glBindTexture(GL_TEXTURE_2D, fieldsTex);
//    hm->draw(trn);
    if (!hmap)
        tr->draw(trn);
    else
        hm->draw(hm_mat);
//    glBindVertexArray(0);
//    glBindTexture(GL_TEXTURE_2D, 0);
    
    glUniform1i(glGetUniformLocation(shaderProgram, "texturize"), false);
    cube->draw(glm::mat4(1.0f));
}
void Window::drawReflection(){
    //Render everything above water (reflection)
    glUniform4f(clipPlaneN, 0.0f,0.0f,1.0f,0.1f);
    
    //gotta move camera down 2*distance_to_water
    float distance = -2*cam_pos.z;
    cam_pos.z += distance; //-Z IS UP / +Z IS DOWN
    invertPitch();
    
    waterFBO->bind(waterFBO->getReflectionFBO());
    
    //Clear colors
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //Draw things here <<<<<<<<
    drawObjects();

    //Move camera back
    invertPitch();
    cam_pos.z -= distance;
}
void Window::drawRefraction(){
    glUniform4f(clipPlaneN, 0.0f, 0.0f, -1.0f, 0.0f);
    waterFBO->bind(waterFBO->getRefractionFBO());
    //Clear colors
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //Draw things here <<<<<<<<
    drawObjects();
}

void Window::drawAll(){
    glUniform4f(clipPlaneN, 0.0f, 0.0f,0.0f, 0.0f);
    glUseProgram(waterProgram);
    glUniform4f(clipPlaneW, 0.0f, 0.0f, 0.0f, 0.0f);
//    waterFBO->unbind();
    
    //Draw things here <<<<<<<<
    drawObjects();
}

void Window::drawWater(){
    glUseProgram(waterProgram);
    
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
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glDisable(GL_CULL_FACE);
    water->draw(water_m);
    
    glDisable(GL_BLEND);
}

void Window::drawScene(){
    tr_counter += 1;
    
    //Get time (seconds) (start)
    timer = clock();
    
    glUseProgram(waterProgram);
    //Move the waves
    move_factor += (WAVE_SPEED*delta_time/16.0f);
    move_factor = fmod(move_factor, 1.0f);
    glUniform1f(loc_move_factor, move_factor);
    
    // Clear the color and depth buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // Set the matrix mode to GL_MODELVIEW
    drawSkybox();
    
    glUseProgram(shaderProgram);

    if(!pause_key && !hmap)
        tr->update();
    //Draw reflecions
    drawReflection();
    
    //Render everything below water (refraction)
    drawRefraction();
    
    //Render everything
    screenFBO->bind(screenFBO->getReflectionFBO());
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    drawAll();
    //Draw the water
    drawWater();
}

void Window::display_callback(GLFWwindow * window)
{
    
    drawScene();
    
    screenFBO->unbind();

    glUseProgram(fullScreenShader);
    glBindVertexArray(screen->getVAO());
    glUniform1i(loc_screen, 10); //bind screen to unit 10
    glEnableVertexAttribArray(0);
    glEnable(GL_TEXTURE_2D);
    glActiveTexture(GL_TEXTURE10);
    glBindTexture(GL_TEXTURE_2D, screenFBO->getReflTex());
    
    glUniform1i(loc_gauss, gauss_on);
    glUniform1i(loc_neon, neon_on);
    
    screen->draw();
    
    // Gets events, including input such as keyboard and mouse or window resizinh
    glfwPollEvents();
    // Swap buffers
    glfwSwapBuffers(window);
    
    //Get time (seconds) end
    timer = clock() - timer;
    delta_time = (float)timer/CLOCKS_PER_SEC;
    
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

int buttonPush = 0;

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
        if (key == GLFW_KEY_Y && mods == 0){
            trn = glm::translate(trn, glm::vec3(0.0f,0.0f,-5.0f));
            std::cout << glm::to_string(trn) << " " << buttonPush++ << std::endl;
        }
        if (key == GLFW_KEY_Y && mods == GLFW_MOD_SHIFT){
            trn = glm::translate(trn, glm::vec3(0.0f,0.0f,5.0f));
            std::cout << glm::to_string(trn) << " " << buttonPush++ << std::endl;
        }

        if (key == GLFW_KEY_H){
            hmap = !hmap;
        }
        if (mods == GLFW_MOD_SHIFT){
            if (key == GLFW_KEY_EQUAL){
                tr->updateOctaves(0.5);
                std::cout << "doing something up" << std::endl;
            }
            if (key == GLFW_KEY_MINUS){
                tr->updateOctaves(-0.5);
                std::cout << "doing something down" << std::endl;
            }
        }
        
        if (key == GLFW_KEY_L){
            parts = !parts;
        }
        if (key == GLFW_KEY_G){
            gauss_on = !gauss_on;
        }
        if (key == GLFW_KEY_N){
            neon_on = !neon_on;
        }
//=======
//        
//        if (key == GLFW_KEY_R){
//            
//        }
//        
//>>>>>>> 14ad8e24514fc53849d8b10095427afa12500312
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
