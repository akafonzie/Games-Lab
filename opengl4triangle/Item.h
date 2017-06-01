//************************************************//
//   Implemented by Alice                         //
//   Class for items to be collected, includes    //
//   methods for player detection and             //
//   randomization of positions                   //
//************************************************//

#ifndef Item_H
#define	Item_H
#include "modelLoader.h"
#define GLM_FORCE_RADIANS
#include "include\glm\gtc\matrix_transform.hpp"
#include "Quaternion.h"
#include "Character.h"
#include "Level.h"
#include <cstdlib>
#include <ctime>
#include "gamePad.h"

using namespace std;





class Item : public Character{
public:
	Item(model* m);
	~Item();
	void initItem(glm::vec3 translate, Level* l, int room);
	void initRand();
	glm::vec3 randomizePos(glm::vec3 min, glm::vec3 max);
	void checkFound(Character* c, gamePad* g, float RT);
	void drawLine();
	void genLine(glm::vec3 point1, glm::vec3 point2);

	bool found, foundChecked;
	glm::vec3 itemToPlayer; 
	float vecMagnitude;
	float rumbleStart;
	unsigned int vao;


};
#endif