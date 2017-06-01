//************************************************//
//   Implemented by Alice                         //
//   Class for detecting collisions between       //
//   characters with each other and characters    //
//   with the level.                              //
//************************************************//

#ifndef Collision_H
#define	Collision_H
#define GLM_FORCE_RADIANS
#include "include\glm\gtc\matrix_transform.hpp"
#include "Quaternion.h"
#include "vec3f.h"
#include "Level.h"
#include "Character.h"

using namespace std;

struct Projection{
	float min;
	float max;
};

struct collisionResult{
	glm::vec3 MTV; //movement translation vector
	bool colliding;
};

class Collision{
public:
	Collision();
	glm::vec3 checkCollision(Character* c1, Character* c2);
	collisionResult SAT(Character* c1, Character* c2);
	Projection Project(glm::vec3 axis, Character* c1);

	glm::vec3 checkCollision(Character* c1, Level* l);
	collisionResult SAT(Character* c1, Level* l, int levelIndex);
	Projection Project(glm::vec3 axis, Level* l, int levelIndex);
};
#endif