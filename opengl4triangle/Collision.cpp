//************************************************//
//   Implemented by Alice                         //
//   Class for detecting collisions between       //
//   characters with each other and characters    //
//   with the level.                              //
//************************************************//

#include "Collision.h"

Collision::Collision(){

}

glm::vec3 Collision::checkCollision(Character* c1, Character* c2){
	collisionResult cr = SAT(c1, c2);
	if(cr.colliding == true){
		//printf("MTV: %.1f, %.1f, %.1f\n", cr.MTV.x, cr.MTV.y, cr.MTV.z); 
	}

	return cr.MTV;
}

glm::vec3 Collision::checkCollision(Character* c1, Level* l){
	for(int i=0; i<l->collPlanes.size(); i++){
		collisionResult cr = SAT(c1, l, i);
		if(cr.colliding == true){
			//printf("MTV: %.1f, %.1f, %.1f\n", cr.MTV.x, cr.MTV.y, cr.MTV.z); 
			return cr.MTV;
		}
	
	}
	return glm::vec3(0.0, 0.0, 0.0);
}


collisionResult Collision::SAT(Character* c1, Character* c2){
	glm::vec3 axis = glm::vec3(0.0f,0.0f,0.0f);
	glm::vec3 distanceBetween;
	glm::vec3 MTV(0.0f,0.0f,0.0f);
	Projection projA, projB;
	collisionResult result;
	float difference;
	float overlap=100.0f;
	glm::vec3 smallest(0.0f,0.0f,0.0f);

	for (int i = 0; i < 6; i++) 
		{
			//get the axis for this loop
			if (i < 3)
				axis = c1->bb.axes[i];
			else
				axis = c2->bb.axes[i-3];
	
			// project both shapes onto the current axis
			projA = Project(axis,c1);
			projB = Project(axis,c2);

			// do the projections overlap?
			if (projA.min < projB.min)
				difference = projB.min - projA.max;
			else
				difference = projA.min - projB.max;

			if (difference > 0)
			{
				//no overlap
				result.colliding = false;
				result.MTV = MTV;
				return result;
			}

	
        		float tempdiff = abs(difference);
        		if (tempdiff < overlap) 
			{
            	overlap = tempdiff;
            	smallest = axis;
				glm::vec3 o1 = c1->centre;
				glm::vec3 o2 = c2->centre;
				distanceBetween = o1 - o2;
				
            		if (glm::dot(distanceBetween, smallest) < 0.0){
                			smallest = -smallest;
					}
        	}
		}

	result.colliding = true;
	if(overlap<0.0001 && overlap >= 0.0){
		overlap = 0.001;
	}
	else if(overlap > -0.0001 && overlap <= 0.0){
		overlap = -0.001;
	}
    result.MTV = smallest*(overlap);
	if(result.MTV.y < 0.0f){
		result.MTV.y = 0.0f; //don't move it up or down to get out of collision!
	}

	return result;
	

}

collisionResult Collision::SAT(Character* c1, Level* l, int levelIndex){
	glm::vec3 levelAxes[3];
	levelAxes[0] = l->collPlanes.at(levelIndex).forward;
	levelAxes[1] = l->collPlanes.at(levelIndex).right;
	levelAxes[2] = l->collPlanes.at(levelIndex).up;

	glm::vec3 axis = glm::vec3(0.0f,0.0f,0.0f);
	glm::vec3 distanceBetween;
	glm::vec3 MTV(0.0f,0.0f,0.0f);
	Projection projA, projB;
	collisionResult result;
	float difference;
	float overlap=100.0f;
	glm::vec3 smallest(0.0f,0.0f,0.0f);

	for (int i = 0; i < 6; i++) 
		{
			//get the axis for this loop
			if (i < 3)
				axis = c1->bb.axes[i];
			else
				axis = levelAxes[i-3];
				
			// project both shapes onto the current axis
			projA = Project(axis,c1);
			projB = Project(axis,l, levelIndex);

			// do the projections overlap?
			if (projA.min < projB.min)
				difference = projB.min - projA.max;
			else
				difference = projA.min - projB.max;

			if (difference > 0)//should be negative if a's max is bigger than b's min or vice versa
			{
				//no overlap
				result.colliding = false;
				result.MTV = MTV;
				return result;
			}

			//get axis on which the smallest overlap lies
			//and the amount of overlap
        	float tempdiff = abs(difference);
        	if (tempdiff < overlap) 
			{
            	overlap = tempdiff;
            	smallest = axis;
				glm::vec3 o1 = c1->centre;
				glm::vec3 o2 = l->collPlanes.at(levelIndex).centre;
				distanceBetween = o1 - o2;							
            		if (glm::dot(distanceBetween, smallest) < 0.0){
                			smallest = -smallest;
					}
        	}
		}

	result.colliding = true;
	//if overlap is tiny
	if(overlap<0.0001 && overlap >= 0.0){
		overlap = 0.001;
	}
	else if(overlap > -0.0001 && overlap <= 0.0){
		overlap = -0.001;
	}
    result.MTV = smallest*(overlap);
	if(result.MTV.y < 0.0f){
		result.MTV.y = 0.0f; //don't move it up or down to get out of collision!
	}

	return result;
	

}

Projection Collision::Project(glm::vec3 axis, Character* c1){
	Projection result;
	float min, max;
	float dotProduct;
	for (int j = 0; j <8; j++)
	{
		//project the current point along the selected axis
		glm::vec3 temp = c1->bb.cornerPoints[j];
		dotProduct = glm::dot(temp, axis);

		if (j == 0)
		{
			min = dotProduct; 
			max = dotProduct;
		}
		// find the highest and lowest points for this object
		else
		{
			if (dotProduct < min)
				min = dotProduct;
			if (dotProduct > max)
				max = dotProduct;
		}
	}
	result.min = min;
	result.max = max;
	return result;

}

Projection Collision::Project(glm::vec3 axis, Level* l, int levelIndex){
	Projection result;
	float min, max;
	float dotProduct;
	for (int j = 0; j <8; j++)
	{
		//project the current point along the selected axis
		glm::vec3 temp = l->collPlanes.at(levelIndex).verts.at(j);
		dotProduct = glm::dot(temp, axis);

		if (j == 0)
		{
			min = dotProduct; 
			max = dotProduct;
		}
		// find the highest and lowest points for this object
		else
		{
			if (dotProduct < min)
				min = dotProduct;
			if (dotProduct > max)
				max = dotProduct;
		}
	}
	result.min = min;
	result.max = max;
	return result;

}