///		***
///
///				sound.h - declaration of the sound class, variables and methods - Tom
///
///		***

#ifndef SOUND_H
#define SOUND_H

#include <vector>
#include "include\fmod\inc\fmod.hpp"
#include "include\fmod\inc\fmod_errors.h"
#include "ppf.h"

class sound{
private:
	FMOD::System *system;
	FMOD_RESULT result;
	size_t version;
	int numDrivers;
	FMOD_SPEAKERMODE speakerMode;
	FMOD_CAPS caps;
	float wFreq; //frequency can be between 100 to 705600, -100 to -705600
	char name[256];
public:
	sound();
	~sound();
	void init();
	void update();
	FMOD::Sound *walk, *amb, *enem;
	FMOD::Channel *amb1, *walk1, *enem1;
	vector<FMOD::Channel*> cList;	
	void loadSounds();
	void setFreq(int id, float f);
	////loading, playing, and manipulating the soundsvoid pSoundAStreamConf(const char *n, int loop, bool pause, float volume); //overloaded method as above but with options
	//setting sound properties
	void setLoop(int id, int l);			//set whether the channel's sound should loop
	void setPause(int id, bool p);		//set whether the channel's sound should pause
	void setSoundVol(int id, float v);	//set whether the channel's sound volume

	//freeing memory from sounds that won't be used again
	void freeSoundMem();

	//error checking
	void fmodErrorCheck(FMOD_RESULT fr);
};
#endif