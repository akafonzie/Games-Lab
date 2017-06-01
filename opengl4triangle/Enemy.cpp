//************************************************//
//   Implemented by Alice                         //
//   Class for the zombies                        //
//   Including methods for their movement,        //
//   collision response and response to the player//
//************************************************//

#include "Enemy.h"



Enemy::Enemy(){
	myModel = NULL;
	sprint = crouch = sRight = sLeft = lookUp = lookDown = lookLeft = lookRight = false;
	heading = pitch = 0.0f;

	crouchSpeed = sprintSpeed = 0.0f;

	initSpeed = speed = deltaSpeed = 15.0f;
    initTurnSpeed = turnSpeed = deltaTurnSpeed = 30.0f;
	chaseSpeed = deltaChaseSpeed = 45.0f;

	centre = glm::vec3(0.0, 0.0, 0.0);
	initcentre = glm::vec3(0.0, 0.0, 0.0);
	forwardvec = glm::vec3(0.0, 0.0, 1.0);
	rightvec = glm::vec3(1.0, 0.0, 0.0);
	upvec = glm::vec3(0.0,1.0,0.0);
	zombToPlayer = normalizedZTP = glm::vec3(0.0, 0.0, 0.0);
	vao = 0;
	angleBetween = 0.0;
	vecMagnitude = 0.0;
	Tamount = 0.0;
	edge1 = move = true;
	edge2 = edge3 = edge4 = canSee = collision = lvlCollision = false;
	anim = 1;
	animtime = rumbleStart = 0.0;
}

Enemy::Enemy(model* model){
	myModel = model;
	sprint = crouch = sRight = sLeft = lookUp = lookDown = 
		lookLeft = lookRight = justCollided = false;
	heading = pitch = 0.0f;

	crouchSpeed = sprintSpeed = 0.0f;

	initSpeed = speed = deltaSpeed = 15.0f;
    initTurnSpeed = turnSpeed = deltaTurnSpeed = 30.0f;
	chaseSpeed = deltaChaseSpeed = 45.0f;

	centre = glm::vec3(0.0, 0.0, 0.0);
	initcentre = glm::vec3(0.0, 0.0, 0.0);
	forwardvec = glm::vec3(0.0, 0.0, 1.0);
	rightvec = glm::vec3(1.0, 0.0, 0.0);
	upvec = glm::vec3(0.0,1.0,0.0);
	zombToPlayer = normalizedZTP = glm::vec3(0.0, 0.0, 0.0);
	vao = 0;
	angleBetween = 0.0;
	vecMagnitude = 0.0;
	Tamount = 0.0;
	edge1 = move = true;
	edge2 = edge3 = edge4 = canSee = collision = 
		lvlCollision = justCollided = false;
	anim = 1;
	animtime = rumbleStart = 0.0;

}

void Enemy::setUpZombie(glm::vec3 scale, glm::vec3 translate){

	ModelView = glm::mat4(1.0f);
	ModelView = glm::scale(ModelView, scale);	
	ModelView = glm::rotate(ModelView, -90.0f*radians, glm::vec3(1.0,0.0,0.0)); //rotate the model 90 on x axis

	//do the same scaling and rotation to the centre point
	glm::vec4 newcent;
	newcent = ModelView * glm::vec4(centre, 1.0); //multiply the centre point by the transformation matrix
	initcentre.x = newcent.x; initcentre.y = newcent.y; initcentre.z = newcent.z; //this transformed centre point is the starting centre point
	setCentre(initcentre.x, initcentre.y, initcentre.z); //update the centre for use by the camera
	//printf("%.2f, %.2f, %.2f\n", centre.x, centre.y, centre.z);

	//translate without changing initial centre point, otherwise the centre of rotation goes off
	ModelView = glm::translate(ModelView, translate); //move zombie to the large room (x and z translation)
	newcent = ModelView * glm::vec4(initcentre, 1.0);
	setCentre(newcent.x, newcent.y, newcent.z);
	//printf("%.2f, %.2f, %.2f\n", centre.x, centre.y, centre.z);

	coll = new Collision();

}

void Enemy::Move(float amount1, float amount2, Player* p, gamePad* g, float RT){

	checkSight(p);
	if(canSee) checkCaught(p, g, RT);
	
	if(move){

		//circuit movement is the last choice if all other bools are false
		if(!canSee && !collision && !lvlCollision){
			CircuitMove(amount1, amount2, p);
		}
	
		else if(collision || lvlCollision){
			CollisionMove();	
		}

		else if(canSee){
			Chase(p);
		}

	
		glm::vec4 newcent;
		//newcent.x = initcentre.x; newcent.y = initcentre.y; newcent.z = initcentre.z; newcent.w = 1.0;
		newcent = ModelView * glm::vec4(initcentre, 1.0);
		//multiply the y by 5 otherwise it gets scaled
		//it appears they all get scaled twice, but we need x and z to increase at a faster (scaled) pace
		//so these don't get divided by 5.
		this->setCentre(newcent.x, -(newcent.y*5.0), newcent.z);

		updateCorners(boxdimz1, boxdimz2, boxdimz3);
		updateVecs();
	}

	//genLine(p->centre, centre); //for debugging
	genLine(centre + forwardvec*10.f, centre); //for debugging
	genPoints(); //for debugging
	
	
}

void Enemy::CircuitMove(float amount1, float amount2, Player* p){
	float first = amount1;
	float second = amount1+amount2;
	float third = amount1+amount2+amount1;
	float fourth = amount1+amount2+amount1+amount2;

	bool translate = false;
	bool rotate = false;

	bool canMove;
	float speed = this->speed*3.0;

	if( (animtime > 0.76 && animtime < 0.98) || (animtime > 2.1 && animtime < 2.32)){
		canMove = true;
	}
	else canMove = false;

	if(edge1){
		if(Tamount < first){
			if(canMove){
				translate = true;
				Tamount +=speed;
			}
		}
		else if(Tamount >= first){
			if(heading < 90){
				rotate = true;
				increaseHeading();
			}
			else{
				edge1 = false;
				edge2 = true;
			}
		}
	}

	if(edge2){		
		if(first-1 < Tamount && Tamount < second){
			if(canMove){
				translate = true;
				Tamount +=speed;
			}
		}
		else if (Tamount >= second){
			if(89 < heading && heading < 180){
				rotate = true;
				increaseHeading();
			}
			else{
				edge2 = false;
				edge3 = true;
			}
		}
	}

	if(edge3){
		if(second-1 < Tamount && Tamount < third){
			if(canMove){
				translate = true;
				Tamount +=speed;
			}
		}
		else if (Tamount >= third){
			if(179 < heading && heading < 270){
				rotate = true;
				increaseHeading();
			}
			else{
				edge3 = false;
				edge4 = true;
			}
		}
	}

	if(edge4){
		if(third-1 < Tamount && Tamount < fourth){
			if(canMove){
				translate = true;	
				Tamount +=speed;
			}
		}
		else if (Tamount >= fourth){
			if(269 < heading && heading <= 360){
				rotate = true;
				increaseHeading();
			}
			else{
				Tamount = 0;
				edge4 = false;
				edge1 = true;
			}
			
		}
	}

	
	if(rotate){
		ModelView = glm::rotate(ModelView, turnSpeed*radians, glm::vec3(0.0,0.0,1.0));
	}

	if(translate){
		ModelView = glm::translate(ModelView, glm::vec3(0.0, -speed, 0.0f)); 
	}
	
}

void Enemy::CollisionMove(){
	//move out of collision and rotate slightly
	if(lvlCollision){
		ModelView = glm::translate(ModelView, lvlCollisionVec);
		increaseHeading();
		ModelView = glm::rotate(ModelView, turnSpeed*radians, glm::vec3(0.0,0.0,1.0));	
		justCollided = true;
		lvlCollision = false;
	}
	//move out of collision
	if(collision){
		ModelView = glm::translate(ModelView, collisionVec);
		collision = false;
	}
}

void Enemy::Chase(Player* p){
	//follow in direction of normalizedZTP
	//rotate so that angleBetween is 0
	glm::vec3 move;
	move.x = normalizedZTP.x;
	move.y = normalizedZTP.z;
	move.z = normalizedZTP.y;

	changeHeading(-angleBetween);
	ModelView = glm::rotate(ModelView, (-angleBetween) * radians, glm::vec3(0.0, 0.0, 1.0));

	//ModelView = glm::translate(ModelView, move*speed);
	ModelView = glm::translate(ModelView, glm::vec3(0.0, -speed, 0.0f)); 



}

void Enemy::checkSight(Player* p){
	zombToPlayer.x = p->centre.x - centre.x;
	zombToPlayer.y = 0.0;
	zombToPlayer.z = p->centre.z - centre.z;

	normalizedZTP = glm::normalize(zombToPlayer);

	float dot = glm::dot(normalizedZTP, forwardvec);
    
	angleBetween = degrees * acos(dot);
	if(angleBetween != angleBetween) angleBetween = 0.0;

	glm::vec3 cross = glm::normalize(glm::cross(normalizedZTP, forwardvec));
	//printf("Cross: %.1f, %.1f, %.1f\n", cross.x, cross.y, cross.z);
	
	if(cross.y < 0.0){
		//make sure you don't try to negate 0 or it breaks!
		if(angleBetween != 0.0) angleBetween = -angleBetween;
	}
	//printf("A: "); printf("%.1f\n", angleBetween);

	vecMagnitude = glm::length(zombToPlayer);
	//printf("M: "); printf("%.1f\n", vecMagnitude);

	float radius, angle;
	if(canSee){//easier to detect when already seen
		radius = 80.0f; 
		angle = 160.0f;	
	}
	else{
		if(p->crouch){//harder to detect when crouching
			radius = 30.0f;
		}
		if(p->sprint){//easier to detect when sprinting
			radius = 70.0f;
		}
		else radius = 60.0f;

		//angle is the same for all speeds
		angle = 55.0f;			
	}

	if((angleBetween <= angle && angleBetween >= -angle && vecMagnitude <= radius) || vecMagnitude <=8.0f){
		//printf("I can see you...\n");
		speed = deltaChaseSpeed;
		anim = 2;
		canSee = true;
	}
	else{
		speed = deltaSpeed;
		anim = 1;
		canSee = false;
	}


}

void Enemy::checkCaught(Player* p, gamePad* g, float RT){

	float radius = 5.0;

	if(vecMagnitude <= radius){
		anim = 3;
		if(move) rumbleStart = RT; //if only just caught
		caught = true;
		p->caught = true;
		move = false;
		p->move = false;
		
		//rumble controller for 2 seconds
		if(RT - rumbleStart < 2.0)
			g->rumble(0.5, 0.5);
		else{
			g->rumble();
			p->LOSE = true;
		}

	}
}

//generate VBO to store points for lines
void Enemy::genLine(glm::vec3 point1, glm::vec3 point2){

	float points[] = {
		point1.x, point1.y, point1.z,
		point2.x, point2.y, point2.z,
	};


	unsigned int vbo = 0;
	glGenBuffers (1, &vbo);
	glBindBuffer (GL_ARRAY_BUFFER, vbo);
	glBufferData (GL_ARRAY_BUFFER, 6 * sizeof (float), points, GL_STATIC_DRAW);


	glGenVertexArrays (1, &vao);
	glBindVertexArray (vao);
	glEnableVertexAttribArray (0);
	glBindBuffer (GL_ARRAY_BUFFER, vbo);
	glVertexAttribPointer (0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
}

//generate VBO to store points
void Enemy::genPoints(){

	float points[] = {
		bb.cornerPoints[0].x, bb.cornerPoints[0].y, bb.cornerPoints[0].z,
		bb.cornerPoints[1].x, bb.cornerPoints[1].y, bb.cornerPoints[1].z,
		bb.cornerPoints[2].x, bb.cornerPoints[2].y, bb.cornerPoints[2].z,
		bb.cornerPoints[3].x, bb.cornerPoints[3].y, bb.cornerPoints[3].z,
		bb.cornerPoints[4].x, bb.cornerPoints[4].y, bb.cornerPoints[4].z,
		bb.cornerPoints[5].x, bb.cornerPoints[5].y, bb.cornerPoints[5].z,
		bb.cornerPoints[6].x, bb.cornerPoints[6].y, bb.cornerPoints[6].z,
		bb.cornerPoints[7].x, bb.cornerPoints[7].y, bb.cornerPoints[7].z,
	};


	unsigned int vbo = 0;
	glGenBuffers (1, &vbo);
	glBindBuffer (GL_ARRAY_BUFFER, vbo);
	glBufferData (GL_ARRAY_BUFFER, 24 * sizeof (float), points, GL_STATIC_DRAW);


	glGenVertexArrays (1, &vao);
	glBindVertexArray (vao);
	glEnableVertexAttribArray (0);
	glBindBuffer (GL_ARRAY_BUFFER, vbo);
	glVertexAttribPointer (0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

}

//send line to shader
void Enemy::drawLine(){

	glBindVertexArray(vao);
	//glDrawElements(GL_LINES, 1, GL_UNSIGNED_INT, 0);
	glDrawArrays (GL_LINES, 0, 2);

	glBindVertexArray(0);

}

//send points to shader
void Enemy::drawPoints(){

	glBindVertexArray(vao);
	glDrawArrays (GL_POINTS, 0, 8);
	glBindVertexArray(0);
}

void Enemy::increaseHeading(){

	if(heading<360){
		heading+=turnSpeed;
	}	
	else heading = 0.0; 

}

void Enemy::changeHeading(float amount){
	//amount will be the angle between the zombie and player
	if(amount > 0.0){//increase
		float newHeading = heading + amount;
		if(newHeading > 360.0){
			float amountOver = newHeading - 360.0;
			heading = amountOver;
		}
		else{
			heading = newHeading;
		}
	}
	else{//decrease
		float newHeading = heading + amount;
		if(newHeading < 0.0){
			float amountUnder = 0.0 - newHeading;
			heading = 360.0 - amountUnder;
		}
		else{
			heading = newHeading;
		}
	}
	//printf("Heading: %.1f\n", heading);
}

void::Enemy::checkCollision(Character* other){
	if(!collision){
		glm::vec3 MTV = coll->checkCollision(this, other);	
		if(MTV.x == 0.0 && MTV.z == 0.0){
			collision = false;
		}
		else{
			collision = true;
		}
		//vector that the zombie needs to translate to move out of collision
		collisionVec.x = MTV.x;
		collisionVec.y = MTV.z;
		collisionVec.z = MTV.y;
	}


}

void Enemy::checkLvlCollision(Level* l){
	if(!lvlCollision){
		glm::vec3 MTV2 = coll->checkCollision(this, l);	
		if(MTV2.x == 0.0 && MTV2.z == 0.0){
			lvlCollision = false;
		}
		else{
			lvlCollision = true;
		}
		//vector that the zombie needs to translate to move out of collision
		lvlCollisionVec.x = MTV2.x;
		lvlCollisionVec.y = MTV2.z;
		lvlCollisionVec.z = MTV2.y;
	}

}



