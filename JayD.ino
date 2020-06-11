#include <Arduino.h>
#include <CircuitOS.h>
#include <Display/Display.h>
#include "src/sprites.h"
#include <NeoPixelBrightnessBus.h>
#include <MPWavLib.h>
#include <MPAudioDriver.h>
#include <CircuitOS.h>
#include <UI/Screen.h>
#include <Elements/ListMenu.h>
#include <Input/InputI2C.h>
#include <esp_task_wdt.h>
#include "src/pots.h"
#include <Sync/Mutex.h>
#include <Update/UpdateManager.h>
#include "src/DjScreen.h"
#include "src/TrackSelectScreen.h"
Mutex trackMutex;
Mutex vuMutex;
Mutex i2cMutex;
I2cExpander i2c;
InputI2C buttons(&i2c, &adsMutex);
Display display(160, 128, -1, 3);
uint8_t numPixels = 20;
uint8_t pixelPin = 32;
NeoPixelBrightnessBus<NeoGrbFeature, Neo800KbpsMethod> strip(numPixels, pixelPin);
// Screen screen(display);

// LIST MENU
volatile uint32_t displayRefreshMillis = millis();

void audioTask(void *pvParameters)
{
	while (true)
		updateWav(&trackMutex, &vuMutex);
}

void ADCtask(void *pvParameters)
{
	while (true)
	{
		scanADC();
		vTaskDelay(1);
	}
}

void encoderTask(void *)
{
	while(1){
		scanEncoder();
	}
}


void refreshPixels(void *params)
{
	while (1)
	{
		strip.Show();
		vTaskDelay(1);
	}
}
uint8_t ledPins[] = {LED_LOAD_LEFT, LED_PLAY_RIGHT, LED_PLAY_LEFT, LED_FX_LEFT, LED_FX_RIGHT, LED_LOAD_RIGHT};
TaskHandle_t TaskHandle1, TaskHandle2, TaskHandle3;

TrackSelectScreen* trackSelectScreen;
DjScreen* djScreen;
void setup()
{
	Serial.begin(115200);
	display.begin();
	i2c.begin(0x74, 14, 27);
	for(uint8_t i = 0; i < 6;i++)
	{
		pinMode(ledPins[i], OUTPUT);
		digitalWrite(ledPins[i], 1);
	}
	// while (!SD.begin(5, SPI, 9000000))
	// 	Serial.println("SD ERROR");
	// display.clear(TFT_GREEN);
	// display.commit();
	// delay(1000);
	// display.getBaseSprite()->fillRect(0,0,10,10,TFT_RED);
	// display.commit();
	// delay(1000);
	djScreen = new DjScreen(display, &trackMutex, &vuMutex, &strip);
	trackSelectScreen = new TrackSelectScreen(display, djScreen);
	trackSelectScreen->unpack();
	trackSelectScreen->start();
	//Audio
	initWavLib();
	strip.Begin();
	strip.ClearTo(RgbColor(255, 255, 255));
	strip.SetBrightness(20);
	strip.Show();

	// MPTrack* track = new MPTrack("/Anderson .Paak.wav");
	// track->setVolume(40);
	// addTrack(track);
	// track->play();
	xTaskCreate(
		audioTask,	 /* Task function. */
		"UpdateWav", /* name of task. */
		10000,		 /* Stack size of task */
		NULL,		 /* parameter of the task */
		2,			 /* priority of the task */
		&TaskHandle1);	 /* Task handle to keep track of created task */
	initPots();
	xTaskCreate(
		ADCtask,
		"ReadADC",
		5000,
		NULL,
		1,
		&TaskHandle2);
	xTaskCreate(
		encoderTask,
		"EncoderTask",
		5000,
		NULL,
		1,
		&TaskHandle3);
	// // buttons.start();
	UpdateManager::addListener(&buttons);
	esp_task_wdt_init(820, false);
	setVolume(300);
	// while(1);
}

void loop()
{
	UpdateManager::update();
	/* 	Serial.println("DA");
	Serial.println("DA");
	Serial.println("DA");
	Serial.println("DA");
	if (millis() - displayRefreshMillis > 100)
	{

		Serial.println("DA"); */

	// vTaskDelay(1);
	//}


	//Serial.println(trackA->getSize());
	//Serial.println(trackB->getSize());

	//screen();

	// vTaskGetRunTimeStats(&buff[0]);
	// Serial.println(xthal_get_ccount());
}

//Supernature.wav - 10 223 516 bytes - 3:51
//swhftm.wav - 14 120 732(14 155 776) - 5:20
//Takefive.wav - 19 925 660 (19 988 480) - 7:31
