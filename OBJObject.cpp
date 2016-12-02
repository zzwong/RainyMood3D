#include "OBJObject.h"
#include "Window.h"

using namespace std;

static const int RIGHT = 0;
static const int LEFT  = 1;
static const int UP    = 2;
static const int DOWN  = 3;
static const int IN    = 4;
static const int OUT   = 5;

OBJObject::OBJObject(const char *filepath) 
{
    angle = 1.0f;
	toWorld = glm::mat4(1.0f);
    translationMat = glm::mat4(1.0f);
    rotationMat = glm::mat4(1.0f);
    scaleMat = glm::mat4(1.0f);
    spinMat = glm::mat4(1.0f);
	parse(filepath);
    
    
    // create array object and buffers
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glGenBuffers(1, &NBO);
    glGenVertexArrays(1, &VAO);
    
    // Bind Vertex Array Object first, and then bind associated buffers to it.
    // VAO is a container for all other buffers
    glBindVertexArray(VAO);
    
    // Bind VBO as GL_ARRAY_BUFFER
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);
    // Enable usage of layout location 0
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(GLfloat), (GLvoid*)0);
    
    
    // Takes care of the ordering of vertices to draw
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(int), &indices[0], GL_STATIC_DRAW);
    
    
    // bind NBO as GL_ARRAY_BUFFER
    glBindBuffer(GL_ARRAY_BUFFER, NBO);
    glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3*sizeof(GLfloat), (GLvoid*)0);
    
    // unbind bound buffer to avoid making unwanted changes
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    // unbind VAO
    glBindVertexArray(0);
    
}

OBJObject::~OBJObject()
{
    // Delete previously generated buffers. Note that forgetting to do this can waste GPU memory in a
    // large project! This could crash the graphics driver due to memory leaks, or slow down application performance!
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteBuffers(1, &NBO);
}


vector<glm::vec3> OBJObject::getVertices()
{
    return  vertices;
}

vector<glm::vec3> OBJObject::getNormals()
{
    return normals;
}

glm::mat4 OBJObject::getToWorldMat()
{
    return toWorld;
}

void OBJObject::parse(const char *filepath) 
{
    // Populate the face indices, vertices, and normals vectors with the OBJ Object data
    string line;                     // string buffer for each line in file
    ifstream obj_file(filepath);     // read input file as an fstream
    float x,y,z;    // vertex coordinates
    int v1, v2, v3; // indices for corresponding vertex in each face
//    float r,g,b;  // vertex color
    float vn_x, vn_y, vn_z; // vertex normal coordinates for the triangle
    vector<string> tokens;
    
    float maxX, maxY, maxZ, minX, minY, minZ, avgX, avgY, avgZ;
    maxX = maxY = maxZ = numeric_limits<float>::min();
    minX = minY = minZ = numeric_limits<float>::max();
    
    
    cout << "Parsing the OBJ file: " << filepath << endl;
    
    if (obj_file.is_open()){
        while(getline(obj_file, line)){
            // We only care about lines starting w/ 'v' and 'f'
            if (line[0] != 'v' && line[0] != 'f') continue;
            // Vertex
            if (line[0] == 'v' && line[1] == ' '){
                tokens = split(line);
                
                x = stof(tokens[1]), y = stof(tokens[2]), z = stof(tokens[3]);
//                r = stof(tokens[4]), g = stof(tokens[5]), b = stof(tokens[6]);
                // check mins and max
                if (x < minX ) minX = x;
                if (x > maxX ) maxX = x;
                if (y < minY ) minY = y;
                if (y > maxY ) maxY = y;
                if (z < minZ ) minZ = z;
                if (z > maxZ ) maxZ = z;

                const glm::vec3 vertex(x,y,z);
//                const glm::vec3 color(r,g,b);
                vertices.push_back(vertex);
//                colors.push_back(color);                
            }
            // Vertex Normal
            else if (line[0] == 'v' && line[1] == 'n'){
                tokens = split(line);
                vn_x = stof(tokens[1]), vn_y = stof(tokens[2]), vn_z = stof(tokens[3]);
                const glm::vec3 normal(vn_x, vn_y, vn_z);
                normals.push_back(normal);
            }
            
            // Parse faces and each index into the indices so GL knows the order of drawing v's
            else if (line[0] == 'f'){
                tokens = split(line, '/');
                v1 = stoi(tokens[1])-1, v2 = stoi(tokens[2])-1, v3 = stoi(tokens[3])-1;
                indices.push_back(v1);
                indices.push_back(v2);
                indices.push_back(v3);
            }

        }
    }
    cout << "Done parsing. " << endl;
    
    // Take the midrange average of each coordinate
    avgX = 0.5 * (maxX + minX);
    avgY = 0.5 * (maxY + minY);
    avgZ = 0.5 * (maxZ + minZ);
    cout << "avgs" <<  avgX << " " << avgY << " " << avgZ << endl;
    cout << "avgs" <<  maxX << " " << maxY << " " << maxZ << endl;
    
    float biggestAxis = max({maxX-minX, maxY-minY, maxZ-minZ});

    cout << "Updating vertices..." << endl;
    // 2nd Pass, update the vertices by subtracting avg - coord val
    // Center and scales!
    for (int i = 0; i < vertices.size(); ++i){
        vertices[i].x -= avgX;
        vertices[i].y -= avgY;
        vertices[i].z -= avgZ;
        vertices[i].x /= biggestAxis;
        vertices[i].y /= biggestAxis;
        vertices[i].z /= biggestAxis;
    }
    
    cout << "Done updating vertices." <<endl;
    
}

// Helper function to split by delimiter
vector<string> OBJObject::split(string src, char delim){
    vector<string> tokens;
    string str_build = "";
    bool skip = false;
    
    for (char& c: src){
        if (c==13) tokens.push_back(str_build);
        if (c==delim) skip = true;
        if (c==' '){
            tokens.push_back(str_build);
            str_build = "";
            skip = false;
        }
        if (isdigit(c) && !skip) str_build.push_back(c);
    }
    
    return tokens;
}

vector<string> OBJObject::split(string src)
{
    vector<string> tokens;
    
    string str_build = "";
    for (char& c: src){
        // # 13 ascii == carriage return
        if (c == 13){
            tokens.push_back(str_build);
        }
        if (c == ' '){
            tokens.push_back(str_build);
            str_build = "";
        } else {
            str_build.push_back(c);
        }
    }
    
    return tokens;
}

void OBJObject::draw(GLuint shaderProgram)
{
    // Calculate the combination of the model & view (C_inv) matrices
//    toWorld = toWorld * glm::rotate(glm::mat4(1.0f), 1.0f / 180.0f * glm::pi<float>(), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 modelview = Window::V * toWorld;
    
//    cout << " HAHA " << vertices.size() << " " << indices.size() << endl;
    
    // Get location of uniform vars "Projection" and "Modelview"
    uProjection = glGetUniformLocation(shaderProgram, "projection");
    uModelview = glGetUniformLocation(shaderProgram, "modelview");
    uModel = glGetUniformLocation(shaderProgram, "model");
    // Send these values to the shader program
    glUniformMatrix4fv(uProjection, 1, GL_FALSE, &Window::P[0][0]);
    glUniformMatrix4fv(uModelview, 1, GL_FALSE, &modelview[0][0]);
    glUniformMatrix4fv(uModel, 1, GL_FALSE, &toWorld[0][0]);
    
    // Draw the Object by binding the VAO associated with it
    glBindVertexArray(VAO);
    // Tell OpenGL to draw w/ triangles, num of indices, type of indices, offset
//    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glDrawElements(GL_TRIANGLES, (int)indices.size(), GL_UNSIGNED_INT, 0);
    
    // unbind the VAO when done so we don't accidentally draw extra stuff or tamper w/ its bound buffers
    glBindVertexArray(0);
}


void OBJObject::spin(float deg)
{
    spinMat = glm::rotate(glm::mat4(1.0f), deg/ 180.0f * glm::pi<float>(), glm::vec3(0.0f, 1.0f, 0.0f));
}

void OBJObject::scale(bool up)
{
    if (up)
        scaleMat = glm::scale(glm::mat4(1.0f), glm::vec3(1.5f, 1.5f, 1.5f));
    else
        scaleMat = glm::scale(glm::mat4(1.0f), glm::vec3(0.75f, 0.75f, 0.75f));
}

void OBJObject::translate(int dir)
{
    if(dir == RIGHT){
        translationMat = glm::translate(glm::mat4(1.0f), glm::vec3(0.5f,0.0f,0.0f)) ;
    }
    else if(dir == LEFT){
        translationMat = glm::translate(glm::mat4(1.0f), glm::vec3(-0.5f,0.0f,0.0f));
    }
    else if(dir == UP){
        translationMat = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f,0.5f,0.0f));
    }
    else if(dir == DOWN){
        translationMat = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f,-0.5f,0.0f));
    }
    else if(dir == IN){
        toWorld = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f,0.0f,0.5f)) * toWorld;
    }
    else if(dir == OUT){
        toWorld = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f,0.0f,-0.5f)) * toWorld;
    }
}

void OBJObject::translateXY(double xpos, double ypos)
{
    toWorld = glm::translate(glm::mat4(1.0f), glm::vec3(xpos,0.0f,0.0f)) * toWorld;
    toWorld = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f,ypos,0.0f)) * toWorld;
}

void OBJObject::translateX(double xpos){
    toWorld = glm::translate(glm::mat4(1.0f), glm::vec3(xpos, 0.0f, 0.0f)) * toWorld;
}

void OBJObject::translateY(double ypos){
    toWorld = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, ypos, 0.0f)) * toWorld;
}


//void OBJObject::translateZ(double offset)
//{
//    
//}

void OBJObject::rotate(bool z_dir)
{
    // rotate clockwise/counter on the window's z axis
    angle += 0.0001;
    if (angle > 360.0f || angle < -360.0f) angle = 0.0f;
    // This creates the matrix to rotate the cube

    if (z_dir)
        rotationMat = glm::rotate(glm::mat4(1.0f), 30 / 180.0f * glm::pi<float>(), glm::vec3(0.0f,0.0f,-1.0f));
    else
        rotationMat = glm::rotate(glm::mat4(1.0f), 30 / 180.0f * glm::pi<float>(), glm::vec3(0.0f,0.0f,1.0f));
        
}

void OBJObject::rotate(float angle, glm::vec3 axis)
{
    toWorld = glm::rotate(glm::mat4(1.0f), angle, axis) * toWorld;
}

void OBJObject::reset()
{
    toWorld = glm::mat4(1.0f);
}

// set the transformation matrices to identity matrix
void OBJObject::clearTransformMatrices()
{
    scaleMat = translationMat = spinMat = rotationMat = glm::mat4(1.0f);
}



void OBJObject::update()
{
    // Keep spinning
    spin(1.0f);
    
    // transformation = T-1 * R * T
    glm::vec3 toOrigin(-toWorld[3]);
    glm::mat4 T = glm::translate(glm::mat4(1.0f), toOrigin);// * toWorld;
    glm::mat4 invT = glm::inverse(T);
    glm::mat4 M_spin = invT * spinMat * T;
    
    //glm::mat4 M_total = translationMat * rotationMat * spinMat * scaleMat;
    glm::mat4 M_total = translationMat * M_spin * rotationMat * scaleMat;
    toWorld = M_total * toWorld;
    
    clearTransformMatrices();
}