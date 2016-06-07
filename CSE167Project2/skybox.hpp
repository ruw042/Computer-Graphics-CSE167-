#ifndef skybox_hpp
#define skybox_hpp

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective


class Skybox
{
public:
    Skybox();
    ~Skybox();
    
    glm::mat4 toWorld;
    GLuint VBO, VAO, EBO;
    GLuint cubemapTexture;
    
    void draw(GLuint);
    GLuint loadTexture();
    unsigned char* loadPPM(const char* filename, int& width, int& height);
};

#endif

