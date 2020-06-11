#ifndef DJTHING_TRACKSELECTSCREEN_H
#define DJTHING_TRACKSELECTSCREEN_H

#include <Arduino.h>
#include "pots.h"
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
#include "pots.h"
#include <SD.h>
#include "DjScreen.h"

class TrackSelectScreen : public Context
{
public:
	TrackSelectScreen(Display& display, DjScreen* _selectedScreen);
	void start() override;
	void stop() override;
	void draw() override;

	static void channelASelect();
	static void channelBSelect();
	static void btnRPress();
	static void btnLPress();


private:
	static TrackSelectScreen* instance;
	void listAudio(const char *dirname);
	void setUI();
	uint32_t audioCount = 0;
	ListMenu menu;
	char audioFiles[50][25];
	DjScreen* selectedScreen = nullptr;

};






#endif