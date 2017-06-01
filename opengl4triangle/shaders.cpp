///		***
///
///		shaders.cpp - implementation of the shader class - Tom & Alice
///		This class implements the loading and checking of .glsl shader files into the application (Tom). It 
///		also implements methods which are used to send the shader specific variables for use by the different
///		objects that use them (Alice); for instance, the bone animation data that the characters in the game use.
///
///		***

#include "shaders.h"	

shaders::shaders(){}

GLuint shaders::loadShaders(const char * vertex_file_path,const char * fragment_file_path){

	// Create the shaders
	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	// Read the Vertex Shader code from the file
	std::string VertexShaderCode;
	std::ifstream VertexShaderStream(vertex_file_path, std::ios::in);
	if(VertexShaderStream.is_open()){
		std::string Line = "";
		while(getline(VertexShaderStream, Line))
			VertexShaderCode += "\n" + Line;
		VertexShaderStream.close();
	}else{
		printf("Impossible to open %s. Are you in the right directory ? Don't forget to read the FAQ !\n", vertex_file_path);
		return 0;
	}

	// Read the Fragment Shader code from the file
	std::string FragmentShaderCode;
	std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);
	if(FragmentShaderStream.is_open()){
		std::string Line = "";
		while(getline(FragmentShaderStream, Line))
			FragmentShaderCode += "\n" + Line;
		FragmentShaderStream.close();
	}

	GLint Result = GL_FALSE;
	int InfoLogLength;

	// Compile Vertex Shader
	print->compilingNoEnd("Vertex Shader:",  vertex_file_path);
	char const * VertexSourcePointer = VertexShaderCode.c_str();
	glShaderSource(VertexShaderID, 1, &VertexSourcePointer , NULL);
	glCompileShader(VertexShaderID);

	// Check Vertex Shader
	glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if ( InfoLogLength > 0 ){
		std::vector<char> VertexShaderErrorMessage(InfoLogLength+1);
		glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
		printf("%s\n", &VertexShaderErrorMessage[0]);
	}
	print->loadingComp();

	// Compile Fragment Shader
	print->compilingNoEnd("Fragment Shader:",  fragment_file_path);
	char const * FragmentSourcePointer = FragmentShaderCode.c_str();
	glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer , NULL);
	glCompileShader(FragmentShaderID);

	// Check Fragment Shader
	glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if ( InfoLogLength > 0 ){
		std::vector<char> FragmentShaderErrorMessage(InfoLogLength+1);
		glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
		printf("%s\n", &FragmentShaderErrorMessage[0]);
	}
	print->loadingComp();

	// Link the program
	print->linking();
	GLuint ProgramID = glCreateProgram();
	glAttachShader(ProgramID, VertexShaderID);
	glAttachShader(ProgramID, FragmentShaderID);
	glLinkProgram(ProgramID);


	// Check the program
	glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
	glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if ( InfoLogLength > 0 ){
		std::vector<char> ProgramErrorMessage(InfoLogLength+1);
		glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
		printf("%s\n", &ProgramErrorMessage[0]);
	}
	print->linkingComp();

	glDeleteShader(VertexShaderID);
	glDeleteShader(FragmentShaderID);

	return ProgramID;
}

void shaders::useShader(GLuint shaderID, glm::mat4 Camera, glm::mat4 Proj){

		glUseProgram(shaderID);
		glUniformMatrix4fv(glGetUniformLocation(shaderID, "View"), 1, GL_FALSE, &Camera[0][0]);
		glUniformMatrix4fv(glGetUniformLocation(shaderID, "Projection"), 1, GL_FALSE, &Proj[0][0]);
}

void shaders::useShader(GLuint shaderID, glm::mat4 ModView, glm::mat4 Camera, glm::mat4 Proj, 
		int type, bool caught, glm::vec3 lightPos, glm::vec3 lightDir, 
		Enemy *z, Item* i){
		glUseProgram(shaderID);

		glUniformMatrix4fv(glGetUniformLocation(shaderID, "View"), 1, GL_FALSE, &Camera[0][0]);
		glUniformMatrix4fv(glGetUniformLocation(shaderID, "ModelView"), 1, GL_FALSE, &ModView[0][0]);
		glUniformMatrix4fv(glGetUniformLocation(shaderID, "Projection"), 1, GL_FALSE, &Proj[0][0]);

		glm::mat3 NormalMatrix = glm::mat3(ModView);
		glm::inverse(NormalMatrix);
		glm::transpose(NormalMatrix);
		
		glUniformMatrix3fv(glGetUniformLocation(shaderID, "NormalMatrix"), 1, GL_FALSE, &NormalMatrix[0][0]);
		
		//set where the light should be
		glUniform3f(glGetUniformLocation(shaderID, "lightPosWorld"),lightPos.x, lightPos.y, lightPos.z);	
		glUniform3f(glGetUniformLocation(shaderID, "spotLightDir"),lightDir.x, lightDir.y, lightDir.z);
		glUniform1i(glGetUniformLocation(shaderID, "texture0"), 0);	
		glUniform1i(glGetUniformLocation(shaderID, "type"), type);

		if(caught) glUniform1i(glGetUniformLocation(shaderID, "caught"), 1);
		else glUniform1i(glGetUniformLocation(shaderID, "caught"), 0); 

		if(type == 1)
			glUniform1i(glGetUniformLocation(shaderID, "texNorm"), 1);
		//so shader knows model type e.g. level or character
															     
		//if zombie, send the bone transformations
		if(type == 0){
			setBoneLocation(shaderID, z);
		 
				for (int i = 0 ; i < z->Transforms.size() ; i++) {
					assert(i < 100); //100 is max bones
					//already got the glGetUniformLocation earlier, 
					//from the method setBoneLocation,
					//so it knows where to send this.
					//send the current matrix to the relative position 
					//in the transformation matrices array
				
					glUniformMatrix4fv(z->myModel->boneTransforms[i], 1, 
						GL_TRUE, (const GLfloat*)z->Transforms[i].m);
				}
		}

		if(type ==2){
			if(i->found)
				glUniform1i(glGetUniformLocation(shaderID, "found"), 1); 
			else
				glUniform1i(glGetUniformLocation(shaderID, "found"), 0); 
		}



}

void shaders::setBoneLocation(GLuint shaderID, Enemy *z){
	for (unsigned int i = 0 ; i < 100 ; i++) {
        char Name[128];
        memset(Name, 0, sizeof(Name));
		//sprintf writes to the string Name, just adds the index in between the brackets
        sprintf(Name, "boneTransforms[%d]", i);
		z->myModel->boneTransforms[i] = glGetUniformLocation(shaderID, Name); 

		//this is like doing
		//glGetUniformLocation(shaderID, "gBones[0]");
		//glGetUniformLocation(shaderID, "gBones[1]");
		//glGetUniformLocation(shaderID, "gBones[2]");
		//etc.
	}

	//return bt;
}

