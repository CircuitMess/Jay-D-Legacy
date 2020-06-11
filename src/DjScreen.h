#ifndef DJTHING_DJSCREEN_H
#define DJTHING_DJSCREEN_H

#define VU_THRESHLO 30
#define VU_THRESHHI 100
#include <CircuitOS.h>
#include <UI/Image.h>
#include <UI/Screen.h>
#include <CircuitOS.h>
#include <Util/Vector.h>
#include <UI/LinearLayout.h>
#include <UI/GridLayout.h>
#include <UI/ScrollLayout.h>
#include <Util/Task.h>
#include <Input/Input.h>
#include <Bitmaps/Bitmaps.hpp>
#include <Elements/ListMenu.h>
#include <Input/InputGPIO.h>
#include <Support/Context.h>
#include <MPWavLib.h>
#include <MPAudioDriver.h>
#include <Sync/Mutex.h>
#include "pots.h"
#include <NeoPixelBrightnessBus.h>

class DjScreen : public Context, public UpdateListener
{
public:
	DjScreen(Display& display, Mutex* _trackMutex, Mutex* _vuMutex, NeoPixelBrightnessBus<NeoGrbFeature, Neo800KbpsMethod>* strip);

	void start() override;
	void stop() override;
	void draw() override;
	static void loadFiles(char* trackA = nullptr, char* trackB = nullptr);
	void update(uint32_t time) override;

private:
	

	static void startA();
	static void startB();
	static void stopA();
	static void stopB();
	static void pauseA();
	static void pauseB();
	static void setSpeedA(float speed);
	static void setSpeedB(float speed);
	static void setVolumeA(float volume);
	static void setVolumeB(float volume);
	static void setFader(float fade);
	static void faderCallback(float fade);
	static void backToSelect();
	static void setFilterA(float val);
	static void setFilterB(float val);
	static void setEchoA(float val);
	static void setEchoB(float val);
	static void refreshVu();
	static void fxToggleA();
	static void fxToggleB();
	void setUI();

	static DjScreen* instance;
	MPTrack *trackA, *trackB;
	float speedA = 0.5;
	float speedB = 0.5;
	char pathA[25] = {0};
	char pathB[25] = {0};
	float volumeA = 20.0;
	float volumeB = 20.0;
	float faderValue = 0.5;
	uint32_t secondTimer = 0;
	uint32_t vuRefreshTimer = 0;
	Mutex* trackMutex;
	Mutex* vuMutex;
	Image baseImage;
	Mutex screenMutex;
	Mutex faderMutex;
	Mutex pixelsMutex;
	NeoPixelBrightnessBus<NeoGrbFeature, Neo800KbpsMethod>* strip;
	bool fxOnA = 1;
	bool fxOnB = 1;
	float filterA = 0.0;
	float filterB = 0.0;
	float echoA = 0.0;
	float echoB = 0.0;

	
};




#endif