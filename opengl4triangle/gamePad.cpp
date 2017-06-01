///		***
///
///		gamePad.cpp - implementation of the gamePad class - Tom
///		This class serves as the interface between the application and the xInput library. The class
///		implements a listener to check for changes in the controllers' inputs. 
///
///		***

#include "gamePad.h"
#pragma comment(lib, "Xinput.lib")

xButtID xButtons;
/*********************************************************************

					default constructors	

**********************************************************************/

gamePad::gamePad()
{

}

gamePad::gamePad(int a_Index)
{
	gpIndex = a_Index - 1;
	//iterate through all buttons and set values to false
	for (int i = 0; i < bCount ; i++)
	{
		buttStates[i]		= false;
		prevButtStates[i]	= false;
		gpButtDown[i]		= false;
	}
}

/*********************************************************************

					Getters, status, update

**********************************************************************/

XINPUT_STATE gamePad::getState()
{
	//create a temp state to return
	XINPUT_STATE tempState;
	//zero mem	
	ZeroMemory(&tempState, sizeof(XINPUT_STATE));
	//get the state of the game pad
	XInputGetState(gpIndex, &tempState);
	//finally, return the state
	return tempState;
}

int gamePad::getIndex()
{
	//return the game pad index
	return gpIndex;
}

bool gamePad::connected()
{
	//zero memory
	ZeroMemory(&state, sizeof(XINPUT_STATE));
	//get the state of the gamepad
	DWORD r = XInputGetState(gpIndex, &state);
	//if success return true, else return false
	if(r==ERROR_SUCCESS)
	{
		return true;
	}
	else {return false;}
}

void gamePad::update()
{
	state = getState();
	//iterate through all buttons 
	for (int i = 0; i < bCount ; i++)
	{
		//set button state for current frame
		buttStates[i] = (state.Gamepad.wButtons & xButt[i]) == xButt[i];
		//set 'pressed' state for current frame
		gpButtDown[i] = !prevButtStates[i] && buttStates[i];
	}
}

void gamePad::refreshState()
{
	memcpy(prevButtStates, buttStates, sizeof(prevButtStates));
}

/*********************************************************************

						thumbstick functions

**********************************************************************/

bool gamePad::lStickInDZ()
{
	//obtain x & y axes of the stick
	short sX = state.Gamepad.sThumbLX;
	short sY = state.Gamepad.sThumbLY;
	if(	sX > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE ||
		sY > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE ||
		sX < XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE ||
		sY < XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE )
	{
		return false; 
	}
	return true;
}

bool gamePad::rStickInDZ()
{
	//obtain x & y axes of the stick
	short sX = state.Gamepad.sThumbRX;
	short sY = state.Gamepad.sThumbRY;
	if(	sX > XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE ||
		sY > XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE ||
		sX < XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE ||
		sY < XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE )
	{
		return false; 
	}
	return true;
}

float gamePad::lStickX()
{
	//obtain the x axis
	short sX = state.Gamepad.sThumbLX;
	//convert to a float
	float fX = (static_cast<float>(sX) / 32768.0f);
	return fX;
}

float gamePad::lStickY()
{
	//obtain the y axis
	short sY = state.Gamepad.sThumbLY;
	//convert to a float
	float fX = (static_cast<float>(sY) / 32768.0f);
	return fX;
}

float gamePad::rStickX()
{
	//obtain the x axis
	short sX = state.Gamepad.sThumbRX;
	//convert to a float
	float fX = (static_cast<float>(sX) / 32768.0f);
	return fX;
}

float gamePad::rStickY()
{
	//obtain the y axis
	short sY = state.Gamepad.sThumbRY;
	//convert to a float
	float fX = (static_cast<float>(sY) / 32768.0f);
	return fX;
}

/*********************************************************************

						trigger functions 
		return value > 0.0 && < 1.0 dependent on depression(ha)

**********************************************************************/

float gamePad::lTrigger()
{
	//get values for a trigger
	BYTE t = state.Gamepad.bLeftTrigger;
	if(t > XINPUT_GAMEPAD_TRIGGER_THRESHOLD)
	{
		return t / 255.0f;
	}
	else { return 0.0f;} //trigger was not pressed
}

float gamePad::rTrigger()
{
	//get values for a trigger
	BYTE t = state.Gamepad.bRightTrigger;
	if(t > XINPUT_GAMEPAD_TRIGGER_THRESHOLD)
	{
		return t / 255.0f;
	}
	else { return 0.0f;} //trigger was not pressed
}

/*********************************************************************

						rumble function

**********************************************************************/

void gamePad::rumble(float a_Lm, float a_Rm)
{
	//create a vibration state
	XINPUT_VIBRATION vState;
	//zero memory
	ZeroMemory(&vState, sizeof(XINPUT_VIBRATION));
	//calculate the vibration values, multiply by large value to get in a range of 0 > 1
	int iLM = int(a_Lm * 65535.0f);
	int iRM = int(a_Rm * 65535.0f);
	//set the vibration values
	vState.wLeftMotorSpeed	= iLM;
	vState.wRightMotorSpeed = iRM;
	//finally, set the vibration state
	XInputSetState(gpIndex, &vState);
}

/*********************************************************************

						button tings

**********************************************************************/

xButtID::xButtID()
{
	//set all dem IDs
	A			= 0; 
	B			= 1; 
	X			= 2;
	Y			= 3;
	dPad.up		= 4; 
	dPad.down	= 5;
	dPad.left	= 6; 
	dPad.right	= 7;
	lShoulder	= 8;
	rShoulder	= 9;
	start		= 10;
	back		= 11;
	lThumb		= 12;
	rThumb		= 13;
}

bool gamePad::bPress(int a_Butt)
{
	//obtain the button value from state and check it against a specified button.
	if(state.Gamepad.wButtons & xButt[a_Butt])
	{
		return true;
	}
	else{return false;}
}

bool gamePad::bDown(int a_Butt)
{
	return gpButtDown[a_Butt];
}