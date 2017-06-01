//************************************************//
//   Implemented by Alice                         //
//   Class for the camera without the Oculus Rift //
//************************************************//



#include <iostream>
#include "Camera.h"


static const float piRadians = (3.141592654f / 180.0f);



Camera::Camera(Player* p){
	eyex=p->centre.x;
	eyey=p->centre.y;
	eyez=p->centre.z;

	atx=eyex;
	aty=eyey;
	atz=eyez+1.0f;

	upx=0;
	upy=1;
	upz=0;

	up.setComponentXYZ(0,1,0);
	right.setComponentXYZ(1,0,0);
	forward.setComponentXYZ(0,0,1);
	//a vector representing the initial distance between the eye and the at points
	//adding this in its various directions to the eye will form a circumference which the at point will lie on
	focus.setComponentXYZ(atx-eyex, aty-eyey, atz-eyez); 
	focus.normalize();

	pitchangle=0;
	yawangle=0;
}

//This method is required for resetting the camera after the headMove method is no longer called,
//so that the camera returns to facing in the same direction as the player's heading
void Camera::reInitialise(Player* p){
	eyex=p->centre.x;
	eyey=p->centre.y; 
	eyez=p->centre.z;

	atx=eyex;
	aty=eyey;
	atz=eyez+1.0f;

	upx=0;
	upy=1;
	upz=0;

	up.setComponentXYZ(0,1,0);
	right.setComponentXYZ(1,0,0);
	forward.setComponentXYZ(0,0,1);

	focus.setComponentXYZ(atx-eyex, aty-eyey, atz-eyez); 
	focus.normalize();

	updateVecs(yawangle,pitchangle);

	eyex = p->centre.x;
	eyey = p->centre.y+2.0f;
	eyez = p->centre.z;
	
	atx = eyex + focus.getComponentX();	
	aty = eyey + focus.getComponentY();
	atz = eyez + focus.getComponentZ();
	
	upx=up.getComponentX();
	upy=up.getComponentY();
	upz=up.getComponentZ();

}

//this camera follows the player 
void Camera::update(Player* p){
	
	float yaw = 0.0;
	float pitch = 0.0;

	if(p->left){
		yaw=p->turnSpeed;
		if(yawangle < 360.0){
			yawangle+=p->turnSpeed;
		}
		else{ yawangle = 0.0; }
	}

	if(p->right){
		yaw=-p->turnSpeed;
		if(yawangle > 0.0){
			yawangle -= p->turnSpeed;
		}
		else{ yawangle = 360.0; }
	}

	else if (!p->left && !p->right){
		yaw=0.0f;
	}

	updateVecs(yaw,pitch);

	//keep the eye up to date with where the model is
	eyex = p->centre.x;
	eyey = p->centre.y+2.0f;
	eyez = p->centre.z;
	
	//add the rotation vector(focus) onto the eye point to get the new focus point (at)
	atx = eyex + focus.getComponentX();	
	aty = eyey + focus.getComponentY();
	atz = eyez + focus.getComponentZ();

	
	upx=up.getComponentX();
	upy=up.getComponentY();
	upz=up.getComponentZ();


	
}

//For rotating the camera with the player movement
void Camera::updateVecs(float angleY, float angleP){
	//**************************************************************//
	//PITCH rotates around the x axis (in gamer terms it's usually y)
	//ROLL rotates around the z axis (x)
	//YAW rotates around the y axis (z)
	//**************************************************************//
	//This method gets the rotation vector that will be used to 
	//translate the eye point to get the at point

	//roll
	//Vec3f* r = new Vec3f(1,0,0);

	Quaternion *y = new Quaternion(angleY, &up); //yaw
	y->normalise(); 
	float yawMat[16]; //the matrix representing the yaw rotation quaternion
	y->convertToMatrix(yawMat);

	Quaternion *p = new Quaternion(angleP, &right); //pitch
	p->normalise();
	float pitchMat[16]; //the matrix representing the pitch rotation quaternion
	p->convertToMatrix(pitchMat);

	Vec3f *rot = &focus; //rotate the camfocus vector
	float vec[3]={0.0f,0.0f,0.0f};
	y->rotateVector(yawMat, rot, vec); //rotate the old focus by the yaw rotation
	focus.setComponentXYZ(vec[0],vec[1],vec[2]);

	Vec3f *rot2 = &focus; //rotate the updated camfocus vector
	float vec2[3]={0.0f,0.0f,0.0f};
	p->rotateVector(pitchMat, rot2, vec2); //rotate the old focus by the pitch rotation
	focus.setComponentXYZ(vec2[0],vec2[1],vec2[2]);

	//rotate its local right vector, so a 'nodding' movement will 
	//go up and down in the direction you are facing
	Vec3f *rightrot = &right; //rotate the right vector
	float vecright[3] = {0.0f,0.0f,0.0f};
	p->rotateVector(yawMat,rightrot,vecright);
	right.setComponentXYZ(vecright[0],vecright[1],vecright[2]);


}

//For rotating the camera with extra 'head' rotation (e.g. to look around without moving)
//Called in the updateHead method
//Not currently doing roll rotation as this is unecessary without the oculus
void Camera::updateVecs2(float angleY, float angleP, float angleR){
	//**************************************************************//
	//PITCH rotates around the x axis (in gamer terms it's usually y)
	//ROLL rotates around the z axis (x)
	//YAW rotates around the y axis (z)
	//**************************************************************//
	//This method gets the rotation vector that will be used to 
	//translate the eye point to get the at point

	//roll
	Quaternion *r = new Quaternion(angleR, &forward); //roll
	r->normalise(); 
	float rollMat[16]; //the matrix representing the roll rotation quaternion
	r->convertToMatrix(rollMat);

	Quaternion *y = new Quaternion(angleY, &up); //yaw
	y->normalise(); 
	float yawMat[16]; //the matrix representing the yaw rotation quaternion
	y->convertToMatrix(yawMat);

	Quaternion *p = new Quaternion(angleP, &right); //pitch
	p->normalise();
	float pitchMat[16]; //the matrix representing the pitch rotation quaternion
	p->convertToMatrix(pitchMat);

	Vec3f *rot = &focus; //rotate the camfocus vector
	float vec[3]={0.0f,0.0f,0.0f};
	y->rotateVector(yawMat, rot, vec); //rotate the old focus by the yaw rotation
	focus.setComponentXYZ(vec[0],vec[1],vec[2]);

	Vec3f *rot2 = &focus; //rotate the updated camfocus vector
	float vec2[3]={0.0f,0.0f,0.0f};
	p->rotateVector(pitchMat, rot2, vec2); //rotate the old focus by the pitch rotation
	focus.setComponentXYZ(vec2[0],vec2[1],vec2[2]);

	Vec3f *rot3 = &focus; //rotate the updated camfocus vector
	float vec3[3]={0.0f,0.0f,0.0f};
	r->rotateVector(rollMat, rot3, vec3); //rotate the old focus by the roll rotation
	focus.setComponentXYZ(vec3[0],vec3[1],vec3[2]);

	//rotate its local right vector, so a 'nodding' movement will 
	//go up and down in the direction you are facing
	Vec3f *rightrot = &right; //rotate the right vector
	float vecright[3] = {0.0f,0.0f,0.0f};
	y->rotateVector(yawMat,rightrot,vecright);
	right.setComponentXYZ(vecright[0],vecright[1],vecright[2]);

	//rotate its local forward vector
	Vec3f *forwardrot = &forward; //rotate the forward vector
	float vecforward[3] = {0.0f,0.0f,0.0f};
	y->rotateVector(yawMat,forwardrot,vecforward);
	forward.setComponentXYZ(vecforward[0],vecforward[1],vecforward[2]);


}


void Camera::updateHead(Player* p){
	//update with player position, but also rotate extra

	float yaw = 0.0;
	float pitch = 0.0;
	float roll = 0.0;

	//yawangle is only used for the zoom method
	if(p->lookLeft){
		yaw=p->turnSpeed;
	}

	if(p->lookRight){
		yaw=-p->turnSpeed;		
	}

	else if (!p->lookLeft && !p->lookRight){
		yaw=0.0f;
	}

	if(p->lookUp){
		pitch=p->turnSpeed;
	}

	if(p->lookDown){
		pitch=-p->turnSpeed;
	}

	else if (!p->lookUp && !p->lookDown){
		pitch=0.0f;
	}

	if(p->rollLeft){
		roll=p->turnSpeed;
	}

	if(p->rollRight){
		roll=-p->turnSpeed;
	}

	else if (!p->rollLeft && !p->rollRight){
		roll=0.0f;
	}
	

	updateVecs2(yaw, pitch, 0);

	//keep the eye up to date with where the model is
	eyex = p->centre.x;
	eyey = p->centre.y+2.0f;
	eyez = p->centre.z;
	
	//add the rotation vector(focus) onto the eye point to get the new focus point (at)
	atx = eyex + focus.getComponentX();	
	aty = eyey + focus.getComponentY();
	atz = eyez + focus.getComponentZ();

	
	upx=up.getComponentX();
	upy=up.getComponentY();
	upz=up.getComponentZ();
}
