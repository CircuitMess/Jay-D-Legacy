#include "TrackSelectScreen.h"

TrackSelectScreen* TrackSelectScreen::instance = nullptr;

TrackSelectScreen::TrackSelectScreen(Display& display, DjScreen* _selectedScreen) :
				Context(display),
				menu(&screen, String("ListMenu")),
				selectedScreen(_selectedScreen)
{
	if (!SD.begin(5, SPI, 9000000))
	{
		Serial.println("SD ERROR");
		return;
	}
	instance = this;
	listAudio("/");
	setUI();
	pack();
}

void TrackSelectScreen::start()
{
	setEncoderChangedEvent(1, btnLPress);
	setEncoderChangedEvent(0, btnRPress);
	Input::getInstance()->setBtnReleaseCallback(BTN_LOAD_LEFT, channelASelect);
	Input::getInstance()->setBtnReleaseCallback(BTN_LOAD_RIGHT, channelBSelect);
	instance->menu.setSelected(0);
	draw();
}

void TrackSelectScreen::stop()
{
	setEncoderChangedEvent(1, nullptr);
	setEncoderChangedEvent(0, nullptr);
	Input::getInstance()->removeBtnReleaseCallback(BTN_LOAD_LEFT);
	Input::getInstance()->removeBtnReleaseCallback(BTN_LOAD_RIGHT);
	instance->menu.setSelected(0);
}
void TrackSelectScreen::channelASelect()
{
	if(instance == nullptr) return;
	Serial.println("A Select");
	Serial.println(instance->audioFiles[instance->menu.getSelected()]);
	instance->selectedScreen->loadFiles(instance->audioFiles[instance->menu.getSelected()]);
	instance->selectedScreen->push(instance);
}

void TrackSelectScreen::channelBSelect()
{
	
	if(instance == nullptr) return;
	Serial.println(instance->audioFiles[instance->menu.getSelected()]);
	instance->selectedScreen->loadFiles(nullptr, instance->audioFiles[instance->menu.getSelected()]);
	instance->selectedScreen->push(instance);
}
void TrackSelectScreen::btnRPress()
{
	if(instance == nullptr) return;
	instance->menu.selectNext();
	instance->screen.commit();
	// instance->draw();
}

void TrackSelectScreen::btnLPress()
{
	if(instance == nullptr) return;
	instance->menu.selectPrev();
	instance->screen.commit();
	// instance->draw();
}

void TrackSelectScreen::setUI()
{
	// display.clear(TFT_GREEN);
	//menu.getSelected();
	menu.setWHType(PARENT, PARENT);
	for (int i = 0; i < audioCount; i++)
	{
		int len = strnlen(audioFiles[i], 50);
		char* title = (char*) malloc(len+1);
		memset(title, 0, len+1);
		memcpy(title, audioFiles[i], len);
		title[len-4] = 0;
		menu.addItem(title);
	}
	menu.setTitleColor(TFT_RED, TFT_BLACK);
	menu.reflow();
	screen.addChild(&menu);
	screen.repos();
}

void TrackSelectScreen::listAudio(const char *dirname)
{
	audioCount = 0;
	while (!SD.begin(5, SPI, 8000000))
		Serial.println(F("SD ERROR"));
	Serial.println("OK");
	delay(5);
	File root = SD.open(dirname);
	if (!root)
	{
		Serial.println(F("Failed to open directory"));
		delay(5);
		return;
	}
	if (!root.isDirectory())
	{
		Serial.println(F("Not a directory"));
		return;
	}

	int counter = 1;
	File file = root.openNextFile();
	//char temp[5][20];
	while (file)
	{
		String Name(file.name());
		// Serial.println(Name);
		// delay(5);

		if (Name.endsWith(F(".wav")) || Name.endsWith(F(".WAV")))
		{
			String help;
			uint8_t delimNum = 1;
			uint8_t oldDelimNum = 1;
			while (delimNum > 0)
			{
				delimNum = Name.indexOf("/", delimNum) + 1;
				help = Name.substring(oldDelimNum);
				oldDelimNum = delimNum;
			}
			Name = help;
			strncpy(audioFiles[counter -1], Name.c_str(), 25);
			audioCount++;
			counter++;
			// Serial.println(audioFiles[audioCount - 1]);
			// Serial.println(audioCount);
		}
		file = root.openNextFile();
	}
	root.close();
	file.close();
}
void TrackSelectScreen::draw()
{
	// screen.clear();
	// menu.draw();
	screen.draw();
	screen.commit();
	// Serial.println("draw");
}