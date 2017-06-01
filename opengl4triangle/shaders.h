///		***
///
///				shaders.h - declaration of the shaders class, variables and methods - Tom
///
///		***
#pragma
#ifndef SHADERS_H
#define SHADERS_H
#include <cstdlib>
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <algorithm>

#include "include\GL\glew.h"
#include "include\GLFW\glfw3.h"
#include "include\glm\gtc\matrix_transform.hpp"
#include "Character.h"
#include "Level.h"
#include "Enemy.h"
#include "ppf.h"
#include "Item.h"

using namespace std;

class shaders{
public:
	GLuint loadShaders(const char * vertex_file_path,const char * fragment_file_path);
	shaders();
	void sendToShaders();

	void useShader(GLuint shaderID, glm::mat4 ModView, glm::mat4 Camera, glm::mat4 Proj, 
		int type, bool caught, glm::vec3 lightPos, glm::vec3 lightDir, 
		Enemy *z, Item* i);//level & zombs & items
	void useShader(GLuint shaderID, glm::mat4 Camera, glm::mat4 Proj); //debug
	void setBoneLocation(GLuint shaderID, Enemy *z);

	ppf* print;
};
#endif