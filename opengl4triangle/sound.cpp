///		***
///
///		sound.cpp - implementation of the sound class - Tom
///		This class serves as an interface between the application and the FMOD sound library. A number of 
///		pointers to FMOD objects are created so that sounds can be easily loaded and played back in the application.
///
///		***

#include "sound.h"
#pragma comment (lib, "fmodex_vc.lib")

sound::sound()
{	//set any tings you need here
	wFreq = 60000.0f;
	//first get a pointer to FMOD::System and error check it
	result  = FMOD::System_Create(&system);
	fmodErrorCheck(result);
	//Next check the version of the DLL is the same as the libs we are using
	result = system->getVersion(&version);
	fmodErrorCheck(result);
	if(version < FMOD_VERSION)
	{
		std::cout<<"Error, you are using old FMOD: "<<version<<". You need this version: "<<FMOD_VERSION<<"."<<std::endl;
	}
	//get number of sound cards
	result = system->getNumDrivers(&numDrivers);
	fmodErrorCheck(result);
	//if no sound cards, disable sound
	if(numDrivers==0)
	{
		result = system->setOutput(FMOD_OUTPUTTYPE_NOSOUND);
		fmodErrorCheck(result);
	}else //at least one sound card
		{
			//get the capabilities of the default sound card (0)
			result=system->getDriverCaps(0, &caps, 0, &speakerMode);
			fmodErrorCheck(result);
			//set the speaker mode to match thac currently selected in control panel
			result = system->setSpeakerMode(speakerMode);
			fmodErrorCheck(result);
		}
	//in case hardware acceleration is dsiabled the software buffer must be increased to prevent skipping/stuttering
	//first param specifies number of samples, second specifies number of buffers(used in a ring)
	//incresae buffer size if user has accelearaion slider set to off
	if(caps & FMOD_CAPS_HARDWARE_EMULATED)
	{
		result = system->setDSPBufferSize(1024, 10);
		fmodErrorCheck(result);
	}

	//a little extra error prevention for "SigmaTel" drivers. if the check is true then output format is changed
	//to PCM floating point to prevent crackling if PCM 16bit is used, if not errything is left as is

	//get name of driver
	result = system->getDriverInfo(0, name, 256, 0);
	fmodErrorCheck(result);
	if(strstr(name, "SigmaTel"))
	{
		result = system->setSoftwareFormat(48800, FMOD_SOUND_FORMAT_PCMFLOAT, 0, 0, FMOD_DSP_RESAMPLER_LINEAR);
		fmodErrorCheck(result);
	}
}

sound::~sound()
{
	system->release();
}


void sound::init()
{
	result = system->init(100, FMOD_INIT_NORMAL, 0);
	//if the speaker mode isn't supported by current sound card then...
	if(result == FMOD_ERR_OUTPUT_CREATEBUFFER)
	{ 
		result = system->setSpeakerMode(FMOD_SPEAKERMODE_STEREO);
		fmodErrorCheck(result);
		result = system->init(100, FMOD_INIT_NORMAL, 0);
	}
	fmodErrorCheck(result);
}

void sound::update()
{
	system->update();
}

/***********************************************************************************************
		
					Loading, playing and manipulating the sounds

***********************************************************************************************/
void sound::loadSounds()
{
	//create the music tings
	result = system->createStream("sound/creepy_ambience.wav", FMOD_DEFAULT, 0, &amb);
	fmodErrorCheck(result);
	
	//create the effect tings
	result = system->createStream("sound/footsteps_wood_looped.mp3", FMOD_DEFAULT, 0, &walk);
	fmodErrorCheck(result);

	result = system->createStream("sound/whispers.mp3", FMOD_DEFAULT, 0, &enem);
	fmodErrorCheck(result);
	
	//set the channel settings
	result = system->playSound(FMOD_CHANNEL_FREE, amb, true, &amb1);
	fmodErrorCheck(result);
	//cList.push_back(amb1); //pos 0
	result = system->playSound(FMOD_CHANNEL_FREE, walk, true, &walk1);
	fmodErrorCheck(result);
	//cList.push_back(walk1); //pos 1
	result = system->playSound(FMOD_CHANNEL_FREE, enem, true, &enem1);
	fmodErrorCheck(result);

	//set effects channel tings
	walk1->setMode(FMOD_LOOP_NORMAL);
	walk1->setLoopCount(-1);
	walk1->setVolume(0.95f);
	walk1->setFrequency(wFreq);
	walk1->setPosition(0, FMOD_TIMEUNIT_MS); //flush the buffer to ensure looping is bueno

	//set music channel tings
	amb1->setLoopCount(-1);
	amb1->setVolume(0.15f);
	
}
/***********************************************************************************************
		
								Set dems sound properties

***********************************************************************************************/


void sound::setLoop(int id, int l)
{
	cList.at(id)->setMode(FMOD_LOOP_NORMAL);	
	cList.at(id)->setLoopCount(l);
}

void sound::setPause(int id, bool p)
{
	cList.at(id)->getPaused(&p);
	cList.at(id)->setPaused(!p);
}

void sound::setSoundVol(int id, float v)
{
	cList.at(id)->setVolume(v);
}

void sound::setFreq(int id, float f)
{
	wFreq = f;
	walk1->setFrequency(wFreq);
}


/***********************************************************************************************
		
									Error checking 

***********************************************************************************************/
void sound::fmodErrorCheck(FMOD_RESULT fr)
{
	if (fr != FMOD_OK)
	{
		std::cout << "FMOD ERROR YO (" << fr << ")" <<FMOD_ErrorString(fr) <<std::endl;
	}
}
