#include "DjScreen.h"
#include "sprites.h"
#include <Update/UpdateManager.h>
DjScreen* DjScreen::instance = nullptr;
DjScreen::DjScreen(Display& display, Mutex* _trackMutex, Mutex* _vuMutex, NeoPixelBrightnessBus<NeoGrbFeature, Neo800KbpsMethod>* _strip) :
				Context(display),
				trackA(new MPTrack("")),
				trackB(new MPTrack("")),
				trackMutex(_trackMutex),
				vuMutex(_vuMutex),
				baseImage(&screen, screen.getWidth(), screen.getHeight()),
				strip(_strip)
{
	instance = this;
	pathA[0] = 0;
	pathB[0] = 0;
	addSprite(&baseImage);
	screen.addChild(&baseImage);
	baseImage.setPos(0,0);
	pack();
}

void DjScreen::start()
{
	if(pathA[0] != 0)
	{
		trackA->setRepeat(0);
		addTrack(trackA);
		trackA->rewind();
	}
	if(pathB[0] != 0)
	{
		trackB->setRepeat(0);
		addTrack(trackB);
		trackB->rewind();
	}
	setSpeedA(getPotValue(POT_BPM_LEFT));
	setSpeedB(getPotValue(POT_BPM_RIGHT));
	setPotChangedEvent(POT_BPM_LEFT, setSpeedA);
	setPotChangedEvent(POT_BPM_RIGHT, setSpeedB);
	setVolumeA(getPotValue(POT_VOLUME_LEFT));
	setVolumeB(getPotValue(POT_VOLUME_RIGHT));
	setPotChangedEvent(POT_VOLUME_LEFT, setVolumeA);
	setPotChangedEvent(POT_VOLUME_RIGHT, setVolumeB);
	setFader(getPotValue(POT_FADE));
	setPotChangedEvent(POT_FADE, faderCallback);
	setFilterA(getPotValue(POT_FX1_LEFT));
	setPotChangedEvent(POT_FX1_LEFT, setFilterA);
	setFilterB(getPotValue(POT_FX1_RIGHT));
	setPotChangedEvent(POT_FX1_RIGHT, setFilterB);
	setEchoA(getPotValue(POT_FX2_LEFT));
	setPotChangedEvent(POT_FX2_LEFT, setEchoA);
	setEchoB(getPotValue(POT_FX2_RIGHT));
	setPotChangedEvent(POT_FX2_RIGHT, setEchoB);
	// setVuCallback(refreshVu);

	Input::getInstance()->setBtnReleaseCallback(BTN_PLAY_LEFT, startA);
	Input::getInstance()->setBtnReleaseCallback(BTN_PLAY_RIGHT, startB);
	Input::getInstance()->setBtnReleaseCallback(BTN_LOAD_RIGHT, backToSelect);
	Input::getInstance()->setBtnReleaseCallback(BTN_LOAD_LEFT, backToSelect);
	Input::getInstance()->setBtnReleaseCallback(BTN_FX_LEFT, fxToggleA);
	Input::getInstance()->setBtnReleaseCallback(BTN_FX_RIGHT, fxToggleB);


	setEncoderChangedEvent(0, backToSelect);
	setEncoderChangedEvent(1, backToSelect);
	
	// setPotChangedEvent(3, setFader);
	// setPotChangedEvent(1, setFader);
	// setPotChangedEvent(3, setFader);
	UpdateManager::addListener(this);
	draw();
}

void DjScreen::stop()
{
	trackA->pause();
	trackB->pause();
	removeTrack(trackA);
	removeTrack(trackB);
	Input::getInstance()->removeBtnReleaseCallback(BTN_PLAY_LEFT);
	Input::getInstance()->removeBtnReleaseCallback(BTN_PLAY_RIGHT);
	Input::getInstance()->removeBtnReleaseCallback(BTN_LOAD_RIGHT);
	Input::getInstance()->removeBtnReleaseCallback(BTN_LOAD_LEFT);
	Input::getInstance()->removeBtnReleaseCallback(BTN_FX_LEFT);
	Input::getInstance()->removeBtnReleaseCallback(BTN_FX_RIGHT);
	setPotChangedEvent(POT_BPM_LEFT, nullptr);
	setPotChangedEvent(POT_BPM_RIGHT, nullptr);
	setPotChangedEvent(POT_VOLUME_LEFT, nullptr);
	setPotChangedEvent(POT_VOLUME_RIGHT, nullptr);
	setPotChangedEvent(POT_FADE, nullptr);
	setPotChangedEvent(POT_FX1_LEFT, nullptr);
	setPotChangedEvent(POT_FX1_RIGHT, nullptr);
	setPotChangedEvent(POT_FX2_RIGHT, nullptr);
	setPotChangedEvent(POT_FX2_LEFT, nullptr);
	// setPotChangedEvent(3, nullptr);
	// setPotChangedEvent(1, nullptr);
	// setPotChangedEvent(3, nullptr);
	UpdateManager::removeListener(this);
}

void DjScreen::startA()
{
	instance->trackMutex->lock();
	if(!instance->trackA->isPlaying())
	{
		instance->trackA->play();
	}
	Input::getInstance()->setBtnReleaseCallback(BTN_PLAY_LEFT, pauseA);
	instance->trackMutex->unlock();
}

void DjScreen::startB()
{
	instance->trackMutex->lock();
	instance->trackB->play();
	Input::getInstance()->setBtnReleaseCallback(BTN_PLAY_RIGHT, pauseB);
	instance->trackMutex->unlock();
}

void DjScreen::stopA()
{
	instance->trackMutex->lock();
	instance->trackA->stop();
	instance->trackMutex->unlock();
}
void DjScreen::pauseA()
{
	instance->trackMutex->lock();
	instance->trackA->pause();
	Input::getInstance()->setBtnReleaseCallback(BTN_PLAY_LEFT, startA);
	instance->trackMutex->unlock();
}

void DjScreen::pauseB()
{
	instance->trackMutex->lock();
	instance->trackB->pause();
	Input::getInstance()->setBtnReleaseCallback(BTN_PLAY_RIGHT, startB);
	instance->trackMutex->unlock();
}

void DjScreen::stopB()
{
	instance->trackMutex->lock();
	instance->trackB->stop();
	instance->trackMutex->unlock();
}

void DjScreen::setSpeedA(float speed)
{
	instance->trackMutex->lock();
	instance->trackA->setSpeed(speed + 0.5);
	instance->trackMutex->unlock();
	instance->screenMutex.lock();
	instance->speedA=((speed*2.0) - 1.0);
	instance->draw();
	instance->screenMutex.unlock();
}

void DjScreen::setSpeedB(float speed)
{
	instance->trackMutex->lock();
	instance->trackB->setSpeed(speed + 0.5);
	instance->trackMutex->unlock();
	instance->screenMutex.lock();
	instance->speedB=((speed*2.0) - 1.0);
	instance->draw();
	instance->screenMutex.unlock();
}

void DjScreen::loadFiles(char* _pathA, char* _pathB)
{
	if(_pathA != nullptr)
	{
		if(strcmp(_pathA, instance->pathA) != 0)
		{
		
			strncpy(instance->pathA, _pathA, 25);
			char temp[50];
			strncpy(temp, "/", 50);
			// strncpy(temp, "/Music/", 50);
			strncat(temp, _pathA, 50);
			instance->trackMutex->lock();
			// instance->trackA->closeFile();
			instance->trackA->stop();
			instance->trackA->closeFile();
			instance->trackA->reloadFile(temp);
			instance->trackA->openFile();
			instance->trackA->setRepeat(0);
			instance->trackA->setVolume(40);
			instance->trackA->rewind();
			instance->trackMutex->unlock();
		}
	}
	if(_pathB != nullptr)
	{
		if(strcmp(_pathB, instance->pathB) != 0)
		{
			strncpy(instance->pathB, _pathB, 25);
			char temp[50];
			// strncpy(temp, "/Music/", 50);
			strncpy(temp, "/", 50);
			strncat(temp, _pathB, 50);
			
			instance->trackMutex->lock();
			// instance->trackB->closeFile();
			instance->trackB->stop();
			instance->trackB->closeFile();
			instance->trackB->reloadFile(temp);
			instance->trackB->openFile();
			instance->trackB->setRepeat(0);
			instance->trackB->setVolume(40);
			instance->trackB->rewind();
			instance->trackMutex->unlock();
		}
	}
}

void DjScreen::draw()
{
	Sprite *spriteScreen = screen.getSprite();
	spriteScreen->clear(TFT_RED);
	spriteScreen->fillRect(0, 0, 160, 66, 0xD082);
	spriteScreen->fillRect(0, 61, 160, 2, 0x10A7);
	spriteScreen->drawIcon(DJIcon, 64, 28, 32, 32, 1);
	spriteScreen->setCursor(100, 40);
	spriteScreen->setTextColor(TFT_BLACK);
	spriteScreen->setTextFont(1);
	spriteScreen->setTextSize(1);
	if (speedA >= -0.05 && speedA <= 0.05){
		spriteScreen->print("+0.0%");
	}
	else if(speedA < -0.99){
		spriteScreen->print("-100.0%");
	}
	else if(speedA > 0.99){
		spriteScreen->print("+100.0%");
	}
	else{
		spriteScreen->print(speedA * 100);
		spriteScreen->print("%");
	}
	
	spriteScreen->fillRoundRect(6, 2, 148, 16, 2, 0x10A7);
	spriteScreen->setTextColor(TFT_BLACK);
	spriteScreen->setCursor(6, 28);
	uint8_t min1 = 0;
	uint8_t sec1 = 0;
	if(pathA[0] != 0)
	{
		trackMutex->lock();
		min1 = (int)((trackA->getSamplePos() / 44100) / 60);
		sec1 = (int)((trackA->getSamplePos() / 44100) % 60);
		trackMutex->unlock();
	}
	spriteScreen->print(min1);
	if (sec1 > 9)
		spriteScreen->print(":");
	else
		spriteScreen->print(":0");
	spriteScreen->print(sec1);
	spriteScreen->drawLine(6, 36, 28, 36, TFT_BLACK);
	spriteScreen->setCursor(6, 38);
	uint8_t sizeMin1 = 0;
	uint8_t sizeSec1 = 0;
	if(pathA[0] != 0)
	{
		trackMutex->lock();
		sizeSec1 = (int)((trackA->getSize() / 44100) % 60);
		sizeMin1 = (int)((trackA->getSize() / 44100) / 60);
		trackMutex->unlock();
	}
	spriteScreen->print(sizeMin1);
	if (sizeSec1 > 9)
		spriteScreen->print(":");
	else
		spriteScreen->print(":0");
	spriteScreen->print(sizeSec1);
	spriteScreen->setTextColor(TFT_WHITE);
	spriteScreen->setCursor(10, 4);
	if(pathA[0] != 0){
		std::string track(pathA);
		track = track.substr(0, track.size()-4);
		spriteScreen->print(track.c_str());
	}

	spriteScreen->fillRect(0, 64, 160, 64, 0x2114);
	spriteScreen->fillRect(0, 65, 160, 2, 0x3061);

	spriteScreen->drawIcon(DJIcon, 64, 68, 32, 32, 1); // CENTER

	spriteScreen->setCursor(100, 84);
	spriteScreen->setTextColor(TFT_BLACK);
	if (speedB >= -0.05 && speedB <= 0.05){
		spriteScreen->print("+0.0%");
	}
	else if(speedB < -0.99){
		spriteScreen->print("-100.0%");
	}
	else if(speedB > 0.99){
		spriteScreen->print("+100.0%");
	}
	else{
		spriteScreen->print(speedB * 100);
		spriteScreen->print("%");
	}
	
	
	spriteScreen->fillRoundRect(6, 110, 148, 16, 2, 0x10A7);
	spriteScreen->setTextColor(TFT_BLACK);
	spriteScreen->setCursor(6, 80);
	uint8_t min2 = 0;
	uint8_t sec2 = 0;
	if(pathB[0] != 0)
	{
		trackMutex->lock();
		min2 = (int)((trackB->getSamplePos() / 44100) / 60);
		sec2 = (int)((trackB->getSamplePos() / 44100) % 60);
		trackMutex->unlock();
	}
	spriteScreen->print(min2);
	if (sec2 > 9)
		spriteScreen->print(":");
	else
		spriteScreen->print(":0");
	spriteScreen->print(sec2);
	spriteScreen->drawLine(6, 88, 28, 88, TFT_BLACK);
	spriteScreen->setCursor(6, 90);
	uint8_t sizeMin2 = 0;
	uint8_t sizeSec2 = 0;
	if(pathB[0] != 0)
	{
		trackMutex->lock();
		sizeSec2 = (int)((trackB->getSize() / 44100) % 60);
		sizeMin2 = (int)((trackB->getSize() / 44100) / 60);
		trackMutex->unlock();
	}
	spriteScreen->print(sizeMin2);
	if (sizeSec2 > 9)
		spriteScreen->print(":");
	else
		spriteScreen->print(":0");
	spriteScreen->print(sizeSec2);
	spriteScreen->setTextColor(TFT_WHITE);
	spriteScreen->setCursor(10, 112);
	if(pathB[0] != 0){
		std::string track(pathB);
		track = track.substr(0, track.size()-4);
		spriteScreen->print(track.c_str());
	}
	screen.commit();
}

void DjScreen::update(uint _time)
{
	if(packed) return;

	secondTimer+=_time;
	if(secondTimer > 1000)
	{
		screenMutex.lock();
		draw();
		screenMutex.unlock();
		secondTimer = 0;
	}
	vuRefreshTimer+=_time;
	if(vuRefreshTimer > 100)
	{
		refreshVu();
		pixelsMutex.unlock();
		vuRefreshTimer = 0;
	}
}

void DjScreen::backToSelect()
{
	instance->parent->push(instance);
}

void DjScreen::setVolumeA(float volume)
{
	instance->faderMutex.lock();
	instance->volumeA = (100.0 - float(volume*100.0));
	if(instance->volumeA >= 95.0) instance->volumeA = 100.00;
	if(instance->volumeA <= 5.0) instance->volumeA = 0.0;
	instance->setFader(instance->faderValue);
	instance->faderMutex.unlock();
}
void DjScreen::setVolumeB(float volume)
{
	instance->faderMutex.lock();
	instance->volumeB = (float(volume*100.0));
	if(instance->volumeB >= 95.0) instance->volumeB = 100.00;
	if(instance->volumeB <= 5.0) instance->volumeB = 0.0;
	instance->setFader(instance->faderValue);
	instance->faderMutex.unlock();
}
void DjScreen::faderCallback(float fade)
{
	instance->faderMutex.lock();
	setFader(fade);
	instance->faderMutex.unlock();
}
void DjScreen::setFader(float fade)
{
	instance->faderValue = fade;
	if(fade <= 0.05) fade = 0;
	else if(fade >= 0.95) fade = 1.0;
	instance->trackA->setVolume(float(fade >= 0.5 ? instance->volumeA : instance->volumeA*(fade*2.0)));
	instance->trackB->setVolume(float(fade <= 0.5 ? instance->volumeB : instance->volumeB*(2.0 - fade*2.0)));
	uint16_t pixelsValue = int(instance->faderValue*2550.0); //2550 = 255 levels on 10 LEDs
	// Serial.println(pixelsValue); //728
	instance->pixelsMutex.lock();
	for(uint8_t i = 0; i < 10; i++)
	{
		if(pixelsValue < i*255)
		{
			instance->strip->SetPixelColor(5 + i, RgbColor(0, 0, 255));
		}
		else if(pixelsValue > (i + 1)*255)
		{
			instance->strip->SetPixelColor(5 + i, RgbColor(255, 0, 0));
		}
		else{
			instance->strip->SetPixelColor(5 + i, RgbColor(pixelsValue - i*255, 0, (i+1)*255 - pixelsValue));
			// Serial.print((pixelsValue - i*255) % 255);
		}
	}
	instance->strip->Show();
	instance->pixelsMutex.unlock();

}
void DjScreen::setFilterA(float val)
{
	instance->filterA = val;
	instance->trackMutex->lock();
	instance->trackA->setFilterAmount(val >= 0.5 ? float(val*2.0 - 1.0) : float(1.0 - val*2.0), val >= 0.5 ? HIPASS : LOWPASS);
	instance->trackMutex->unlock();
}
void DjScreen::setFilterB(float val)
{
	instance->filterB = val;
	instance->trackMutex->lock();
	instance->trackB->setFilterAmount(val >= 0.5 ? float(val*2.0 - 1.0) : float(1.0 - val*2.0), val >= 0.5 ? HIPASS : LOWPASS);
	instance->trackMutex->unlock();
}
void DjScreen::setEchoA(float val)
{
	instance->echoA = val;
	instance->trackMutex->lock();
	instance->trackA->setEchoAmount(val);
	instance->trackA->setEchoPeriod(val);
	instance->trackMutex->unlock();
}
void DjScreen::setEchoB(float val)
{
	instance->echoB = val;
	instance->trackMutex->lock();
	instance->trackB->setEchoAmount(val);
	instance->trackB->setEchoPeriod(val);
	instance->trackMutex->unlock();
}
void DjScreen::refreshVu()
{
	//TODO smooth VU change
	// memory element (can be imagined as capacitor)
	// c = 0
	// float kCharge = 0.1
	// float kDischarge = 0.001

	// if x1 > c then
	// {
	// 	// charge
	// 	c = c * (1-kCharge) + x1*kCharge
	// } else
	// {
	// 	// discharge
	// 	c = c * (1-kDischarge)
	// }
	instance->trackMutex->lock();
	uint16_t vuA = instance->trackA->vu;
	uint16_t vuB = instance->trackB->vu;
	instance->trackMutex->unlock();
	int32_t pixelsValue = 0;
	if(vuA < VU_THRESHLO){
		pixelsValue = 0;
	}
	else if(vuA > VU_THRESHHI){
		pixelsValue = 1275;
	}
	else{
		pixelsValue = map(vuA, 30, 100, 0, 1275);
	}
	// Serial.println(pixelsValue); //728
	instance->pixelsMutex.lock();
	for(uint8_t i = 0; i < 5; i++)
	{
		if(pixelsValue < i*255)
		{
			instance->strip->SetPixelColor(15+i, RgbColor(0, 0, 0));
		}
		else if(pixelsValue > (i + 1)*255)
		{
			instance->strip->SetPixelColor(15+i, RgbColor(255, 0, 0));
		}
		else{
			instance->strip->SetPixelColor(15+i, RgbColor(pixelsValue - i*255, 0, 0));
			// Serial.print((pixelsValue - i*255) % 255);
		}
	}
	instance->pixelsMutex.unlock();

	if(vuB < VU_THRESHLO){
		pixelsValue = 0;
	}
	else if(vuB > VU_THRESHHI){
		pixelsValue = 1275;
	}
	else{
		pixelsValue = map(vuB, 30, 100, 0, 1275);
	}
	// Serial.println(pixelsValue); //728
	instance->pixelsMutex.lock();
	for(uint8_t i = 0; i < 5; i++)
	
	{
		if(pixelsValue < i*255)
		{
			instance->strip->SetPixelColor(4-i, RgbColor(0, 0, 0));
		}
		else if(pixelsValue > (i + 1)*255)
		{
			instance->strip->SetPixelColor(4-i, RgbColor(0, 0, 255));
		}
		else{
			instance->strip->SetPixelColor(4-i, RgbColor(0, 0, pixelsValue - i*255));
			// Serial.print((pixelsValue - i*255) % 255);
		}
	}
	// Serial.println();
	instance->strip->Show();
	instance->pixelsMutex.unlock();
}
void DjScreen::fxToggleA()
{
	if(instance->fxOnA)
	{
		setPotChangedEvent(POT_FX1_LEFT, nullptr);
		setPotChangedEvent(POT_FX2_LEFT, nullptr);
		setFilterA(0.5);
		setEchoA(0.0);
	}
	else
	{
		setFilterA(getPotValue(POT_FX1_LEFT));
		setPotChangedEvent(POT_FX1_LEFT, setFilterA);
		setEchoA(getPotValue(POT_FX2_LEFT));
		setPotChangedEvent(POT_FX2_LEFT, setEchoA);
	}
	instance->fxOnA = !instance->fxOnA;
}
void DjScreen::fxToggleB()
{
	if(instance->fxOnB)
	{
		setPotChangedEvent(POT_FX1_RIGHT, nullptr);
		setPotChangedEvent(POT_FX2_RIGHT, nullptr);
		setFilterB(0.5);
		setEchoB(0.0);
	}
	else
	{
		setFilterB(getPotValue(POT_FX1_RIGHT));
		setPotChangedEvent(POT_FX1_RIGHT, setFilterB);
		setEchoB(getPotValue(POT_FX2_RIGHT));
		setPotChangedEvent(POT_FX2_RIGHT, setEchoB);
	}
	instance->fxOnB = !instance->fxOnB;
}