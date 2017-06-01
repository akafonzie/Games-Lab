///		***
///
///		Level.cpp - implementation of the Level class - Tom
///		This class loads a pre-built 3D model of a level, and any associated textures/materials using the 
///		modelLoader class. 
///		Automated generation of collision materials were attempted using regular grids, and collision planes. 
///		However, it was found that the automated planes and grids were not generating effectively,
///		so the collision planes were (time-consumingly) manually generated.
///
///		***


#include "Level.h"

Level::Level()
{
	levelModel = NULL;
	ModelView = glm::mat4(1.0f);
	vbp = 0;
}

Level::Level(model* m)
{
	levelModel = m;
	ModelView = glm::mat4(1.0f);
	vbp = 0;
}

void Level::renderCollPlanes()
{
	if(vbp==54){vbp = 0;}
	for(size_t i = 0; i < collPlanes.size(); i ++){
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);
		GLuint vbo = collPlanes[i].vbo;
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glBindVertexArray(vao);
		glDrawArrays (GL_LINE_LOOP, 0, 8);
		glBindVertexArray(0);
	}
}

void Level::makeCollPlanes(){
	manualConstruct();

	float BBMaxDepth = 5.0f;  //set the overall BB/BP depth
	float depth = BBMaxDepth *0.5f;

	cout<< "collPlanes size: " << collPlanes.size() << endl;
	for(size_t i = 0; i < collPlanes.size(); i++){
		collPlanes[i].thePlane.zero();
		plane pTemp;
		pTemp.zero();
		float x = collPlanes[i].max.x - collPlanes[i].min.x;
		float y = collPlanes[i].max.y - collPlanes[i].min.y;
		float z = collPlanes[i].max.z - collPlanes[i].min.z;
		glm::vec3 tMin = collPlanes[i].min;
		glm::vec3 tMax = collPlanes[i].max;
		//set the relative forward and right vectors for use in the SAT collision model
		if(x > z){
			collPlanes[i].forward	= glm::vec3(0, 0, 1);
			collPlanes[i].right		= glm::vec3(1, 0, 0);
		}else{
			collPlanes[i].forward	= glm::vec3(1, 0, 0);
			collPlanes[i].right		= glm::vec3(0, 0, 1);
		}
		collPlanes[i].up = glm::vec3(0, 1, 0);
		//set the vertex values for each point of the collPlane
		if( (x > z) && (y > 0) ){
			/// ** if we are here that means that z is the forward vecotr (as usual)
			//set the back vertices of the plane
			pTemp.blb = glm::vec3(tMin.x, tMin.y, tMax.z + depth);
			pTemp.brb = glm::vec3(tMax.x, tMin.y, tMax.z + depth);
			pTemp.trb = glm::vec3(tMax.x, tMax.y, tMax.z + depth);
			pTemp.tlb = glm::vec3(tMin.x, tMax.y, tMax.z + depth);
			//set the front vertices of the plane
			pTemp.blf = glm::vec3(tMin.x, tMin.y, tMin.z - depth);
			pTemp.brf = glm::vec3(tMax.x, tMin.y, tMin.z - depth);
			pTemp.trf = glm::vec3(tMax.x, tMax.y, tMin.z - depth);
			pTemp.tlf = glm::vec3(tMin.x, tMax.y, tMin.z - depth);
			
			collPlanes[i].verts.push_back(pTemp.blb);collPlanes[i].verts.push_back(pTemp.brb);
			collPlanes[i].verts.push_back(pTemp.trb);collPlanes[i].verts.push_back(pTemp.tlb);
			collPlanes[i].verts.push_back(pTemp.blf);collPlanes[i].verts.push_back(pTemp.brf);
			collPlanes[i].verts.push_back(pTemp.trf);collPlanes[i].verts.push_back(pTemp.tlf);				
		}else if ( (x < z) && (y > 0) ){
			/// ** if we are here that means that x is the forward vecotr (instead of z)
			//set the back vertices of the plane
			pTemp.blb = glm::vec3(tMin.x + depth, tMin.y, tMin.z);
			pTemp.brb = glm::vec3(tMin.x + depth, tMin.y, tMax.z);
			pTemp.trb = glm::vec3(tMin.x + depth, tMax.y, tMax.z);
			pTemp.tlb = glm::vec3(tMin.x + depth, tMax.y, tMin.z);
			//set the front vertices of the plane
			pTemp.blf = glm::vec3(tMin.x - depth, tMin.y, tMin.z);
			pTemp.brf = glm::vec3(tMin.x - depth, tMin.y, tMax.z);
			pTemp.trf = glm::vec3(tMin.x - depth, tMax.y, tMax.z);
			pTemp.tlf = glm::vec3(tMin.x - depth, tMax.y, tMin.z);

			collPlanes[i].verts.push_back(pTemp.blb);collPlanes[i].verts.push_back(pTemp.brb);
			collPlanes[i].verts.push_back(pTemp.trb);collPlanes[i].verts.push_back(pTemp.tlb);
			collPlanes[i].verts.push_back(pTemp.blf);collPlanes[i].verts.push_back(pTemp.brf);
			collPlanes[i].verts.push_back(pTemp.trf);collPlanes[i].verts.push_back(pTemp.tlf);
		}else{ //if this is a floor or a ceiling
			//set the bottom vertices of the plane
			pTemp.blb = glm::vec3(tMin.x, tMin.y - depth, tMin.z);
			pTemp.brb = glm::vec3(tMax.x, tMin.y - depth, tMin.z);
			pTemp.trb = glm::vec3(tMax.x, tMin.y - depth, tMax.z);
			pTemp.tlb = glm::vec3(tMin.x, tMin.y - depth, tMax.z);
			//set the top vertices of the plane
			pTemp.blf = glm::vec3(tMin.x, tMax.y + depth, tMin.z);
			pTemp.brf = glm::vec3(tMax.x, tMax.y + depth, tMin.z);
			pTemp.trf = glm::vec3(tMax.x, tMax.y + depth, tMax.z);
			pTemp.tlf = glm::vec3(tMin.x, tMax.y + depth, tMax.z);

			collPlanes[i].verts.push_back(pTemp.blb);collPlanes[i].verts.push_back(pTemp.brb);
			collPlanes[i].verts.push_back(pTemp.trb);collPlanes[i].verts.push_back(pTemp.tlb);
			collPlanes[i].verts.push_back(pTemp.blf);collPlanes[i].verts.push_back(pTemp.brf);
			collPlanes[i].verts.push_back(pTemp.trf);collPlanes[i].verts.push_back(pTemp.tlf);
		}

		collPlanes[i].vbo = i; //set the vbo identifier to be the current collPlane we are working on
		glGenBuffers(1, &collPlanes[i].vbo);
		glBindBuffer(GL_ARRAY_BUFFER, collPlanes[i].vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 24, &collPlanes[i].verts[0], GL_DYNAMIC_DRAW);
		collPlanes[i].thePlane = pTemp; //set the current collPlane's plane as the temp plane pTemp

		//gen centre
		glm::vec3 mid1, mid2, mid1_2, mid3, mid4, mid3_4;
		mid1 = (collPlanes[i].verts.at(0) + collPlanes[i].verts.at(4)) / 2.0f;
		mid2 = (collPlanes[i].verts.at(1) + collPlanes[i].verts.at(5)) / 2.0f;
		mid3 = (collPlanes[i].verts.at(3) + collPlanes[i].verts.at(7)) / 2.0f;
		mid4 = (collPlanes[i].verts.at(2) + collPlanes[i].verts.at(6)) / 2.0f;
		mid1_2 = (mid1+mid2)/2.0f;
		mid3_4 = (mid3+mid4)/2.0f;
		collPlanes[i].centre = (mid1_2+mid3_4)/2.0f;

	}

}

void Level::manualConstruct(){
	//This was boring

	for(size_t i = 0; i < 54; i++){
		collPlane tempCP;
		tempCP.zero();
		collPlanes.push_back(tempCP);
	}

	/// *** first room
	collPlanes[0].min = glm::vec3(-50.0, 0.00, -50.0);		collPlanes[0].max = glm::vec3(50.0, 0.00, 50.0);	// floor
	collPlanes[1].min = glm::vec3(-50.0, 40.0, -50.0);		collPlanes[1].max = glm::vec3(50.0, 40.0, 50.0);	// ceiling
	collPlanes[2].min = glm::vec3(50.0, 0.00, -50.0);		collPlanes[2].max = glm::vec3(50.0, 40.0, 50.0);	// wall 1
	collPlanes[3].min = glm::vec3(-50.0, 0.00, 50.0);		collPlanes[3].max = glm::vec3(50.0, 40.0, 50.0);	// wall 2
	collPlanes[4].min = glm::vec3(-50.0, 0.00, -50.0);		collPlanes[4].max = glm::vec3(50.0, 40.0, -50.0);	// wall 3
	collPlanes[5].min = glm::vec3(-50.0, 0.00, -50.0);		collPlanes[5].max = glm::vec3(-50.0, 40.0, -17.5);	// wall 4
	collPlanes[6].min = glm::vec3(-50.0, 0.00, 12.5);		collPlanes[6].max = glm::vec3(-50.0, 40.0, 50.0);	//wall 5

	/// *** first hall
	collPlanes[7].min = glm::vec3(-150.0, 0.00, -17.5);		collPlanes[7].max = glm::vec3(-50.0, 0.00, 12.5);	//floor
	collPlanes[8].min = glm::vec3(-150.0, 40.0, -17.5);		collPlanes[8].max = glm::vec3(-50.0, 40.0, 12.5);	//ceiling
	collPlanes[9].min = glm::vec3(-150.0, 0.00, -17.5);		collPlanes[9].max = glm::vec3(-50.0, 40.0, -17.5);	//wall1
	collPlanes[10].min = glm::vec3(-150.0, 0.00, 12.5);		collPlanes[10].max = glm::vec3(-50.0, 40.0, 12.5);	//wall2

	/// *** Big Room
	collPlanes[11].min = glm::vec3(-350.0, 0.00, -50.0);	collPlanes[11].max = glm::vec3(-150.0, 0.00, 150.0);// floor
	collPlanes[12].min = glm::vec3(-350.0, 40.0, -50.0);	collPlanes[12].max = glm::vec3(-150.0, 40.0, 150.0);// ceiling
	collPlanes[13].min = glm::vec3(-150.0, 0.00, 12.5);		collPlanes[13].max = glm::vec3(-150.0, 40.0, 150.0);// wall1
	collPlanes[14].min = glm::vec3(-150.0, 0.00, -50.0);	collPlanes[14].max = glm::vec3(-150.0, 40.0, -17.5);// wall2
	collPlanes[15].min = glm::vec3(-350.0, 0.00, -50.0);	collPlanes[15].max = glm::vec3(-350.0, 40.0, 120.0);// wall3
	collPlanes[16].min = glm::vec3(-350.0, 0.00, 150.0);	collPlanes[16].max = glm::vec3(-150.0, 40.0, 150.0);// wall4
	collPlanes[17].min = glm::vec3(-295.0, 0.00, -50.0);	collPlanes[17].max = glm::vec3(-150.0, 40.0, -50.0);// wall5
	collPlanes[18].min = glm::vec3(-350, 0.00, -50.0);		collPlanes[18].max = glm::vec3(-325.0, 40.0, -50.0);// wall6

	/// *** Dead End Hall
	collPlanes[19].min = glm::vec3(-450.0, 0.00, 120.0);	collPlanes[19].max = glm::vec3(-350.0, 0.00, 150.0);// floor
	collPlanes[20].min = glm::vec3(-450.0, 40.0, 120.0);	collPlanes[20].max = glm::vec3(-350.0, 40.0, 150.0);// ceiling
	collPlanes[21].min = glm::vec3(-450.0, 0.00, 120.0);	collPlanes[21].max = glm::vec3(-350.0, 40.0, 120.0);// wall1
	collPlanes[22].min = glm::vec3(-450.0, 0.00, 150.0);	collPlanes[22].max = glm::vec3(-350.0, 40.0, 150.0);// wall2

	/// *** Dead end Room
	collPlanes[23].min = glm::vec3(-510.0, 0.00, 50.0);		collPlanes[23].max = glm::vec3(-450.0, 0.00, 150.0);// floor
	collPlanes[24].min = glm::vec3(-510.0, 40.0, 50.0);		collPlanes[24].max = glm::vec3(-450.0, 40.0, 150.0);// ceiling
	collPlanes[25].min = glm::vec3(-510.0, 0.00, 50.0);		collPlanes[25].max = glm::vec3(-450.0, 40.0, 50.0);// wall1
	collPlanes[26].min = glm::vec3(-510.0, 0.00, 150.0);	collPlanes[26].max = glm::vec3(-450.0, 40.0, 150.0);// wall2
	collPlanes[27].min = glm::vec3(-450.0, 0.00, 50.0);		collPlanes[27].max = glm::vec3(-450.0, 40.0, 120.0);// wall3
	collPlanes[28].min = glm::vec3(-510.0, 0.00, 50.0);		collPlanes[28].max = glm::vec3(-510.0, 40.0, 150.0);// wall4

	/// *** second hall
	collPlanes[29].min = glm::vec3(-325.0, 0.00, -150.0);	collPlanes[29].max = glm::vec3(-295.0, 0.00, -50.0);// floor
	collPlanes[30].min = glm::vec3(-325.0, 40.0, -150.0);	collPlanes[30].max = glm::vec3(-295.0, 40.0, -50.0);// ceiling
	collPlanes[31].min = glm::vec3(-325.0, 0.00, -150.0);	collPlanes[31].max = glm::vec3(-325.0, 40.0, -50.0);// wall4
	collPlanes[32].min = glm::vec3(-295.0, 0.00, -150.0);	collPlanes[32].max = glm::vec3(-295.0, 40.0, -50.0);// wall4
	
	/// *** third hall
	collPlanes[34].min = glm::vec3(-495.0, 0.00, -150.0);	collPlanes[34].max = glm::vec3(-295.0, 0.00, -180.0);// floor
	collPlanes[35].min = glm::vec3(-495.0, 40.0, -150.0);	collPlanes[35].max = glm::vec3(-295.0, 40.0, -180.0);// ceiling
	collPlanes[36].min = glm::vec3(-495.0, 0.00, -180.0);	collPlanes[36].max = glm::vec3(-295.0, 40.0, -180.0);// wall1
	collPlanes[37].min = glm::vec3(-495.0, 0.00, -150.0);	collPlanes[37].max = glm::vec3(-325.0, 40.0, -150.0);// wall2

	/// *** long/wide hall
	collPlanes[38].min = glm::vec3(-495.0, 0.00, -550.0);	collPlanes[38].max = glm::vec3(-595.0, 00.0, -150.0);// floor
	collPlanes[39].min = glm::vec3(-495.0, 40.0, -550.0);	collPlanes[39].max = glm::vec3(-595.0, 40.0, -150.0);// ceiling
	collPlanes[40].min = glm::vec3(-595.0, 0.00, -150.0);	collPlanes[40].max = glm::vec3(-495.00, 40.0, -150.0);// wall 1
	collPlanes[41].min = glm::vec3(-595.0, 0.00, -520.0);	collPlanes[41].max = glm::vec3(-595.0, 40.0, -150.0);// wall 2	
	collPlanes[42].min = glm::vec3(-595.0, 0.00, -550.0);	collPlanes[42].max = glm::vec3(-495, 40.0, -550.0);	 // wall 3
	collPlanes[43].min = glm::vec3(-495.0, 0.00, -550.0);	collPlanes[43].max = glm::vec3(-495.0, 40.0, -180.0);// wall 4

	/// *** fourth hall
	collPlanes[44].min = glm::vec3(-695.0, 0.00, -550.0);	collPlanes[44].max = glm::vec3(-595.0, 0.00, -520.0);// floor
	collPlanes[45].min = glm::vec3(-695.0, 40.0, -550.0);	collPlanes[45].max = glm::vec3(-595.0, 40.0, -520.0);// ceiling
	collPlanes[46].min = glm::vec3(-695.0, 0.00, -550.0);	collPlanes[46].max = glm::vec3(-595.0, 40.0, -550.0);// wall1
	collPlanes[47].min = glm::vec3(-695.0, 0.00, -520.0);	collPlanes[47].max = glm::vec3(-595.0, 40.0, -520.0);// wall2

	/// *** final room
	collPlanes[48].min = glm::vec3(-755.0, 0.00, -620.0);	collPlanes[48].max = glm::vec3(-695.0, 0.00, -520.0);// floor
	collPlanes[49].min = glm::vec3(-755.0, 40.0, -620.0);	collPlanes[49].max = glm::vec3(-695.0, 40.0, -520.0);// ceiling
	collPlanes[50].min = glm::vec3(-755.0, 0.00, -620.0);	collPlanes[50].max = glm::vec3(-755.0, 40.0, -520.0);// wall1
	collPlanes[51].min = glm::vec3(-695.0, 0.00, -620.0);	collPlanes[51].max = glm::vec3(-695.0, 40.0, -550.0);// wall2
	collPlanes[52].min = glm::vec3(-755.0, 0.00, -620.0);	collPlanes[52].max = glm::vec3(-695.0, 40.0, -620.0);// wall3
	collPlanes[53].min = glm::vec3(-755.0, 0.00, -520.0);	collPlanes[53].max = glm::vec3(-695.0, 40.0, -520.0);// wall4
}
