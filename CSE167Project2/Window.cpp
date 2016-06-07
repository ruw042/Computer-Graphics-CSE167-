#include "window.h"
#include "glm/gtx/string_cast.hpp"
#include <unistd.h>
#include <random>

#include "vkgllib.h"

#include "PlantGenerator.hpp"
#include "LSystem.hpp"

const char* window_title = "GLFW Starter Project";
GLfloat size;
int mode = 0;
int lightType = 1;
int movement = 0;
glm::vec3 lastpoint(0.0f,0.0f,0.0f);
double xpos, ypos;
Cube * cube;
Skybox * box;

OBJObject * toDisplay;
OBJObject * bunny;
OBJObject * bear;
OBJObject * dragon;
OBJObject * cylinder;
OBJObject * pod;
OBJObject * lichKing;



// Plant stuff

MatrixTransform * plant;

int grammar_choice = 1;
int z_angle = 20;
int y_angle = 120;
float f_length = 2.5f;


std::vector<glm::vec3> plantLocations;



//texture data
struct VertexTerrain {
    GLfloat px, py, pz;
    GLfloat nx, ny, nz;
    GLfloat s, t;
};

float m_heightmap[MAP_X][MAP_Z];
std::vector<VertexTerrain> terrainData;
std::vector<GLuint> terrainIndices;
GLuint VBO, VAO, EBO;
GLint terrainProgram;
GLuint terrainTexture;





GLint shaderProgram;
GLint skyboxShader;

GLint shaderGeometryPass;
GLint shaderLightingPass;
GLint shaderSSAO;
GLint shaderSSAOBlur;


glm::vec4 light(1.f,1.f,1.f,0.f);

glm::vec4 pl_dir(1.f,1.f,1.f,0.f);
GLfloat pl_scale = 3.0f;

glm::vec4 spot_dir(1.f,1.f,1.f,0.f);
GLfloat spot_scale = 3.0f;
float range = 0.8;
float spotExp = 1.f;

struct material {
    glm::vec4 spec;
    GLfloat shine;
    glm::vec4 mat;
    glm::vec4 amb;
} bunnyMat,bearMat,dragonMat;
struct material * currentMat = &bunnyMat;

float cam_scale = 10.0f;
// Default camera parameters
glm::vec3 cam_pos(0.0f, 0.0f, 20.0f);		// e  | Position of camera
glm::vec3 cam_look_at(0.0f, 0.0f, 0.0f);	// d  | This is where the camera looks at
glm::vec3 cam_up(0.0f, 1.0f, 0.0f);			// up | What orientation "up" is
glm::vec3 cam_pos1(0.0f, 0.5f, 0.0f);
glm::vec3 cam_look_at1(0.0f, 0.5f, 1.0f);
int cam_mode = 0;

//SSAO related
GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;

GLuint gBuffer;
GLuint gPositionDepth, gNormal, gAlbedo;

GLuint attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };

GLuint rboDepth;

GLuint ssaoFBO, ssaoBlurFBO;
GLuint ssaoColorBuffer, ssaoColorBufferBlur;

std::uniform_real_distribution<GLfloat> randomFloats(0.0, 1.0); // generates random floats between 0.0 and 1.0
std::default_random_engine generator;
std::vector<glm::vec3> ssaoKernel;
std::vector<glm::vec3> ssaoNoise;
GLuint noiseTexture;

glm::vec3 lightPos = glm::vec3(0.0, 100, 0.0);
glm::vec3 lightColor = glm::vec3(1.0, 1.0, 1.0);


GLuint quadVAO = 0;
GLuint quadVBO;
void RenderQuad()
{
    if (quadVAO == 0)
    {
        GLfloat quadVertices[] = {
            // Positions        // Texture Coords
            -1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
            1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
            1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        };
        // Setup plane VAO
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    }
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}
//

//shadow map related
// Options
GLboolean shadows = true;

// Global variables

// Configure depth map FBO
const GLuint SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
GLuint depthMapFBO;

// - Create depth texture
GLuint depthMap;

GLint simpleDepthShader;
GLint shadowMapShader;

GLuint planeVAO;
GLuint planeVBO;
GLfloat planeVertices[] = {
    // Positions          // Normals         // Texture Coords
    25.0f, -0.5f, 25.0f, 0.0f, 1.0f, 0.0f, 25.0f, 0.0f,
    -25.0f, -0.5f, -25.0f, 0.0f, 1.0f, 0.0f, 0.0f, 25.0f,
    -25.0f, -0.5f, 25.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
    
    25.0f, -0.5f, 25.0f, 0.0f, 1.0f, 0.0f, 25.0f, 0.0f,
    25.0f, -0.5f, -25.0f, 0.0f, 1.0f, 0.0f, 25.0f, 25.0f,
    - 25.0f, -0.5f, -25.0f, 0.0f, 1.0f, 0.0f, 0.0f, 25.0f
};

GLuint cubeVAO = 0;
GLuint cubeVBO = 0;
void RenderCube()
{
    // Initialize (if necessary)
    if (cubeVAO == 0)
    {
        GLfloat vertices[] = {
            // Back face
            -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, // Bottom-left
            0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f, // top-right
            0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, // bottom-right
            0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,  // top-right
            -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,  // bottom-left
            -0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f,// top-left
            // Front face
            -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom-left
            0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,  // bottom-right
            0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,  // top-right
            0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, // top-right
            -0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,  // top-left
            -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,  // bottom-left
            // Left face
            -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // top-right
            -0.5f, 0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // top-left
            -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,  // bottom-left
            -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-left
            -0.5f, -0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f,  // bottom-right
            -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // top-right
            // Right face
            0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // top-left
            0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-right
            0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // top-right
            0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,  // bottom-right
            0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,  // top-left
            0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, // bottom-left
            // Bottom face
            -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, // top-right
            0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f, // top-left
            0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,// bottom-left
            0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, // bottom-left
            -0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, // bottom-right
            -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, // top-right
            // Top face
            -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,// top-left
            0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // bottom-right
            0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, // top-right
            0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // bottom-right
            -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,// top-left
            -0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f // bottom-left
        };
        glGenVertexArrays(1, &cubeVAO);
        glGenBuffers(1, &cubeVBO);
        // Fill buffer
        glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        // Link vertex attributes
        glBindVertexArray(cubeVAO);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }
    // Render Cube
    glBindVertexArray(cubeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
}



int Window::width;
int Window::height;

glm::mat4 Window::P;
glm::mat4 Window::V;

int initialized = 0;
int effectmode = 0;
glm::vec3 savedLight;



//texture methods
void Window::InitializeTerrain()
{
    // loop through all of the heightfield points, calculating
    // the coordinates for each point
    for (int z = 0; z < MAP_Z; z++)
    {
        for (int x = 0; x < MAP_X; x++)
        {
            
            terrain[x][z][0] = float(x)*MAP_SCALE - 1000.f;
            terrain[x][z][1] = (float)m_heightmap[x][z] - 200.f;
            terrain[x][z][2] = -float(z)*MAP_SCALE + 10000.f;
            
        }
    }
}
// referenced http://gamedev.stackexchange.com/questions/37389/diamond-square-terrain-generation-problem
//for diamond square algorithm

void DiamondSquare(unsigned x1, unsigned y1, unsigned x2, unsigned y2, float range, unsigned level) {
    if (level < 1) return;
    
    // diamonds
    for (unsigned i = x1 + level; i < x2; i += level)
        for (unsigned j = y1 + level; j < y2; j += level) {
            float a = m_heightmap[i - level][j - level];
            float b = m_heightmap[i][j - level];
            float c = m_heightmap[i - level][j];
            float d = m_heightmap[i][j];
            //            if (rand()%2 == 0)
            m_heightmap[i - level / 2][j - level / 2] = (a + b + c + d) / 4 + rand()%20 * range;
            //            else
            //                m_heightmap[i - level / 2][j - level / 2] = (a + b + c + d) / 4 + rand()%100 * range;
        }
    
    // squares
    for (unsigned i = x1 + 2 * level; i < x2; i += level)
        for (unsigned j = y1 + 2 * level; j < y2; j += level) {
            float a = m_heightmap[i - level][j - level];
            float b = m_heightmap[i][j - level];
            float c = m_heightmap[i - level][j];
            // float d = m_heightmap[i][j];
            float e = m_heightmap[i - level / 2][j - level / 2];
            //            if (rand()%2 == 0)
            //            {
            m_heightmap[i - level][j - level / 2] = (a + c + e + m_heightmap[i - 3 * level / 2][j - level / 2]) / 4 + rand()%20 * range;
            m_heightmap[i - level / 2][j - level] = (a + b + e + m_heightmap[i - level / 2][j - 3 * level / 2]) / 4 + rand()%20 * range;
            //            }
            //            else
            //            {
            //                m_heightmap[i - level][j - level / 2] = (a + c + e + m_heightmap[i - 3 * level / 2][j - level / 2]) / 4 - rand()%100 * range;
            //                m_heightmap[i - level / 2][j - level] = (a + b + e + m_heightmap[i - level / 2][j - 3 * level / 2]) / 4 - rand()%100 * range;
            //            }
        }
    
    DiamondSquare(x1, y1, x2, y2, range / 2, level / 2);
}
void Window::redrawTerrain()
{
    for (int i = 0; i < MAP_X; i++)
        for (int j = 0; j < MAP_Z; j++)
            m_heightmap[i][j] = 0;
    
    DiamondSquare(0, 0, 256, 256, 200, 256);
    InitializeTerrain();
    
    terrainData.clear();
    
    cam_pos.y = 15.0f+ terrain[(int)ceil(((cam_pos.x + 1000.f) / MAP_SCALE))][(int)ceil(((cam_pos.z - 10000.f) / (-1.f*MAP_SCALE)))][1];
    cam_look_at.y = 15.0f+ terrain[(int)ceil(((cam_pos.x + 1000.f) / MAP_SCALE))][(int)ceil(((cam_pos.z - 10000.f) / (-1.f*MAP_SCALE)))][1];
    glm::vec3 vector = glm::normalize(glm::vec3(cam_pos - cam_look_at));
    cam_pos = glm::vec3(cam_pos - vector);
    cam_look_at = glm::vec3(cam_look_at - vector);
    V = glm::lookAt(cam_pos, cam_look_at, cam_up);
    
    for (int z = 0; z < MAP_Z-1; z++)
    {
        for (int x = 0; x < MAP_X-1; x++)
        {
            // for each vertex, we calculate the grayscale shade color,
            // we set the texture coordinate, and we draw the vertex.
            
            VertexTerrain vertex1;
            vertex1.px = terrain[x][z][0];
            vertex1.py = terrain[x][z][1];
            vertex1.pz = terrain[x][z][2];
            vertex1.nx = terrain[x][z][1]/255.0f;
            vertex1.ny = terrain[x][z][1]/255.0f;
            vertex1.nz = terrain[x][z][1]/255.0f;
            vertex1.s = 0.0f;
            vertex1.t = 0.0f;
            terrainData.push_back(vertex1);
            
            VertexTerrain vertex2;
            vertex2.px = terrain[x+1][z][0];
            vertex2.py = terrain[x+1][z][1];
            vertex2.pz = terrain[x+1][z][2];
            vertex2.nx = terrain[x+1][z][1]/255.0f;
            vertex2.ny = terrain[x+1][z][1]/255.0f;
            vertex2.nz = terrain[x+1][z][1]/255.0f;
            vertex2.s = 1.0f;
            vertex2.t = 0.0f;
            terrainData.push_back(vertex2);
            
            VertexTerrain vertex3;
            vertex3.px = terrain[x][z+1][0];
            vertex3.py = terrain[x][z+1][1];
            vertex3.pz = terrain[x][z+1][2];
            vertex3.nx = terrain[x][z+1][1]/255.0f;
            vertex3.ny = terrain[x][z+1][1]/255.0f;
            vertex3.nz = terrain[x][z+1][1]/255.0f;
            vertex3.s = 0.0f;
            vertex3.t = 1.0f;
            terrainData.push_back(vertex3);
            
            VertexTerrain vertex4;
            vertex4.px = terrain[x+1][z+1][0];
            vertex4.py = terrain[x+1][z+1][1];
            vertex4.pz = terrain[x+1][z+1][2];
            vertex4.nx = terrain[x+1][z+1][1]/255.0f;
            vertex4.ny = terrain[x+1][z+1][1]/255.0f;
            vertex4.nz = terrain[x+1][z+1][1]/255.0f;
            vertex4.s = 1.0f;
            vertex4.t = 1.0f;
            terrainData.push_back(vertex4);
            
        }
    }
    terrainVBO();
    
}
void Window::terrainVBO()
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    //    glGenBuffers(1, &EBO);
    
    // Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
    glBindVertexArray(VAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(VertexTerrain) * terrainData.size(), &terrainData[0], GL_STATIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, terrainIndices.size() * sizeof(GLuint), &terrainIndices[0], GL_STATIC_DRAW);
    
    //vertex positions
    
    glVertexAttribPointer(0,// This first parameter x should be the same as the number passed into the line "layout (location = x)" in the vertex shader. In this case, it's 0. Valid values are 0 to GL_MAX_UNIFORM_LOCATIONS.
                          3, // This second line tells us how any components there are per vertex. In this case, it's 3 (we have an x, y, and z component)
                          GL_FLOAT, // What type these components are
                          GL_FALSE, // GL_TRUE means the values should be normalized. GL_FALSE means they shouldn't
                          8 * sizeof(GLfloat),//sizeof(Vertex),//3 * sizeof(GLfloat), // Offset between consecutive vertex attributes. Since each of our vertices have 3 floats, they should have the size of 3 floats in between
                          (GLvoid*)0); // Offset of the first vertex's component. In our case it's 0 since we don't pad the vertices array with anything.
    glEnableVertexAttribArray(0);
    //vertex normals
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat),
                          (GLvoid*) (3 * sizeof(GLfloat)));
    
    //vertex textures
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat),
                          (GLvoid*) (6 * sizeof(GLfloat)));
    
    
    glBindBuffer(GL_ARRAY_BUFFER, 0); // Note that this is allowed, the call to glVertexAttribPointer registered VBO as the currently bound vertex buffer object so afterwards we can safely unbind
    
    glBindVertexArray(0); // Unbind VAO (it's always a good thing to unbind any buffer/array to prevent strange bugs), remember: do NOT unbind the EBO, keep it bound to this VAO
    
    
}











GLfloat lerp(GLfloat a, GLfloat b, GLfloat f)
{
    return a + f * (b - a);
}



MatrixTransform *Window::make_plant() {
    if (grammar_choice == 1) {
        std::vector<std::string> producers1 = *new std::vector<std::string>();
        producers1.push_back("F");
        
        std::vector<std::string> rules1 = *new std::vector<std::string>();
        //    rules.push_back("FF-[-F+F+F]+[+F-F-F]");
        //    rules.push_back("1FF-[2-F+F+F]+[3+F-F-F]");
        //    rules.push_back("1FF-[2-F+F]+[3+F-F]"); // good!!
        rules1.push_back("1F-[2-F+F]+[3+F-F]");
        std::string start = "FF";
        int levels = 4;
        
        Grammar gram = {producers1, rules1, start, f_length, levels, z_angle, y_angle};
        plant = generatePlant(gram, cylinder);

        
    } else if (grammar_choice == 2) {
        std::vector<std::string> producers1 = *new std::vector<std::string>();
        producers1.push_back("X");
        producers1.push_back("F");
        
        std::vector<std::string> rules1 = *new std::vector<std::string>();
        //    rules.push_back("FF-[-F+F+F]+[+F-F-F]");
        //    rules.push_back("1FF-[2-F+F+F]+[3+F-F-F]");
        //    rules.push_back("1FF-[2-F+F]+[3+F-F]"); // good!!
        rules1.push_back("1F-[3[X]+4X]+2F[4+FX]-X");
        rules1.push_back("FF");
        
        std::string start = "X";
        int levels = 5;
        
        Grammar gram = {producers1, rules1, start, f_length, levels, z_angle, y_angle};
        plant = generatePlant(gram, cylinder);

    } else if (grammar_choice == 3) {
        std::vector<std::string> producers1 = *new std::vector<std::string>();
        producers1.push_back("F");
        
        std::vector<std::string> rules1 = *new std::vector<std::string>();
        
        rules1.push_back("1FF-[2-F+F+F]+[3+F-F-F]");
        
        std::string start = "F";
        int levels = 4;
        
        Grammar gram = {producers1, rules1, start, f_length, levels, z_angle, y_angle};
        plant = generatePlant(gram, cylinder);

    } else if (grammar_choice == 4) {

        return make_same_group();
    }
    return plant;
}

MatrixTransform * Window::make_same_group() {
    
    
    
    MatrixTransform * group = new MatrixTransform();
    
    std::string start = "F";
    
    for (int i = 0; i < 3; i++) {
        
        std::vector<std::string> producers1 = *new std::vector<std::string>();
        
        producers1.push_back("F");
        
        
        
        std::vector<std::string> rules1 = *new std::vector<std::string>();
        
        rules1.push_back(" 1F [ + 2F ] F [ - 2F ] F");
        
        
        
        int levels = 4;
        
        
        
        Grammar gram = {producers1, rules1, start, f_length, levels, z_angle, 20};
        
        MatrixTransform *ind_plant = generatePlant(gram, cylinder);
        
        ind_plant->translate(glm::vec3(20*i, 0,0));
        
        group->addChild(ind_plant);
        
        
        
        start += "F";
        
    }
    
    
    
    return group;
    
}

MatrixTransform * Window::make_forest() {
    MatrixTransform *forest = new MatrixTransform();
    for (std::vector<glm::vec3>::iterator it = plantLocations.begin() ; it != plantLocations.end(); ++it) {
        MatrixTransform *ind_plant = make_plant();
        glm::vec3 t = *it;
        float xval = t.x*20;
        float zval = t.z*20;
        float yval = 15.0f + terrain[(int)floor(((cam_pos.x + 1000.f) / MAP_SCALE))][(int)floor(((cam_pos.z - 10000.f) / (-1.f*MAP_SCALE)))][1];
        ind_plant->translate(glm::vec3(xval,yval,zval));
        forest->addChild(ind_plant);
    }
    
    forest->translate(glm::vec3(0,-1,0));
    return forest;
}

//MatrixTransform




void Window::initialize_objects()
{
    //texture init
    //referenced http://www.codeproject.com/Articles/14154/OpenGL-Terrain-Generation-An-Introduction for terrain generation
    DiamondSquare(0, 0, 256, 256, 200, 256);
    imageData = LoadBitmapFile("/Users/katia/proj3CSE167/proj3CSE167/terrain2.bmp", &bitmapInfoHeader);
    
    // initialize the terrain data and load the textures
    InitializeTerrain();
    LoadTextures();
    int i = 0;
    for (int z = 0; z < MAP_Z-1; z++)
    {
        for (int x = 0; x < MAP_X-1; x++)
        {
            // for each vertex, we calculate the grayscale shade color,
            // we set the texture coordinate, and we draw the vertex.
            /*
             the vertices are drawn in this order:
             
             0  ---> 1
			          /
             /
             |/
             2  ---> 3
             */
            
            
            terrainIndices.push_back(i);
            terrainIndices.push_back(i + 1);
            terrainIndices.push_back(i + 2);
            terrainIndices.push_back(i + 3);
            
            VertexTerrain vertex1;
            vertex1.px = terrain[x][z][0];
            vertex1.py = terrain[x][z][1];
            vertex1.pz = terrain[x][z][2];
            vertex1.nx = terrain[x][z][1]/255.0f;
            vertex1.ny = terrain[x][z][1]/255.0f;
            vertex1.nz = terrain[x][z][1]/255.0f;
            vertex1.s = 0.0f;
            vertex1.t = 0.0f;
            terrainData.push_back(vertex1);
            
            VertexTerrain vertex2;
            vertex2.px = terrain[x+1][z][0];
            vertex2.py = terrain[x+1][z][1];
            vertex2.pz = terrain[x+1][z][2];
            vertex2.nx = terrain[x+1][z][1]/255.0f;
            vertex2.ny = terrain[x+1][z][1]/255.0f;
            vertex2.nz = terrain[x+1][z][1]/255.0f;
            vertex2.s = 1.0f;
            vertex2.t = 0.0f;
            terrainData.push_back(vertex2);
            
            VertexTerrain vertex3;
            vertex3.px = terrain[x][z+1][0];
            vertex3.py = terrain[x][z+1][1];
            vertex3.pz = terrain[x][z+1][2];
            vertex3.nx = terrain[x][z+1][1]/255.0f;
            vertex3.ny = terrain[x][z+1][1]/255.0f;
            vertex3.nz = terrain[x][z+1][1]/255.0f;
            vertex3.s = 0.0f;
            vertex3.t = 1.0f;
            terrainData.push_back(vertex3);
            
            VertexTerrain vertex4;
            vertex4.px = terrain[x+1][z+1][0];
            vertex4.py = terrain[x+1][z+1][1];
            vertex4.pz = terrain[x+1][z+1][2];
            vertex4.nx = terrain[x+1][z+1][1]/255.0f;
            vertex4.ny = terrain[x+1][z+1][1]/255.0f;
            vertex4.nz = terrain[x+1][z+1][1]/255.0f;
            vertex4.s = 1.0f;
            vertex4.t = 1.0f;
            terrainData.push_back(vertex4);
            i = i + 4;
        }
    }
    cam_pos.y = 15.0f+ terrain[(int)ceil(((cam_pos.x + 1000.f) / MAP_SCALE))][(int)ceil(((cam_pos.z - 10000.f) / (-1.f*MAP_SCALE)))][1];
    cam_look_at.y = 15.0f+ terrain[(int)ceil(((cam_pos.x + 1000.f) / MAP_SCALE))][(int)ceil(((cam_pos.z - 10000.f) / (-1.f*MAP_SCALE)))][1];
    glm::vec3 vector = glm::normalize(glm::vec3(cam_pos - cam_look_at));
    cam_pos = glm::vec3(cam_pos - vector);
    cam_look_at = glm::vec3(cam_look_at - vector);
    V = glm::lookAt(cam_pos, cam_look_at, cam_up);
    
    terrainVBO();
    
    
    
    
    initialized = 1;
    
    
	cube = new Cube();
    box = new Skybox();
    bunny = new OBJObject("bunny.obj");
    cylinder = new OBJObject("cylinder.obj");
    pod = new OBJObject("pod.obj");
    //bear = new OBJObject("bear.obj");
    lichKing = new OBJObject("Lich_King.obj");
    //dragon = new OBJObject("dragon.obj");
    //bear = bunny;
    //dragon = bunny;
    toDisplay = bunny;
    
    
    
    plantLocations.push_back(glm::vec3(-1,0,0));
    plantLocations.push_back(glm::vec3(-1,0,-1));
    plantLocations.push_back(glm::vec3(-1,0,-2));
    plantLocations.push_back(glm::vec3(-1,0,-3));
    plantLocations.push_back(glm::vec3(0,0,-3));
    plantLocations.push_back(glm::vec3(1,0,-3));
    plantLocations.push_back(glm::vec3(2,0,-3));
    plantLocations.push_back(glm::vec3(3,0,-3));
    plantLocations.push_back(glm::vec3(3,0,-2));
    plantLocations.push_back(glm::vec3(3,0,-1));
    plantLocations.push_back(glm::vec3(3,0,0));
    plantLocations.push_back(glm::vec3(2,0,0));
    plantLocations.push_back(glm::vec3(1,0,0));
    plantLocations.push_back(glm::vec3(1,0,-1));
    // bunny coordinate @
    // plantLocations.push_back(glm::vec3(2*20,0,-1*20));
    
    plant = make_plant();
    plant = make_forest();
    //    plant = new MatrixTransform(glm::mat4());
//    Geode *g = new Geode(cylinder);
//    dtest->addChild(g);
////    dtest->draw();
    
    bunnyMat.spec = glm::vec4(0.628281f,0.555802f,0.366065f,1.f);
    bunnyMat.shine = 51.2;
    bunnyMat.mat = glm::vec4(0.75164f,0.60648f,0.22648f,1.f);
    bunnyMat.amb = glm::vec4(0.24725f,0.1995f,0.0745f,1.f);
    
    bearMat.spec = glm::vec4(0.727811f,0.626959f,0.626959f,0.55f);
    bearMat.shine = 76.8;
    bearMat.mat = glm::vec4(0.61424f,0.04136f,0.04136f,0.55f);
    bearMat.amb = glm::vec4(0.1745f,0.01175f,0.01175f,0.55f);
    
    dragonMat.spec = glm::vec4(0.316228f,0.316228f,0.316228f,0.95f);
    dragonMat.shine = 12.8;
    dragonMat.mat = glm::vec4(0.54f,0.89f,0.63f,0.95f);
    dragonMat.amb = glm::vec4(0.135f,0.2225f,0.1575f,0.95f);

	// Load the shader program. Similar to the .obj objects, different platforms expect a different directory for files
#ifdef _WIN32 // Windows (both 32 and 64 bit versions)
	shaderProgram = LoadShaders("../shader.vert", "../shader.frag");
#else // Not windows
	shaderProgram = LoadShaders("plant_shader.vert", "plant_shader.frag");
    skyboxShader = LoadShaders("skyboxShader.vert", "skyboxShader.frag");
    //shaderProgram = LoadShaders("skyboxShader.vert", "skyboxShader.frag");
    shaderGeometryPass = LoadShaders("ssao_geometry.vert", "ssao_geometry.frag");
    shaderLightingPass = LoadShaders("ssao.vert", "ssao_lighting.frag");
    shaderSSAO = LoadShaders("ssao.vert", "ssao.frag");
    shaderSSAOBlur = LoadShaders("ssao.vert", "ssao_blur.frag");
    simpleDepthShader = LoadShaders("simpleDepthShader.vert","simpleDepthShader.frag");
    shadowMapShader = LoadShaders("shadowMapShader.vert","shadowMapShader.frag");
    terrainProgram = LoadShaders("terrain.vert", "terrain.frag");
#endif
    
    //ssao related
    glUseProgram(shaderLightingPass);
    glUniform1i(glGetUniformLocation(shaderLightingPass, "gPositionDepth"), 0);
    glUniform1i(glGetUniformLocation(shaderLightingPass, "gNormal"), 1);
    glUniform1i(glGetUniformLocation(shaderLightingPass, "gAlbedo"), 2);
    glUniform1i(glGetUniformLocation(shaderLightingPass, "ssao"), 3);
    glUseProgram(shaderSSAO);
    glUniform1i(glGetUniformLocation(shaderSSAO, "gPositionDepth"), 0);
    glUniform1i(glGetUniformLocation(shaderSSAO, "gNormal"), 1);
    glUniform1i(glGetUniformLocation(shaderSSAO, "texNoise"), 2);
    
    glGenFramebuffers(1, &gBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
    
    // - Position + linear depth color buffer
    glGenTextures(1, &gPositionDepth);
    glBindTexture(GL_TEXTURE_2D, gPositionDepth);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGB, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPositionDepth, 0);
    // - Normal color buffer
    glGenTextures(1, &gNormal);
    glBindTexture(GL_TEXTURE_2D, gNormal);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);
    // - Albedo color buffer
    glGenTextures(1, &gAlbedo);
    glBindTexture(GL_TEXTURE_2D, gAlbedo);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gAlbedo, 0);
    // - Tell OpenGL which color attachments we'll use (of this framebuffer) for rendering
    
    glDrawBuffers(3, attachments);
    
    glGenRenderbuffers(1, &rboDepth);
    glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
    // - Finally check if framebuffer is complete
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "GBuffer Framebuffer not complete!" << std::endl;
    
    glGenFramebuffers(1, &ssaoFBO);  glGenFramebuffers(1, &ssaoBlurFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, ssaoFBO);
    
    // - SSAO color buffer
    glGenTextures(1, &ssaoColorBuffer);
    glBindTexture(GL_TEXTURE_2D, ssaoColorBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RGB, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssaoColorBuffer, 0);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "SSAO Framebuffer not complete!" << std::endl;
    
    // - and blur stage
    glBindFramebuffer(GL_FRAMEBUFFER, ssaoBlurFBO);
    glGenTextures(1, &ssaoColorBufferBlur);
    glBindTexture(GL_TEXTURE_2D, ssaoColorBufferBlur);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RGB, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssaoColorBufferBlur, 0);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "SSAO Blur Framebuffer not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
    // Sample kernel
    for (GLuint i = 0; i < 64; ++i)
    {
        glm::vec3 sample(randomFloats(generator) * 2.0 - 1.0, randomFloats(generator) * 2.0 - 1.0, randomFloats(generator));
        sample = glm::normalize(sample);
        sample *= randomFloats(generator);
        GLfloat scale = GLfloat(i) / 64.0;
        
        // Scale samples s.t. they're more aligned to center of kernel
        scale = lerp(0.1f, 1.0f, scale * scale);
        sample *= scale;
        ssaoKernel.push_back(sample);
    }
    
    // Noise texture
    
    for (GLuint i = 0; i < 16; i++)
    {
        glm::vec3 noise(randomFloats(generator) * 2.0 - 1.0, randomFloats(generator) * 2.0 - 1.0, 0.0f); // rotate around z-axis (in tangent space)
        ssaoNoise.push_back(noise);
    }
    
    glGenTextures(1, &noiseTexture);
    glBindTexture(GL_TEXTURE_2D, noiseTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, 4, 4, 0, GL_RGB, GL_FLOAT, &ssaoNoise[0]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    
    //
    
    //shadow map related
    glUseProgram(shadowMapShader);
    glUniform1i(glGetUniformLocation(shadowMapShader, "shadowMap"), 0);
    
    glGenFramebuffers(1, &depthMapFBO);
    
    glGenTextures(1, &depthMap);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    GLfloat borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
    glClearColor(1.f, 0.0f, 0.0f, 1.0f);
    
    glm::mat4 model;
    float xval = 40.0f;
    float zval = -20.0f;
    float yval = 25 + terrain[(int)floor(((cam_pos.x + 1000.f) / MAP_SCALE))][(int)floor(((cam_pos.z - 10000.f) / (-1.f*MAP_SCALE)))][1];
    
    model = glm::translate(glm::mat4(), glm::vec3(xval,yval,zval));
    model = glm::scale(model,glm::vec3(10,10,10));
    glUseProgram(shaderGeometryPass);
    glUniformMatrix4fv(glGetUniformLocation(shaderGeometryPass, "model"), 1, GL_FALSE, &model[0][0]);
    
    
    lightPos = glm::vec3(xval-15,yval+5,zval);
    savedLight = lightPos;
    //lightPos = glm::vec3(-2,4,-1);
    
    
    glGenVertexArrays(1, &planeVAO);
    glGenBuffers(1, &planeVBO);
    glBindVertexArray(planeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), &planeVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
    glBindVertexArray(0);

    


}

void Window::clean_up()
{
	delete(cube);
    delete(bunny);
    delete(box);
    //delete(bear);
    //delete(dragon);
	glDeleteProgram(shaderProgram);
    glDeleteProgram(skyboxShader);
}

GLFWwindow* Window::create_window(int width, int height)
{
	// Initialize GLFW
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		return NULL;
	}

	// 4x antialiasing
	glfwWindowHint(GLFW_SAMPLES, 4);
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	// Create the GLFW window
	GLFWwindow* window = glfwCreateWindow(width, height, window_title, NULL, NULL);

	// Check if the window could not be created
	if (!window)
	{
		fprintf(stderr, "Failed to open GLFW window.\n");
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

void Window::resize_callback(GLFWwindow* window, int width, int height)
{
	Window::width = width;
	Window::height = height;
	// Set the viewport size
	glViewport(0, 0, width, height);

	if (height > 0)
	{
		P = glm::perspective(45.0f, (float)width / (float)height, 0.1f, 1000.0f);
		V = glm::lookAt(cam_pos, cam_look_at, cam_up);
	}
    
    if(initialized == 1)
    {
    //ssao related
    glUseProgram(shaderLightingPass);
    glUniform1i(glGetUniformLocation(shaderLightingPass, "gPositionDepth"), 0);
    glUniform1i(glGetUniformLocation(shaderLightingPass, "gNormal"), 1);
    glUniform1i(glGetUniformLocation(shaderLightingPass, "gAlbedo"), 2);
    glUniform1i(glGetUniformLocation(shaderLightingPass, "ssao"), 3);
    glUseProgram(shaderSSAO);
    glUniform1i(glGetUniformLocation(shaderSSAO, "gPositionDepth"), 0);
    glUniform1i(glGetUniformLocation(shaderSSAO, "gNormal"), 1);
    glUniform1i(glGetUniformLocation(shaderSSAO, "texNoise"), 2);
    
    glGenFramebuffers(1, &gBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
    
    // - Position + linear depth color buffer
    glGenTextures(1, &gPositionDepth);
    glBindTexture(GL_TEXTURE_2D, gPositionDepth);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGB, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPositionDepth, 0);
    // - Normal color buffer
    glGenTextures(1, &gNormal);
    glBindTexture(GL_TEXTURE_2D, gNormal);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);
    // - Albedo color buffer
    glGenTextures(1, &gAlbedo);
    glBindTexture(GL_TEXTURE_2D, gAlbedo);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gAlbedo, 0);
    // - Tell OpenGL which color attachments we'll use (of this framebuffer) for rendering
    
    glDrawBuffers(3, attachments);
    
    glGenRenderbuffers(1, &rboDepth);
    glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
    // - Finally check if framebuffer is complete
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "GBuffer Framebuffer not complete!" << std::endl;
    
    glGenFramebuffers(1, &ssaoFBO);  glGenFramebuffers(1, &ssaoBlurFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, ssaoFBO);
    
    // - SSAO color buffer
    glGenTextures(1, &ssaoColorBuffer);
    glBindTexture(GL_TEXTURE_2D, ssaoColorBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RGB, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssaoColorBuffer, 0);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "SSAO Framebuffer not complete!" << std::endl;
    
    // - and blur stage
    glBindFramebuffer(GL_FRAMEBUFFER, ssaoBlurFBO);
    glGenTextures(1, &ssaoColorBufferBlur);
    glBindTexture(GL_TEXTURE_2D, ssaoColorBufferBlur);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RGB, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssaoColorBufferBlur, 0);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "SSAO Blur Framebuffer not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
    // Sample kernel
    for (GLuint i = 0; i < 64; ++i)
    {
        glm::vec3 sample(randomFloats(generator) * 2.0 - 1.0, randomFloats(generator) * 2.0 - 1.0, randomFloats(generator));
        sample = glm::normalize(sample);
        sample *= randomFloats(generator);
        GLfloat scale = GLfloat(i) / 64.0;
        
        // Scale samples s.t. they're more aligned to center of kernel
        scale = lerp(0.1f, 1.0f, scale * scale);
        sample *= scale;
        ssaoKernel.push_back(sample);
    }
    
    // Noise texture
    
    for (GLuint i = 0; i < 16; i++)
    {
        glm::vec3 noise(randomFloats(generator) * 2.0 - 1.0, randomFloats(generator) * 2.0 - 1.0, 0.0f); // rotate around z-axis (in tangent space)
        ssaoNoise.push_back(noise);
    }
    
    glGenTextures(1, &noiseTexture);
    glBindTexture(GL_TEXTURE_2D, noiseTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, 4, 4, 0, GL_RGB, GL_FLOAT, &ssaoNoise[0]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    }

}

void Window::idle_callback()
{
	// Call the update function the cube
	//cube->update();
    //toDisplay->update();
   
    //mt_root->update(glm::rotate(glm::mat4(1.0f), .0000001f, glm::vec3(0.0f, 1.0f, 0.0f)));
    //mt_arm111->update(glm::rotate(glm::mat4(1.0f), .001f, glm::vec3(0.0f, 1.0f, 0.0f)));
   
    plant->update(glm::mat4());
}

void Window::display_callback(GLFWwindow* window)
{
    glClearColor(0.f, 0.f, 1.f, 1.0f);
    // Clear the color and depth buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    
    // Use the shader of programID
    //glUseProgram(skyboxShader);
    

    if(effectmode == 0)
    {

    //SSAO
    GLfloat currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;
    
    // 1. Geometry Pass: render scene's geometry/color data into gbuffer
    glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //glUseProgram(skyboxShader);
    //box->draw(skyboxShader);
    glm::mat4 projection = P;
    glm::mat4 view = V;
    glm::mat4 model;
    glUseProgram(shaderGeometryPass);
    glUniformMatrix4fv(glGetUniformLocation(shaderGeometryPass, "projection"), 1, GL_FALSE, &projection[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(shaderGeometryPass, "view"), 1, GL_FALSE, &view[0][0]);
       
    //glUniform1i(glGetUniformLocation(shaderGeometryPass,"skyVert"),0);
    bunny->draw(shaderGeometryPass); // SSAO BUNNY
    //plant->draw();
    
    
    //RenderCube();
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
    // 2. Create SSAO texture
    glBindFramebuffer(GL_FRAMEBUFFER, ssaoFBO);
    glClear(GL_COLOR_BUFFER_BIT);
    glUseProgram(shaderSSAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gPositionDepth);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, gNormal);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, noiseTexture);
    // Send kernel + rotation
    for (GLuint i = 0; i < 64; ++i)
        glUniform3fv(glGetUniformLocation(shaderSSAO, ("samples[" + std::to_string(i) + "]").c_str()), 1, &ssaoKernel[i][0]);
    glUniformMatrix4fv(glGetUniformLocation(shaderSSAO, "projection"), 1, GL_FALSE, &projection[0][0]);
    RenderQuad();
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
    
    // 3. Blur SSAO texture to remove noise
    glBindFramebuffer(GL_FRAMEBUFFER, ssaoBlurFBO);
    glClear(GL_COLOR_BUFFER_BIT);
    glUseProgram(shaderSSAOBlur);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, ssaoColorBuffer);
    RenderQuad();
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
    
    // 4. Lighting Pass: traditional deferred Blinn-Phong lighting now with added screen-space ambient occlusion
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(shaderLightingPass);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gPositionDepth);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, gNormal);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, gAlbedo);
    glActiveTexture(GL_TEXTURE3); // Add extra SSAO texture to lighting pass
    glBindTexture(GL_TEXTURE_2D, ssaoColorBufferBlur);
    // Also send light relevant uniforms
    glm::vec3 lightPosView = glm::vec3(view * glm::vec4(lightPos, 1.0));
    glUniform3fv(glGetUniformLocation(shaderLightingPass, "light.Position"), 1, &lightPosView[0]);
    glUniform3fv(glGetUniformLocation(shaderLightingPass, "light.Color"), 1, &lightColor[0]);
    // Update attenuation parameters
    const GLfloat constant = 1.0; // Note that we don't send this to the shader, we assume it is always 1.0 (in our case)
    const GLfloat linear = 0.09;
    const GLfloat quadratic = 0.032;
    glUniform1f(glGetUniformLocation(shaderLightingPass, "light.Linear"), linear);
    glUniform1f(glGetUniformLocation(shaderLightingPass, "light.Quadratic"), quadratic);
    glUniform1i(glGetUniformLocation(shaderLightingPass, "draw_mode"), 1);
    RenderQuad();
    
    glBindFramebuffer(GL_READ_FRAMEBUFFER, gBuffer);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); // Write to default framebuffer
    // blit to default framebuffer. Note that this may or may not work as the internal formats of both the FBO and default framebuffer have to match.
    // the internal formats are implementation defined. This works on all of my systems, but if it doesn't on yours you'll likely have to write to the
    // depth buffer in another shader stage (or somehow see to match the default framebuffer's internal format with the FBO's internal format).
    glBlitFramebuffer(0, 0, width, width, 0, 0, width, width, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
    
    glUseProgram(skyboxShader);
    box->draw(skyboxShader);
    
        glUseProgram(shaderProgram);
        plant->draw();
    //glUseProgram(shaderProgram);
    //bunny->draw(shaderProgram); // non ssao bunny
    //dtest->draw();

    
    
    
    glUseProgram(terrainProgram);
    //glShadeModel(GL_SMOOTH);					   // use smooth shading
    //glEnable(GL_DEPTH_TEST);					   // hidden surface removal
    //    glEnable(GL_CULL_FACE);						   // do not calculate inside of poly's
    glFrontFace(GL_CCW);						      // counter clock-wise polygons are out
    
    //glEnable(GL_TEXTURE_2D);					   // enable 2D texturing
    glm::mat4 MVP = Window::P * Window::V * glm::mat4(1.0f);
    // We need to calculate this because as of GLSL version 1.40 (OpenGL 3.1, released March 2009), gl_ModelViewProjectionMatrix has been
    // removed from the language. The user is expected to supply this matrix to the shader when using modern OpenGL.
    GLuint MatrixID = glGetUniformLocation(terrainProgram, "MVP");
    glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, land);
    glUniform1i(glGetUniformLocation(terrainProgram, "gSampler"), 0);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, second);
    glUniform1i(glGetUniformLocation(terrainProgram, "secondSampler"), 1);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, third);
    glUniform1i(glGetUniformLocation(terrainProgram, "thirdSampler"), 2);
    
    
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 260100);
    glBindVertexArray(0);
    
    // enable blending
    //glEnable(GL_BLEND);
    
    // enable read-only depth buffer
    //glDepthMask(GL_FALSE);
    
    // set the blend function to what we use for transparency
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    
    // set back to normal depth buffer mode (writable)
    //glDepthMask(GL_TRUE);
    
    // disable blending
    //glDisable(GL_BLEND);

    }
    else{
    
    
    
    
    
    //shadow map
    GLfloat currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;
    
    // 1. Render depth of scene to texture (from light's perspective)
    // - Get light projection/view matrix.
    glm::mat4 lightProjection, lightView;
    glm::mat4 lightSpaceMatrix;
    GLfloat near_plane = 1.0f, far_plane = 7.5f;
    //lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
    lightProjection = glm::perspective(45.0f, (GLfloat)SHADOW_WIDTH / (GLfloat)SHADOW_HEIGHT, near_plane, far_plane); // Note that if you use a perspective projection matrix you'll have to change the light position as the current light position isn't enough to reflect the whole scene.
    lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
    lightSpaceMatrix = lightProjection * lightView;
    // - now render scene from light's point of view
    glUseProgram(simpleDepthShader);
    glUniformMatrix4fv(glGetUniformLocation(simpleDepthShader, "lightSpaceMatrix"), 1, GL_FALSE, &lightSpaceMatrix[0][0]);
    glm::mat4 model = glm::mat4(1.f);
    glUniformMatrix4fv(glGetUniformLocation(simpleDepthShader, "model"), 1, GL_FALSE, &model[0][0]);
    glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glClear(GL_DEPTH_BUFFER_BIT);
        glBindVertexArray(planeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);
            model = glm::mat4(1.f);
        glUniformMatrix4fv(glGetUniformLocation(simpleDepthShader, "model"), 1, GL_FALSE, &model[0][0]);
        // Cubes
        model = glm::mat4();
        model = glm::translate(model, glm::vec3(0.0f, 1.5f, 0.0));
        glUniformMatrix4fv(glGetUniformLocation(simpleDepthShader, "model"), 1, GL_FALSE, &model[0][0]);
        RenderCube();
        model = glm::mat4();
        model = glm::translate(model, glm::vec3(2.0f, 0.0f, 1.0));
        glUniformMatrix4fv(glGetUniformLocation(simpleDepthShader, "model"), 1, GL_FALSE, &model[0][0]);
        RenderCube();
        model = glm::mat4();
        model = glm::translate(model, glm::vec3(-1.0f, 0.0f, 2.0));
        model = glm::rotate(model, 60.0f, glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
        model = glm::scale(model, glm::vec3(0.5));
        glUniformMatrix4fv(glGetUniformLocation(simpleDepthShader, "model"), 1, GL_FALSE, &model[0][0]);
        //RenderCube();
        bunny->draw(simpleDepthShader);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
    // 2. Render scene as normal
    glViewport(0, 0, width, height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(skyboxShader);
    box->draw(skyboxShader);
    glUseProgram(shadowMapShader);
    glm::mat4 projection = P;
    glm::mat4 view = V;
    glUniformMatrix4fv(glGetUniformLocation(shadowMapShader, "projection"), 1, GL_FALSE, &projection[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(shadowMapShader, "view"), 1, GL_FALSE, &view[0][0]);
    model = glm::mat4(1.f);
    glUniformMatrix4fv(glGetUniformLocation(shadowMapShader, "model"), 1, GL_FALSE, &model[0][0]);

    // Set light uniforms
    glUniform3fv(glGetUniformLocation(shadowMapShader, "lightPos"), 1, &lightPos[0]);
    glUniform3fv(glGetUniformLocation(shadowMapShader, "viewPos"), 1, &cam_pos[0]);
    glUniformMatrix4fv(glGetUniformLocation(shadowMapShader, "lightSpaceMatrix"), 1, GL_FALSE, &lightSpaceMatrix[0][0]);
    // Enable/Disable shadows by pressing 'SPACE'
    glUniform1i(glGetUniformLocation(shadowMapShader, "shadows"), shadows);
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, depthMap);
        
        glBindVertexArray(planeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);
        model = glm::mat4(1.f);
        glUniformMatrix4fv(glGetUniformLocation(shadowMapShader, "model"), 1, GL_FALSE, &model[0][0]);
        // Cubes
        model = glm::mat4();
        model = glm::translate(model, glm::vec3(0.0f, 1.5f, 0.0));
        glUniformMatrix4fv(glGetUniformLocation(shadowMapShader, "model"), 1, GL_FALSE, &model[0][0]);
        RenderCube();
        model = glm::mat4();
        model = glm::translate(model, glm::vec3(2.0f, 0.0f, 1.0));
        glUniformMatrix4fv(glGetUniformLocation(shadowMapShader, "model"), 1, GL_FALSE, &model[0][0]);
        RenderCube();
        model = glm::mat4();
        model = glm::translate(model, glm::vec3(-1.0f, 0.0f, 2.0));
        model = glm::rotate(model, 60.0f, glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
        model = glm::scale(model, glm::vec3(0.5));
        glUniformMatrix4fv(glGetUniformLocation(shadowMapShader, "model"), 1, GL_FALSE, &model[0][0]);
        //RenderCube();
        bunny->draw(shadowMapShader);

    
    
    
    /*
    glUseProgram(shaderProgram);
	// Render the cube
	//toDisplay->draw(shaderProgram);
    
    
    bunny->draw(shaderProgram);*/
   
    //sleep(1000);
    }

	// Gets events, including input such as keyboard and mouse or window resizing
	glfwPollEvents();
	// Swap buffers
	glfwSwapBuffers(window);
}

glm::vec3 Window::trackBallMapping(float xpos,float ypos)
{
    glm::vec3 v;
    float d;
    v.x = (2.0*xpos - Window::width) / Window::width;
    v.y = (Window::height - 2.0*ypos) / Window::height;
    v.z = 0.0;
    d = glm::length(v);
    d = (d<1.0) ? d : 1.0;
    v.z = sqrtf(1.001 - d*d);
    v = glm::normalize(v);
    //v.Normalize(); // Still need to normalize, since we only capped d, not v.
    return v;
}
void Window::cursor_pos_callback(GLFWwindow* window, double x, double y)
{
    glm::vec3 direction;
    float rot_angle;
    glm::vec3 curPoint;
    if(mode ==0)
    {
        switch (movement)
        {
            
            case 1 : // Left-mouse button is being held down
            {
                curPoint = trackBallMapping((float)(2.0*x),(float)(2.0*y)); // Map the mouse position to a logical
                // sphere location.
                direction = curPoint - lastpoint;
                float velocity = direction.length();
                if( velocity > 0.0001 ) // If little movement - do nothing.
                {
                    glm::vec3 rotAxis;
                    rotAxis = glm::normalize(glm::cross( lastpoint, curPoint ));
                    rot_angle = velocity*0.04;
                    //toDisplay->move(rot_angle,rotAxis);
                    if(rotAxis.length() >0)
                    {
                        cam_pos = glm::vec3((glm::rotate(glm::mat4(1.0f), rot_angle, rotAxis))*glm::vec4(cam_pos,1.0f));
                        V = glm::lookAt(cam_pos, cam_look_at, cam_up);
                    }
                }
                break;
            }
            case 2:
            {
                toDisplay->move((xpos-x)*0.01,(ypos-y)*0.01);
                xpos = x;
                ypos = y;
                break;
            }
        }
        lastpoint = curPoint;
    }
    else
    {
        if(movement == 2)
        {
           if(lightType ==3)
           {
               if((y-ypos)>0)
                   range = range/1.1;
               else if((y-ypos)<0)
                   range = range*1.1;
               if((x-xpos)>0)
                   spotExp = spotExp/1.1;
               else if((x-xpos)<0)
                   spotExp = spotExp*1.1;

               xpos = x;
               ypos = y;
           }
        }
        else
        {
        if(lightType == 1)
        {
            lastpoint = trackBallMapping(x,y);
            light = glm::vec4(lastpoint,0.f);
        }
        else if(lightType ==2)
        {
            lastpoint = trackBallMapping(x,y);
            pl_dir = glm::vec4(lastpoint,0.f);
        }
        else if(lightType ==3)
        {
            lastpoint = trackBallMapping(x,y);
            spot_dir = glm::vec4(lastpoint,0.f);
        }
        }
    }
}
void Window::mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if(mode ==0)
    {
        if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
        {
        
            glfwGetCursorPos(window, &xpos, &ypos);
            movement = 1;
            lastpoint = trackBallMapping((float)(2.0*xpos),(float)(2.0*ypos));
        }
        if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
        {
        
            movement = 0;
        }
        if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
        {
            glfwGetCursorPos(window, &xpos, &ypos);
            movement = 2;
        
        }
        if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE)
        {
        
            movement = 0;
        }
    }
    else
    {
        if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
        {
            glfwGetCursorPos(window, &xpos, &ypos);
            movement = 2;
            
        }
        if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE)
        {
            
            movement = 0;
        }

    }
    
    
}

void Window::scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    if(mode == 0)
    {
        if(yoffset >0)
        {
            cam_scale = 1.2*cam_scale;
            cam_pos = cam_pos*(1.2f);
            V = glm::lookAt(cam_pos, cam_look_at, cam_up);
            //toDisplay->move('Z');
        }
        else
        {
            cam_scale = cam_scale/1.2;
            cam_pos = cam_pos/(1.2f);
            V = glm::lookAt(cam_pos, cam_look_at, cam_up);
            //toDisplay->move('z');
        }
    }
    else
    {
        if(lightType ==2)
        {
            if(yoffset >0)
                pl_scale = 1.1*pl_scale;
            else
                pl_scale = pl_scale/1.1;
        }
        if(lightType ==3)
        {
            if(yoffset >0)
                spot_scale = 1.2*pl_scale;
            else
                spot_scale = pl_scale/1.2;
        }
    }
}
void Window::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    // Check for a key press
    if (action == GLFW_PRESS || action == GLFW_REPEAT)
    {
        // Check if escape was pressed
        if (key == GLFW_KEY_ESCAPE)
        {
            // Close the window. This causes the program to also terminate.
            glfwSetWindowShouldClose(window, GL_TRUE);
        }
        
        else if (key == 'B') //redraw terrain
        {
            redrawTerrain();
        }
        else if (key == 'Z') //zoom out
        {
            cam_pos.y = 15.0f + cam_pos.y ;
            V = glm::lookAt(cam_pos, cam_look_at, cam_up);
        }

        else if (key == 'W') //move forward
        {
            cam_pos.y = 15.0f+ terrain[(int)ceil(((cam_pos.x + 1000.f) / MAP_SCALE))][(int)ceil(((cam_pos.z - 10000.f) / (-1.f*MAP_SCALE)))][1];
            cam_look_at.y = 15.0f+ terrain[(int)ceil(((cam_pos.x + 1000.f) / MAP_SCALE))][(int)ceil(((cam_pos.z - 10000.f) / (-1.f*MAP_SCALE)))][1];
            glm::vec3 vector = glm::normalize(glm::vec3(cam_pos - cam_look_at)) * 4.0f;
            cam_pos = glm::vec3(cam_pos - vector);
            cam_look_at = glm::vec3(cam_look_at - vector);
            V = glm::lookAt(cam_pos, cam_look_at, cam_up);
        }
        else if (key == 'D') //move right
        {
            
            cam_pos.y = 15.0f+ terrain[(int)ceil(((cam_pos.x + 1000.f) / MAP_SCALE))][(int)ceil(((cam_pos.z - 10000.f) / (-1.f*MAP_SCALE)))][1];
            cam_look_at.y = 15.0f + terrain[(int)ceil(((cam_pos.x + 1000.f) / MAP_SCALE))][(int)ceil(((cam_pos.z - 10000.f) / (-1.f*MAP_SCALE)))][1];
            glm::vec3 vector = glm::normalize(glm::vec3(cam_pos - cam_look_at));
            vector = glm::rotate(vector, 90.0f/180.0f * glm::pi<float>(), glm::vec3(0.0f, 1.0f, 0.f));
            cam_pos = glm::vec3(cam_pos + vector);
            cam_look_at = glm::vec3(cam_look_at + vector);
            
            V = glm::lookAt(cam_pos, cam_look_at, cam_up);
        }
        else if (key == 'S') //move backwards
        {
            cam_pos.y = 15.0f+ terrain[(int)ceil(((cam_pos.x + 1000.f) / MAP_SCALE))][(int)ceil(((cam_pos.z - 10000.f) / (-1.f*MAP_SCALE)))][1];
            cam_look_at.y = 15.0f+ terrain[(int)ceil(((cam_pos.x + 1000.f) / MAP_SCALE))][(int)ceil(((cam_pos.z - 10000.f) / (-1.f*MAP_SCALE)))][1];
            glm::vec3 vector = glm::normalize(glm::vec3(cam_pos - cam_look_at)) * 4.0f;
            cam_pos = glm::vec3(cam_pos + vector);
            cam_look_at = glm::vec3(cam_look_at + vector);
            V = glm::lookAt(cam_pos, cam_look_at, cam_up);
        }
        else if (key == 'A') //move left
        {
            cam_pos.y = 15.0f+ terrain[(int)ceil(((cam_pos.x + 1000.f) / MAP_SCALE))][(int)ceil(((cam_pos.z - 10000.f) / (-1.f*MAP_SCALE)))][1];
            cam_look_at.y = 15.0f+ terrain[(int)ceil(((cam_pos.x + 1000.f) / MAP_SCALE))][(int)ceil(((cam_pos.z - 10000.f) / (-1.f*MAP_SCALE)))][1];
            
            glm::vec3 vector = glm::normalize(glm::vec3(cam_pos - cam_look_at));
            vector = glm::rotate(vector, -90.0f/180.0f * glm::pi<float>(), glm::vec3(0.0f, 1.0f, 0.f));
            cam_pos = glm::vec3(cam_pos + vector);
            cam_look_at = glm::vec3(cam_look_at + vector);
            
            V = glm::lookAt(cam_pos, cam_look_at, cam_up);
        }
        else if (key == 'Q')//rotate left
        {
            glm::vec3 rotAxis = glm::vec3(0.0f, 1.0f, 0.0f);
            float  rot_angle = 5.f;
            if (rotAxis.length() > 0)
            {
//                cam_pos = glm::vec3(glm::rotate(glm::mat4(1.f), rot_angle/180.0f * glm::pi<float>(), rotAxis)*glm::vec4(cam_pos,1.f));
                cam_look_at = glm::vec3(glm::rotate(glm::mat4(1.f), rot_angle/180.0f * glm::pi<float>(), rotAxis)*glm::vec4(cam_look_at,1.f));
                //cam_pos.y = 15.0f+ terrain[(int)ceil(((cam_pos.x + 1000.f) / MAP_SCALE))][(int)ceil(((cam_pos.z - 10000.f) / (-1.f*MAP_SCALE)))][1];
                cam_look_at.y =  terrain[(int)ceil(((cam_pos.x + 1000.f) / MAP_SCALE))][(int)ceil(((cam_pos.z - 10000.f) / (-1.f*MAP_SCALE)))][1];
                V = glm::lookAt(cam_pos, cam_look_at, cam_up);
            }
        }
        else if (key == 'E')//rotate right
        {
            glm::vec3 rotAxis = glm::vec3(0.0f, 1.0f, 0.0f);
            float  rot_angle = -5.f;
            if (rotAxis.length() > 0)
            {
//                cam_pos = glm::vec3(glm::rotate(glm::mat4(), rot_angle/180.0f * glm::pi<float>(), rotAxis)*glm::vec4(cam_pos,1.f));
                cam_look_at = glm::vec3(glm::rotate(glm::mat4(1.f), rot_angle/180.0f * glm::pi<float>(), rotAxis)*glm::vec4(cam_look_at,1.f));
                //cam_pos.y = 15.0f+ terrain[(int)ceil(((cam_pos.x + 1000.f) / MAP_SCALE))][(int)ceil(((cam_pos.z - 10000.f) / (-1.f*MAP_SCALE)))][1];
                cam_look_at.y =  terrain[(int)ceil(((cam_pos.x + 1000.f) / MAP_SCALE))][(int)ceil(((cam_pos.z - 10000.f) / (-1.f*MAP_SCALE)))][1];
                
                V = glm::lookAt(cam_pos, cam_look_at, cam_up);
            }
        } else if (key == GLFW_KEY_1) {
            grammar_choice = 1;
            plant = make_plant();
            float xval = 0.0f*20;
            float zval = 0.0f*20;
            float yval = 30.0f + terrain[(int)floor(((cam_pos.x + 1000.f) / MAP_SCALE))][(int)floor(((cam_pos.z - 10000.f) / (-1.f*MAP_SCALE)))][1];
            plant->translate(glm::vec3(xval,yval,zval));
        } else if (key == GLFW_KEY_2) {
            grammar_choice = 2;
            plant = make_plant();
            float xval = 0.0f*20;
            float zval = 0.0f*20;
            float yval = 15.0f + terrain[(int)floor(((cam_pos.x + 1000.f) / MAP_SCALE))][(int)floor(((cam_pos.z - 10000.f) / (-1.f*MAP_SCALE)))][1];
            plant->translate(glm::vec3(xval,yval,zval));
        } else if (key == GLFW_KEY_3) {
            grammar_choice = 3;
            plant = make_plant();
            float xval = 0.0f*20;
            float zval = 0.0f*20;
            float yval = 15.0f + terrain[(int)floor(((cam_pos.x + 1000.f) / MAP_SCALE))][(int)floor(((cam_pos.z - 10000.f) / (-1.f*MAP_SCALE)))][1];
            plant->translate(glm::vec3(xval,yval,zval));
            
        } else if (key == GLFW_KEY_4) {
            grammar_choice = 4;
            plant = make_plant();
            float xval = 0.0f*20;
            float zval = 0.0f*20;
            float yval = 15.0f + terrain[(int)floor(((cam_pos.x + 1000.f) / MAP_SCALE))][(int)floor(((cam_pos.z - 10000.f) / (-1.f*MAP_SCALE)))][1];
            plant->translate(glm::vec3(xval,yval,zval));
        } else if (key == GLFW_KEY_EQUAL) {
            z_angle++;
            plant = make_plant();
            float xval = 0.0f*20;
            float zval = 0.0f*20;
            float yval = 15.0f + terrain[(int)floor(((cam_pos.x + 1000.f) / MAP_SCALE))][(int)floor(((cam_pos.z - 10000.f) / (-1.f*MAP_SCALE)))][1];
            plant->translate(glm::vec3(xval,yval,zval));
        } else if (key == GLFW_KEY_MINUS) {
            z_angle--;
            plant = make_plant();
            float xval = 0.0f*20;
            float zval = 0.0f*20;
            float yval = 15.0f + terrain[(int)floor(((cam_pos.x + 1000.f) / MAP_SCALE))][(int)floor(((cam_pos.z - 10000.f) / (-1.f*MAP_SCALE)))][1];
            plant->translate(glm::vec3(xval,yval,zval));
        } else if (key == GLFW_KEY_F) {
            if (mods == GLFW_MOD_SHIFT) {
                f_length++;
            } else {
                f_length--;
            }
            plant = make_plant();
            float xval = 0.0f*20;
            float zval = 0.0f*20;
            float yval = 15.0f + terrain[(int)floor(((cam_pos.x + 1000.f) / MAP_SCALE))][(int)floor(((cam_pos.z - 10000.f) / (-1.f*MAP_SCALE)))][1];
            plant->translate(glm::vec3(xval,yval,zval));
        } else if (key == GLFW_KEY_C) {
//            plant = make_same_group();
            float xval = 0.0f*20;
            float zval = 0.0f*20;
            float yval = 15.0f + terrain[(int)floor(((cam_pos.x + 1000.f) / MAP_SCALE))][(int)floor(((cam_pos.z - 10000.f) / (-1.f*MAP_SCALE)))][1];
            plant->translate(glm::vec3(xval,yval,zval));
        }
        else if (key == GLFW_KEY_SPACE){
            if(effectmode == 0)
            {
                cam_pos = glm::vec3(0,0,3.0f);
                cam_look_at = glm::vec3(0.0f, 0.0f, 0.0f);
                lightPos = glm::vec3(-2,4,-1);
                effectmode = 1;
                //display_callback(window);
            }
            else
            {
                effectmode = 0;
                cam_pos = glm::vec3(0.0f, 0.0f, 20.0f);		// e  | Position of camera
                cam_look_at = glm::vec3(0.0f, 0.0f, 0.0f);
                lightPos = savedLight;
                //display_callback(window);
                
            }
        }
        
        
        
       
        glGetFloatv(GL_POINT_SIZE, &size);
        if( mods&GLFW_MOD_SHIFT )
        {

            
//            if (key == GLFW_KEY_P)
//            {
//                glPointSize(size+1);
//            }
//            if (key == GLFW_KEY_X)
//            {
//                toDisplay->move('X');
//            }
//            if (key == GLFW_KEY_Y)
//            {
//                toDisplay->move('Y');
//            }
//            if (key == GLFW_KEY_Z)
//            {
//                toDisplay->move('Z');
//            }
//            if (key == GLFW_KEY_S)
//            {
//                toDisplay->move('S');
//            }
//            if (key == GLFW_KEY_O)
//            {
//                toDisplay->move('O');
//            }
        }
        else
        {
            if (key == GLFW_KEY_P)
            {
                glPointSize(size-1);
            }
            if (key == GLFW_KEY_X)
            {
                toDisplay->move('x');
            }
            if (key == GLFW_KEY_Y)
            {
                toDisplay->move('y');
            }
            if (key == GLFW_KEY_Z)
            {
                toDisplay->move('z');
            }
            
            if (key == GLFW_KEY_O)
            {
                toDisplay->move('o');
            }
            if (key == GLFW_KEY_R)
            {
                toDisplay->move('r');
            }
            if (key == GLFW_KEY_C)
            {
                if(cam_mode == 0)
                {
                    V = glm::lookAt(cam_pos1, cam_look_at1, cam_up);
                    cam_mode = 1;
                }
                else
                {
                    V = glm::lookAt(cam_pos, cam_look_at, cam_up);
                    cam_mode = 0;
                }
            }

        }
        
        if (key == GLFW_KEY_F1)
        {
            toDisplay = bunny;
            currentMat = &bunnyMat;
            
        }
        if (key == GLFW_KEY_F2)
        {
            toDisplay = bear;
            currentMat = &bearMat;
            
        }
        if (key == GLFW_KEY_F3)
        {
            toDisplay = dragon;
            currentMat = &dragonMat;
            
        }
        if (key == GLFW_KEY_0)
        {
            if(mode ==0)
                mode = 1;
            else
                mode = 0;
        }

        
    }
}