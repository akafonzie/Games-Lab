//////////////////////////////////////////////////////////////
//model from:                                               //
//http://www.turbosquid.com/FullPreview/Index.cfm/ID/615861 //
//////////////////////////////////////////////////////////////

//************************************************//
//   Implemented by Alice                         //
//   Class for items to be collected, includes    //
//   methods for player detection and             //
//   randomization of positions                   //
//************************************************//


#include "Item.h"

Item::Item(model* m){
	myModel = m;
	found = false;
	foundChecked = false;
	centre = glm::vec3(0.0, 0.0, 0.0);
	rumbleStart = 0.0;
}

Item::~Item(){

}

void Item::initItem(glm::vec3 translate, Level* l, int room){

	ModelView = glm::mat4(1.0f);
	glm::vec3 scale2 = glm::vec3(0.2, 0.2, 0.2);
	ModelView = glm::scale(ModelView, scale2);
	ModelView = glm::rotate(ModelView, -90.0f*radians, glm::vec3(1.0,0.0,0.0)); //rotate the model 90 on x axis

	glm::vec4 newcent;
	newcent = ModelView * glm::vec4(centre, 1.0); //multiply the centre point by the transformation matrix
	initcentre.x = newcent.x; initcentre.y = newcent.y; initcentre.z = newcent.z; //this transformed centre point is the starting centre point
	setCentre(initcentre.x, initcentre.y, initcentre.z); //update the centre for use by the camera
	//printf("%.2f, %.2f, %.2f\n", centre.x, centre.y, centre.z);

	ModelView = glm::translate(ModelView, translate); //move zombie to the large room (x and z translation)
	newcent = ModelView * glm::vec4(initcentre, 1.0);
	setCentre(newcent.x, newcent.y, newcent.z);

	glm::vec3 pos;
	switch(room){
		case 1: pos = randomizePos(l->collPlanes[0].min, l->collPlanes[0].max); //first room
			break;
		case 2: pos = randomizePos(l->collPlanes[11].min, l->collPlanes[11].max); //big room
			break;
		case 3: pos = randomizePos(l->collPlanes[23].min, l->collPlanes[23].max); //second small room
			break;
		case 4: pos = randomizePos(l->collPlanes[38].min, l->collPlanes[38].max); //second big room
			break;
		case 5: pos = randomizePos(l->collPlanes[48].min, l->collPlanes[48].max); //last small room
			break;
		default: pos = randomizePos(l->collPlanes[0].min, l->collPlanes[0].max); //first room
	}
	//glm::vec3 trans = randomizePos();
	//translate without changing initial centre point, otherwise the centre of rotation goes off
	ModelView = glm::translate(ModelView, pos); //move zombie to the large room (x and z translation)
	ModelView = glm::rotate(ModelView, 90.0f*radians, glm::vec3(0.0,0.0,1.0)); //rotate the model 90 on y axis
	newcent = ModelView * glm::vec4(initcentre, 1.0);
	setCentre(newcent.x, newcent.y, newcent.z); 
	foundChecked = false;

}

void Item::checkFound(Character* c, gamePad* g, float RT){
	if(foundChecked == false){//no longer need to check if already found once
		itemToPlayer.x = c->centre.x - centre.x;
		itemToPlayer.y = 0.0;
		itemToPlayer.z = c->centre.z - centre.z;

		vecMagnitude = glm::length(itemToPlayer);

		if(vecMagnitude < 5.0f || found == true){
			if(!found) rumbleStart = RT; //start timer as just been found		
			found = true;
			if(RT - rumbleStart < 0.4 && RT){ //rumble for 0.4 secs
				g->rumble(0.5, 0.5);
			}
			else{
				g->rumble(); //set rumble back to 0
				foundChecked = true; //been found, done the rumble, don't check any more	
			}
		}
	}
}

glm::vec3 Item::randomizePos(glm::vec3 min, glm::vec3 max){

	glm::vec3 vec = glm::vec3(0.0, 0.0, 0.0);
	float randx, randz;
	float scale = 5.0;
	float min_x = (min.x)*scale + 21.0; //+ scale*box width i.e. 5*4, + 1 because init x pos is 0.19
	float max_x = (max.x)*scale - 21.0;
	float min_z = (min.z)*scale + 20.0;
	float max_z = (max.z)*scale - 20.0;

	randx = min_x + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(max_x - min_x)));
	randz = min_z + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(max_z - min_z)));

	vec.x = randx; vec.y = -randz; vec.z =0.0;
	
	return vec;
}

//seed the randomizer 
void Item::initRand(){
	srand (time(NULL));

}

//debug shader methods; send lines or points to the shader
void Item::genLine(glm::vec3 point1, glm::vec3 point2){

	
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

void Item::drawLine(){
	//render model equiv

	glBindVertexArray(vao);
	//glDrawElements(GL_LINES, 1, GL_UNSIGNED_INT, 0);
	glDrawArrays (GL_LINES, 0, 2);

	glBindVertexArray(0);

}
