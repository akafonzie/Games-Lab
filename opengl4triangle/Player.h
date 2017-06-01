//************************************************//
//   Implemented by Alice                         //
//   Class for the player                         //
//   Including methods for movement               //
//												  //
//   Oculus methods implemented by Tom            //
//************************************************//

#ifndef Player_H
#define	Player_H

#include "Character.h"
#include "oculus.h"
#include "Collision.h"
#include "Level.h"


const float boxdimp1 = 2.0; //y
const float boxdimp2 = 2.0; //x
const float boxdimp3 = 2.0; //z

class Player : public Character{
public:
	Player();
	Player(model *model);
	Player(model *model, const oculus* oc);
	void Move();
	void keyboardMove();
	void gamepadMove();
	void CollisionMove();
	void checkCollision(Level* l);
	glm::vec3 multiplyPoint(glm::vec3 c);

	void update(float dt);
	void start( oculus* oc);
	void updateTorch();
	void changeTorchDir(float f);
	glm::vec3 torchDir, torchPos;
	float m_Speed;
	OVR::Vector3f m_Position;
	OVR::Vector3f m_Velocity;
	const oculus* m_Rift;

	bool collision;
	glm::vec3 collisionVec;
	Collision* coll;

	bool WIN, LOSE;
};


#endif