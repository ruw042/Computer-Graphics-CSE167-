#ifndef OBJOBJECT_H
#define OBJOBJECT_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <iostream>
#include <fstream>


struct Material {
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    float shininess;
};

class OBJObject
{


public:
    std::vector<unsigned int> indices;
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec3> normals;
    glm::mat4 toWorld;
    
    GLuint VBO2,VBO, VAO, EBO;
    float angle;
    OBJObject(const char* filepath);
    OBJObject(std::vector<glm::vec3> vertices, std::vector<glm::vec3> normals,std::vector<GLuint> indices);
    ~OBJObject();

	void parse(const char* filepath);
    void parse1(const char* filepath);
    void draw(GLuint shaderProgram);
    void draw(GLuint shaderProgram, glm::mat4 M);
    //void spin(float);
    //void update();
    void move(char input);
    void move(float angle,glm::vec3 axis);
    void move(float x,float y);
    
//    glm::vec3 color;
    bool isPlant = false;
    Material material;

};

#endif