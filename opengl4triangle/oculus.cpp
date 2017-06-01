///		***
///
///		oculus.cpp - implementation of the gamePad class - Tom
///		This class serves as the interface between the application and the Oculus Rift SDK. Pointers to various
///		different Oculus rift objects are created so that the application can access the data for the Rifts' sensors
///		and head mounted display. 
///
///		***

#include "oculus.h"

oculus::oculus(): renderMode(STEREO), useTracker(false), elapsed(0){
	
}

oculus::~oculus()
{
	OVR::MessageHandler::RemoveHandlerFromDevices();
	ovrSensor.Clear();
	ovrHMD.Clear();
	ovrManager.Clear();
	OVR::System::Destroy();
	OVR_DEBUG_STATEMENT(_CrtDumpMemoryLeaks());
}


void oculus::init(){
	// *** Init basic values for all the tings
	eyeYaw = yawInit;
	eyePitch = 0;
	eyeRoll = 0;
	lastSensorYaw = 0;
	eyePos = OVR::Vector3f(0.0f, standingHeight, -5.0f);
	viewAngleDeg = 45.0f; //for the no HMD case
	sFusion.SetGravityEnabled(false);
	sFusion.SetPredictionEnabled(false);
	sFusion.SetYawCorrectionEnabled(false);
	hmdInfo.HResolution				= 1280;
	hmdInfo.VResolution				= 800;
	hmdInfo.HScreenSize				= 0.149759993f;
	hmdInfo.VScreenSize				= 0.0935999975f;
	hmdInfo.VScreenCenter			= 0.0467999987f;
	hmdInfo.EyeToScreenDistance		= 0.0410000011f;
	hmdInfo.LensSeparationDistance	= 0.0635000020f;
	hmdInfo.InterpupillaryDistance	= 0.0640000030f;
	hmdInfo.DistortionK[0]			= 1.000000000f;
	hmdInfo.DistortionK[1]			= 0.219999999f;
	hmdInfo.DistortionK[2]			= 0.239999995f;
	hmdInfo.DistortionK[3]			= 0.000000000f;
	hmdInfo.ChromaAbCorrection[0]	= 0.995999992f;
	hmdInfo.ChromaAbCorrection[1]	= -0.00400000019f;
	hmdInfo.ChromaAbCorrection[2]	= 1.01400006f;
	hmdInfo.ChromaAbCorrection[3]	= 0.000000000f;
	hmdInfo.DesktopX = 0;
	hmdInfo.DesktopY = 0;


	//Oculus HMD & Sensor init
	ovrManager = *DeviceManager::Create();
	//handle dems messages
	ovrManager->SetMessageHandler(this);
	
	int			detectionResult = IDCONTINUE;
	const char* detectionMessage;
	
	//release the sensor & HMD in case this is a retry
	ovrSensor.Clear();
	ovrHMD.Clear();
	if(ovrManager)
		{
			ovrHMD = *ovrManager->EnumerateDevices<HMDDevice>().CreateDevice();
			if(ovrHMD)
			{
				ovrSensor = ovrHMD->GetSensor();
				//this will initialise ovrHMD with information about configured IPD, screen size
				//and other variable needed for correct projection. We pass HMD DisplayDeviceName into the renderer
				//to select the correct monitor in full screen mode
				ovrHMD->GetDeviceInfo(&hmdInfo);
				// *** configure stereo settings yo
				sConfig.SetHMDInfo(hmdInfo);
				sConfig.SetFullViewport(OVR::Util::Render::Viewport(0, 0, hmdInfo.HResolution, hmdInfo.VResolution));
				sConfig.SetStereoMode(OVR::Util::Render::StereoMode::Stereo_LeftRight_Multipass);
				sFusion.AttachToSensor(ovrSensor);

				// Configure proper Distortion Fit.
				// For 7" screen, fit to touch left side of the view, leaving a bit of invisible
				// screen on the top (saves on rendering cost).
				// For smaller screens (5.5"), fit to the top.
				if(hmdInfo.HScreenSize > 0.140f) //7 inches of glory
					sConfig.SetDistortionFitPointVP(-1.0f, 0.0f);
				else 
					sConfig.SetDistortionFitPointVP(0.0f, 1.0f);
			}
			else{
				//if we detected no HMD, create sensor directly, useful for debugging sensor interaction
				ovrSensor = *ovrManager->EnumerateDevices<SensorDevice>().CreateDevice();
			}
				
		}
	detectionResult = IDCONTINUE;
	//print out errors if there are any
	if(!ovrHMD && !ovrSensor)
	{
		detectionMessage = "Oculus Rift Not Detected!";
	}
	else if(!ovrHMD)
	{
		detectionMessage = "Oculus Sensor Detected! HMD Display not detected";
	}
	else if(!ovrSensor)
	{
		detectionMessage = "Oculus HMD Display Detected! Sensor not detected";
	}
	else if(hmdInfo.DisplayDeviceName[0] == '\0')
	{
		detectionMessage = "Oculus Sensor Detected! HMD Display EDID not detected";
	}
	else 
		detectionMessage = 0;

	if(detectionMessage)
	{
		String messageText(detectionMessage);
		messageText +=	"\n\n"
						"Press Cancel to exit the application!\n"
						"Press Ok to Continue anyway!";
		detectionResult = ::MessageBox(0, messageText.ToCStr(), "Oculus Rift Detection, ERROR", MB_OKCANCEL|MB_ICONWARNING);
		if(detectionResult==IDCANCEL)
		{
			glfwTerminate();
			exit(0);
		}
		if(detectionResult==IDOK)
		{
			detectionMessage = GL_FALSE;
		}
	}

	//If the horizontal resolution is  > 0 set the height and width variables
	if(hmdInfo.HResolution > 0)
	{
		r_height = hmdInfo.VResolution;
		r_width = hmdInfo.HResolution;
	}

	//If we detect the sensor then initialise and attach the sensorFusion object 
	if(ovrSensor)
	{
		sFusion.AttachToSensor(ovrSensor);
		sFusion.SetDelegateMessageHandler(this);
		sFusion.SetPredictionEnabled(true);
	}
	sConfig.Set2DAreaFov(DegreeToRad(85.0f));
	eyeWidth	= hmdInfo.HResolution/2;
	eyeHeight	= hmdInfo.VResolution;
	fboWidth	= eyeWidth * FRAMEBUFFER_OBJECT_SCALE;
	fboHeight	= eyeHeight * FRAMEBUFFER_OBJECT_SCALE;
}

//Return the sensor orientation so we can update the camera accordingly 
bool oculus::getSensorOrientation(float& eyeYaw, float& eyePitch, float& eyeRoll) const
{
	eyeYaw = eyePitch = eyeRoll = 0.0f; 
	if(!sFusion.IsAttachedToSensor())
		return false;
	
	OVR::Quatf hmdOrient = sFusion.GetOrientation();
	hmdOrient.GetEulerAngles<OVR::Axis_Y, OVR::Axis_X, 	OVR::Axis_Z>(&eyeYaw, &eyePitch, &eyeRoll);
	return true;
}


//return the HMDInfo object 
const OVR::HMDInfo& oculus::getHMDInfo() const
{
	return hmdInfo;
}


// Return the Stereo Config object
OVR::Util::Render::StereoConfig oculus::getStereoConfig() const
{
	return sConfig;
}


//If for some reason the Rift becomes disconnected then print out an error message to the console
void oculus::onMessage(const OVR::Message& msg)
{
	if(msg.pDevice == ovrSensor)
	{
		if(msg.Type == Message_BodyFrame)
		{

		}
	}
	else if(msg.pDevice == ovrManager)
	{
		if(msg.Type == Message_DeviceRemoved)
		{
			printf("DeviceManager reported device removal\n");
		}
		else if(msg.Type == Message_DeviceAdded)
		{
			printf("DeviceManager reported device added\n");
		}

	}
	else if (msg.pDevice == ovrSensor)
	{
		if(msg.Type == Message_DeviceRemoved)
		{
			printf("Sensor reported device removal\n");
		}
		else if(msg.Type == Message_DeviceAdded)
		{
			printf("Sensor reported device added\n");
		}
	}

}

