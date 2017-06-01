//************************************************//
//   Implemented by Alice                         //
//   Class for the zombies                        //
//   Including methods for their movement,        //
//   collision response and response to the player//
//************************************************//

#ifndef Enemy_H
#define Enemy_H

#include "Character.h"
#include "Player.h"
#include "Collision.h"
#include "Level.h"
#include "gamePad.h"

const float boxdimz1 = 2.0; //y
const float boxdimz2 = 6.0; //x
const float boxdimz3 = 2.5; //z

class Enemy : public Character{
public:
	Enemy();
	Enemy(model *model);
	void Move(float amount1, float amount2, Player* p, gamePad *g, float RT);
	//void Move();
	void CircuitMove(float amount1, float amount2, Player* p);
	void CollisionMove();
	void Chase(Player* p);
	void checkSight(Player* p);
	void checkCaught(Player* p, gamePad* g, float RT);
	void genLine(glm::vec3 point1, glm::vec3 point2);
	void genPoints();
	void drawLine();
	void drawPoints();
	void setUpZombie(glm::vec3 scale, glm::vec3 translate);
	void checkCollision(Character* c);
	void checkLvlCollision(Level* l);

	float Tamount;
	float angleBetween;
	float vecMagnitude;
	bool edge1;
	bool edge2;
	bool edge3;
	bool edge4;
	bool canSee;
	bool justCollided;
	unsigned int vao;
	int anim;
	float animtime;
	float rumbleStart;

	bool collision, lvlCollision;
	glm::vec3 collisionVec, lvlCollisionVec;
	glm::vec3 zombToPlayer, normalizedZTP;
	vector<Matrix_4f> Transforms;
	Collision* coll;

private:
	void increaseHeading();
	void changeHeading(float amount);
};

#endif