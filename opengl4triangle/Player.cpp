//************************************************//
//   Implemented by Alice                         //
//   Class for the player                         //
//   Including methods for movement               //
//												  //
//   Oculus methods implemented by Tom            //
//************************************************//

#include "Player.h"


Player::Player(){
	myModel = NULL;
	heading = pitch = 0.0f;	

	chaseSpeed = 0.0f;

	initSpeed = speed = deltaSpeed = 25.0f;
	crouchSpeed	= deltaCrouchSpeed = 15.0f;
	sprintSpeed	= deltaSprintSpeed= 40.0f;
	initTurnSpeed = turnSpeed = deltaTurnSpeed= 60.0f;

	centre.x = 0.0f; centre.y = 0.0f; centre.z = 0.0f;
	sprint = crouch = sRight = sLeft = lookUp = lookDown = lookLeft = lookRight = left = right = false;
	initcentre.x = 0.0f; initcentre.y = 0.0f; initcentre.z = 0.0f;
	ModelView = glm::mat4(1.0f);
	forwardvec = glm::vec3(0.0, 0.0, 1.0);
	rightvec = glm::vec3(1.0, 0.0, 0.0);
	upvec = glm::vec3(0.0,1.0,0.0);
	torchDir = glm::vec3(0.0, 0.0, 1.0);
	torchPos = glm::vec3(0.0,0.0,0.0);
	move = true;
	caught = false;
	collision = false;

    torchDir.x = -0.5;
	torchDir.y = 0.3;
	torchDir.z = -0.5;

	coll = new Collision();
	WIN = false;
	LOSE = false;
}





void Player::start(oculus* oc)
{
	m_Rift = oc;
	m_Speed = 10.0f;
}

void Player::Move(){
	if(move){

		if(left){
			if(this->heading<360){
				this->heading+=turnSpeed;
			}
			else{ 
				this->heading = 0.0; 
			}
		}
		if(right){
			if(this->heading>0){
				this->heading-=turnSpeed;
			}
			else { 
				this->heading = 360.0; 
			}
		}

		if(collision){
			CollisionMove(); //translate out of collision
		}
		else{
			keyboardMove(); //move in which ever way the controller requests
			gamepadMove();				
		}


		updateCorners(boxdimp1, boxdimp2, boxdimp3);
		updateVecs();
		changeTorchDir(0);
		updateTorch();
	}
	
}

void Player::keyboardMove(){
		if(turnLeft){
			if(this->heading<360){
				this->heading+=turnSpeed;
			}
			else{ 
				this->heading = 0.0; 
			}
		}
		if(turnRight)
		{
			if(this->heading>0){
				this->heading-=turnSpeed;
			}
			else { 
				this->heading = 360.0; 
			}
		}
	



}

void Player::gamepadMove(){
	if(forward){
		m_Velocity.z = -m_Speed;
			centre.z += speed * cos(heading*radians);
			centre.x += speed * sin(heading*radians);
	}
	if(backward){	
		m_Velocity.z = m_Speed;
			centre.z -= speed * cos(heading*radians);
			centre.x -= speed * sin(heading*radians);		
	}	



	if(sLeft)
	{
		m_Velocity.x = -m_Speed;
		centre.z += speed * cos( (heading-90)*radians);
		centre.x += speed * sin( (heading-90)*radians);
	}

	if(sRight)
	{
		m_Velocity.x = m_Speed;
		centre.z += speed * cos( (heading+90)*radians);
		centre.x += speed * sin( (heading+90)*radians);
	}
}

void Player::CollisionMove(){
	centre.z +=collisionVec.z;
	centre.x +=collisionVec.x;
}

void Player::updateTorch(){

	//move torch with player
	torchPos.x = centre.x;
	torchPos.y = centre.y+2.0f;
	torchPos.z = centre.z;
	torchPos += (forwardvec * 5.0f);
}

void Player::changeTorchDir(float f){
	//rotate the initial torch direction by the player's heading
	glm::vec3 dir = glm::vec3(-0.5, 0.3, -0.5);

	//had to add 130 on to angle to ensure the torch is directly in front
	Quaternion *y = new Quaternion(heading+130, &glm::vec3(0.0, 1.0, 0.0)); //yaw
	y->normalise(); 
	float Mat[16]; //the matrix representing the yaw rotation quaternion
	y->convertToMatrix(Mat);

	glm::vec3 *rot = &dir; //rotate the torch dir
	float vec[3]={0.0f,0.0f,0.0f};
	y->rotateVector(Mat, rot, vec);
	torchDir.x = vec[0]; torchDir.y = vec[1]; torchDir.z = vec[2];


}



void::Player::checkCollision(Level* l){
	glm::vec3 MTV = coll->checkCollision(this, l);	
	if(MTV.x == 0.0 && MTV.z == 0.0){
		collision = false;
	}
	else{
		collision = true;
	}
	//vector the player must be translated by in order to move out of collision
	collisionVec.x = MTV.x;
	collisionVec.y = MTV.y;
	collisionVec.z = MTV.z;
}

