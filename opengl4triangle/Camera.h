//************************************************//
//   Implemented by Alice                         //
//   Class for the camera without the Oculus Rift //
//************************************************//

#ifndef Camera_H
#define	Camera_H

#include "Quaternion.h"
#include "vec3f.h"
#include "Player.h"
#include "gamePad.h"

static const float rads = 3.14159265359f / 180.0f;

class Camera{
public:
	Camera(Player* p);
	Camera();
	float eyex, eyey, eyez;
	float atx, aty, atz;
	float temp_atx, temp_aty, temp_atz;
	float upx, upy, upz;
	float zoomAmount;
	float pitchangle;
	float yawangle;
	Vec3f up, tempUp, right, forward, focus;
	void update(bool left, bool right, bool up, bool down);
	void update(Player* p);
	void translate(Player* p);
	void updateVecs(float angleY, float angleP);
	void updateVecs2(float angleY, float angleP, float angleR);
	void updateHead(Player* p);
	void zoom(bool in, bool out);
	void reInitialise(Player* p);
};
#endif	