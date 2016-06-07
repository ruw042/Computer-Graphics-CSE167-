#ifndef _WINDOW_H_
#define _WINDOW_H_

#include <iostream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "Cube.h"
#include "OBJObject.h"
#include "shader.h"
#include "skybox.hpp"
#include "MatrixTransform.hpp"
#include <glm/gtx/rotate_vector.hpp>
#include "Geode.hpp"

class Window
{
public:
	static int width;
	static int height;
	static glm::mat4 P;
	static glm::mat4 V;
    static void InitializeTerrain();
    static void redrawTerrain();
    static void terrainVBO();
	static void initialize_objects();
	static void clean_up();
	static GLFWwindow* create_window(int width, int height);
	static void resize_callback(GLFWwindow* window, int width, int height);
	static void idle_callback();
	static void display_callback(GLFWwindow*);
	static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void cursor_pos_callback(GLFWwindow* window, double xpos, double ypos);
    static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
    static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
    static glm::vec3 trackBallMapping(float xpos,float ypos);
    
    static MatrixTransform *make_plant();
    static MatrixTransform * make_forest();
    static MatrixTransform * make_same_group();
};

#endif
