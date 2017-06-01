//************************************************//
//   Implemented by Alice                         //
//   Base class for the zombies, player and items //
//************************************************//

#include "Character.h"

void Character::updateHeading(float f){
	heading+=f;
}

void Character::setHeading(float f){
	heading = f;
}

float Character::getHeading(){
	return heading;
}

void Character::setPitch(float f){
	pitch = f;
}

void Character::setBackward(bool b){
	this->backward = b;
}

void Character::setForward(bool b){
	this->forward = b;
}

void Character::setKeyF(bool b){
	keyforward = b;
}

void Character::setKeyB(bool b){
	keybackward = b;
}

void Character::setLeft(bool b){
	this->left = b;
}

void Character::setRight(bool b){
	this->right = b;
}

void Character::setStrafingL(bool b){
	this->sLeft = b;
}

void Character::setStrafingR(bool b){
	this->sRight = b;
}

void Character::setLookUp(bool b){
	this->lookUp = b;
}

void Character::setLookDown(bool b){
	this->lookDown = b;
}

void Character::setLookLeft(bool b){
	this->lookLeft = b;
}

void Character::setLookRight(bool b){
	this->lookRight = b;
}

void Character::setRollLeft(bool b){
	this->rollLeft = b;
}

void Character::setRollRight(bool b){
	this->rollRight = b;
}

void Character::setCentre(float x, float y, float z){
	centre.x = x;
	centre.y = y;
	centre.z = z;
}

void Character::setSpeed(float s)
{
	speed = s;
	initSpeed = speed;
	chaseSpeed = speed*4.0;
}

//updates speeds with the delta value
void Character::updateDeltaSpeed(float delta){
	deltaSpeed = initSpeed * delta;
	deltaChaseSpeed = deltaSpeed * 8.0;
	deltaTurnSpeed = initTurnSpeed * delta;
	deltaCrouchSpeed = crouchSpeed * delta;
	deltaSprintSpeed = sprintSpeed * delta;

	turnSpeed = deltaTurnSpeed;
}

void Character::setTurnSpeed(float s){
	turnSpeed = s;
}

void Character::updateVecs(){

	Quaternion *y = new Quaternion(heading, &upvec); //yaw
	y->normalise(); 
	float yawMat[16]; //the matrix representing the yaw rotation quaternion
	y->convertToMatrix(yawMat);

	//rotate from 1,0,0 by heading (yaw)
	rightvec.x = 1.0; rightvec.y = 0.0; rightvec.z = 0.0;
	glm::vec3 *rightrot = &rightvec; //rotate the right vector
	float vecright[3] = {0.0f,0.0f,0.0f};
	y->rotateVector(yawMat,rightrot,vecright);
	rightvec.x = vecright[0]; rightvec.y = vecright[1]; rightvec.z = vecright[2];

	//rotate from 0,0,1 by heading (yaw)
	forwardvec.x = 0.0; forwardvec.y = 0.0; forwardvec.z = 1.0;
	glm::vec3 *forwardrot = &forwardvec; //rotate the forward vector
	float vecforward[3] = {0.0f,0.0f,0.0f};
	y->rotateVector(yawMat,forwardrot,vecforward);
	forwardvec.x = vecforward[0]; forwardvec.y = vecforward[1]; forwardvec.z = vecforward[2];

	bb.axes[0] = forwardvec;
	bb.axes[1] = rightvec;
	bb.axes[2] = glm::cross(forwardvec, rightvec);
}

void Character::updateCorners(float boxdim1, float boxdim2, float boxdim3){

	//   3_____0	
	//  /______/|
	//  2|     1|
	//  |7_____|4
	//  6______5/

	glm::vec3 xRot = glm::vec3(cos(-heading*radians), 0.0, sin(-heading*radians));
	glm::vec3 yRot = glm::vec3(0.0, 1.0, 0.0);
	glm::vec3 zRot = glm::vec3(-sin(-heading*radians), 0.0, cos(-heading*radians));

	//rotate corners around centre point
	bb.cornerPoints[0] = centre +  yRot*boxdim1  +  xRot*boxdim2  + -zRot*boxdim3; 	
	bb.cornerPoints[1] = centre +  yRot*boxdim1  + -xRot*boxdim2  + -zRot*boxdim3; 
	bb.cornerPoints[2] = centre +  yRot*boxdim1  + -xRot*boxdim2  +  zRot*boxdim3; 
	bb.cornerPoints[3] = centre +  yRot*boxdim1  +  xRot*boxdim2  +  zRot*boxdim3; 
    bb.cornerPoints[4] = centre + -yRot*boxdim1  +  xRot*boxdim2  + -zRot*boxdim3; 
	bb.cornerPoints[5] = centre + -yRot*boxdim1  + -xRot*boxdim2  + -zRot*boxdim3; 
    bb.cornerPoints[6] = centre + -yRot*boxdim1  + -xRot*boxdim2  +  zRot*boxdim3; 
	bb.cornerPoints[7] = centre + -yRot*boxdim1  +  xRot*boxdim2  +  zRot*boxdim3; 

}
