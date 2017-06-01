//************************************************//
//   Implemented by Alice                         //
//   Base class for the zombies, player and items //
//************************************************//

#ifndef Character_H
#define	Character_H
#include "modelLoader.h"
#define GLM_FORCE_RADIANS
#include "include\glm\gtc\matrix_transform.hpp"
#include "Quaternion.h"
#include "Vec3f.h"

using namespace std;

static const float radians = 3.14159265359f / 180.0f;
static const float degrees = 180.0f / 3.14159265359f;

//For SAT tests
struct BoundingBox{
	float boxDims[3];
	glm::vec3 cornerPoints[8];
	glm::vec3 axes[3];
};


class Character{
public:
	
	model* myModel;
	float heading, pitch;
	float speed, turnSpeed;
	float initSpeed, initTurnSpeed, chaseSpeed, crouchSpeed, sprintSpeed;
	//speeds multiplied by delta so the speed is similar on all machines
	float deltaSpeed, deltaChaseSpeed, deltaCrouchSpeed, deltaSprintSpeed, deltaTurnSpeed;
	//states from the controllers (keyboard or gamepad)
	bool forward, backward, left, right, 
		 keyforward, keybackward,
		 sprint, crouch, 
		 turnLeft, turnRight, sLeft, sRight, 
		 lookUp, lookDown, lookLeft, lookRight, 
		 rollLeft, rollRight,
		 caught, move;
	glm::vec3 centre, initcentre, forwardvec, rightvec, upvec;
	glm::mat4 ModelView;
	BoundingBox bb;
	

	void updateHeading(float f);
	void setHeading(float f);
	void setPitch(float f);
	float getHeading();
	void setForward(bool b);
	void setBackward(bool b);
	void setKeyF(bool b);
	void setKeyB(bool b );
	void setLeft(bool b);
	void setRight(bool b);
	void setStrafingL(bool b);
	void setStrafingR(bool b);
	void setLookUp(bool b);
	void setLookDown(bool b);
	void setLookLeft(bool b);
	void setLookRight(bool b);
	void setRollLeft(bool b);
	void setRollRight(bool b);
	void setSpeed(float s);
	void setTurnSpeed(float s);
	void setCentre(float x, float y, float z);
	void updateDeltaSpeed(float delta);

	//update forward and right vecs
	void updateVecs();
	void genLine(glm::vec3 point1, glm::vec3 point2);
	void drawLine();
	void updateCorners(float boxdim1, float boxdim2, float boxdim3);
};
#endif