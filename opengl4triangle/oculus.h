///		***
///
///				oculus.h - declaration of the oculus class, variables and methods - Tom
///
///		***

#ifndef OCULUS_H
#define OCULUS_H

#include "include\GL\glew.h"
#include "include\glm\glm.hpp"
#include "include\GLFW\glfw3.h"
#include "include\LibOVR\Include\OVR.h"
#include "include\OVRkill\OVR_Shaders.h"
#include "include\LibOVR\Src\Util\Util_Render_Stereo.h"
#include "include\LibOVR\Include\OVRkill.h"
#include <iostream>

const OVR::Vector3f upVector(0.0f, 1.0f, 0.0f);
const OVR::Vector3f forwardVector(0.0f, 0.0f, -1.0f);
const OVR::Vector3f rightVector(1.0f, 0.0f, 0.0f);
const float yawInit = 3.14159265f;
const float sensitivity = 1.0f;
const float moveSpeed = 3.0f;
const float standingHeight = 1.78f;
const float crouchHeight = 0.6f;

using namespace OVR;

// How big do we want our renderbuffer
#define FRAMEBUFFER_OBJECT_SCALE 3

class oculus : public OVR::MessageHandler{
public:

	//dems variables and ting
	enum MODE{
		MONO, STEREO, STEREO_DISTORT
	};

	// *** Oculus HMD Variable
	Ptr<DeviceManager>				ovrManager;
	Ptr<SensorDevice>				ovrSensor;
	Ptr<HMDDevice>					ovrHMD;
	SensorFusion					sFusion;
	HMDInfo							hmdInfo;
	OVR::Util::Render::StereoConfig sConfig;
	

	//timing ting
	double lastUpdate;
	UInt64 startupTicks;


	OVR::Matrix4f view;
	int eyeWidth, eyeHeight;
	int fboWidth, fboHeight;


	int r_width, r_height;
	MODE renderMode;
	bool useTracker, riftConn;
	long elapsed;
	float eyeYaw;
	float eyePitch;
	float eyeRoll;
	float lastSensorYaw;
	float viewAngleDeg;
	float bufferScaleUp;
	OVR::Vector3f eyePos;

	//dems methods
	oculus();
	~oculus();
	void init();
	bool isRiftDetected() const;
	bool getSensorOrientation(float& eyeYaw, float& eyePitch, float& eyeRoll) const;
	const OVR::HMDInfo& getHMDInfo() const;
	OVR::Util::Render::StereoConfig getStereoConfig() const;
	virtual void onMessage(const OVR::Message& msg);

private:
	void setupLRProjMat();
	void updateDeviceRot();
};
#endif