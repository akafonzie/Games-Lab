///		***
///
///				level.h - declaration of the Level class, variables and methods - Tom
///
///		***

#ifndef Level_H
#define Level_H
#include "modelLoader.h"
#include "include\glm\gtc\matrix_transform.hpp"

using namespace std;
struct plane{
	glm::vec3 blf, blb, brf, brb, tlf, tlb, trf, trb;
	void zero(){
		glm::vec3 z = glm::vec3(0, 0, 0);
		blf=blb=brf=brb=tlf=tlb=trf=trb = z;
	}
};

struct collPlane{
	GLuint vbo;
	plane thePlane;
	std::vector<glm::vec3> verts;
	glm::vec3 min, max, forward, right, up, centre;
	void zero(){
		min = max = forward = right = glm::vec3(0,0,0);
	}
};

class Level{
public:
	glm::mat4 MVP, ModelView;
	model* levelModel;
	Level();
	Level(model* m);
	~Level();
	void loadLevel();
	void manualConstruct();
	void makeCollPlanes();
	void renderCollPlanes();
	std::vector<collPlane> collPlanes;
	unsigned int vao;
	size_t vbp;
};
#endif
