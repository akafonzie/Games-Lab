///		***
///
///				gamePad.h - declaration of the gamePad class, variables and methods - Tom
///
///		***
#ifndef GAMEPAD_H
#define GAMEPAD_H

#include <Windows.h>
#include "include\xInput\Xinput.h"

//xinput button values
static const WORD xButt[]={
		XINPUT_GAMEPAD_A, 
		XINPUT_GAMEPAD_B, 
		XINPUT_GAMEPAD_X, 
		XINPUT_GAMEPAD_Y, 
		XINPUT_GAMEPAD_DPAD_DOWN, 
		XINPUT_GAMEPAD_DPAD_UP, 
		XINPUT_GAMEPAD_DPAD_LEFT, 
		XINPUT_GAMEPAD_DPAD_RIGHT, 
		XINPUT_GAMEPAD_LEFT_SHOULDER, 
		XINPUT_GAMEPAD_RIGHT_SHOULDER, 
		XINPUT_GAMEPAD_LEFT_THUMB, 
		XINPUT_GAMEPAD_RIGHT_THUMB, 
		XINPUT_GAMEPAD_START, 
		XINPUT_GAMEPAD_BACK
	};

//xinput button IDs
struct xButtID{
	//reg buttons
	int A, B, X, Y;
	//dpad
	struct dpad{
		int up, down, left, right;
	}dPad;
	//shoulder buttons
	int lShoulder, rShoulder;
	//thumbstick buttons
	int lThumb, rThumb;
	int start;	//start button
	int back;	//back button
	xButtID(); //default constructor
};
class gamePad{
private: 
	XINPUT_STATE state;				//the current state of the gamepad
	int gpIndex;					//the gamePad index (1, 2, 3, 4)
	static const int  bCount = 14;	//total number of buttons
	bool prevButtStates[bCount];	//previous frame button states
	bool buttStates[bCount];		//current frame button states
	bool gpButtDown[bCount];		//buttons pressed on current frame
public:
	//default constructors
	gamePad();
	gamePad(int a_Index);

	//update  & refresh function
	void update();
	void refreshState();

	//thumbstick functions
	bool lStickInDZ(); //return if the left thumbstick is in the deadzone
	bool rStickInDZ(); //return if the right thumbstick is in the deadzone
	float lStickX(); //return x axis of left thumbstick
	float lStickY(); //return y axis of left thumbstick
	float rStickX(); //return x axis of right thumbstick
	float rStickY(); //return y axis of right thumbstick

	//Trigger functions
	float lTrigger();
	float rTrigger();

	//vibration functions
	void rumble(float a_Lm = 0.0f, float a_Rm = 0.0f); //initialise variables so method can be called without passing in values

	//button support
	bool bPress(int a_Butt);
	//	frame specific version of above function
	bool bDown(int a_Butt);

	//utility functions
	XINPUT_STATE getState();
	int getIndex();
	bool connected();
};
#endif
extern xButtID xButtons;