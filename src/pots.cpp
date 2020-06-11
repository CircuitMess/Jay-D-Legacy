#include "pots.h"
#include <Sync/Mutex.h>
//audio refresh task
TaskHandle_t Task1, Task2, Task3, Task4, Task5;

RotaryEncoder encoder(34, 36);
Adafruit_ADS1015 adsArray[3];

#define ADC_TRESH 5
#define ENC_TRESH 2
#define ENC_RESET 100
#define ADC_MAX 1095.0

void (*encChanged[ENCODER_COUNT])() = {NULL};
void (*potChanged[POT_COUNT])(float) = {NULL};
uint16_t adsRead[POT_COUNT] = {0};
uint16_t adsPrevRead[POT_COUNT] = {0};
uint8_t adc_counter = 0;
int32_t enc, encPrev = 0;
Mutex adsMutex;
void initPots()
{
	adsArray[0] = Adafruit_ADS1015(0x48);
	adsArray[1] = Adafruit_ADS1015(0x49);
	adsArray[2] = Adafruit_ADS1015(0x4B);
	for (Adafruit_ADS1015 ads : adsArray)
	{
		ads.begin();
		ads.setGain(GAIN_ONE);
	}
}

void scanADC()
{
	for(uint8_t i = 0; i < POT_COUNT; i++)
	{
		adsMutex.lock();
		adsArray[i/4].readADC_SingleEnded(i%4);
		adsRead[i] = adsArray[i/4].readADC_SingleEnded(i%4);
		adsMutex.unlock();
		// Serial.printf("ADC %d: %d, ", i, adsRead[i]);
		if (adsRead[i] < (adsPrevRead[i] - ADC_TRESH) || adsRead[i] > (adsPrevRead[i] + ADC_TRESH))
		{
			if (potChanged[i] != NULL)
				potChanged[i](float(adsRead[i] / ADC_MAX));
			adsPrevRead[i] = adsRead[i];
		}
	}
	// Serial.println("-------------");
}

void scanEncoder()
{
	encoder.tick();
	int8_t dir = (int)encoder.getDirection();
	if(dir != 0)
	{
		if(dir > 0 && encChanged[0] != NULL)
			encChanged[0]();
		if(dir < 0 && encChanged[1] != NULL)
			encChanged[1]();
	}
	vTaskDelay(1);
}

void clearEvents()
{
	for (uint8_t i = 0; i < POT_COUNT; i++)
		potChanged[i] = NULL;
}

void setEncoderChangedEvent(unsigned char btn, void (*fun)())
{
	encChanged[btn] = fun;
}

void setPotChangedEvent(uint8_t pot, void (*fun)(float))
{
	potChanged[pot] = fun;
}

float getPotValue(uint8_t pot)
{
	adsMutex.lock();
	float read = float(adsRead[pot] / ADC_MAX);
	adsMutex.unlock();
	return read;
}
// void setLEDs()
// {
// 	// BMP Track A
// 	if (adc2 > 861)
// 	{
// 		strip.SetPixelColor(8, RgbColor(20, 20, 80));
// 		if (adc2 > 1020)
// 		{
// 			strip.SetPixelColor(9, RgbColor(20, 20, 80));
// 			if (adc2 > 1220)
// 			{
// 				strip.SetPixelColor(10, RgbColor(20, 20, 80));
// 				if (adc2 > 1420)
// 					strip.SetPixelColor(11, RgbColor(20, 20, 80));
// 				else
// 					strip.SetPixelColor(11, RgbColor(0, 0, 0));
// 			}
// 			else
// 			{
// 				strip.SetPixelColor(10, RgbColor(0, 0, 0));
// 				strip.SetPixelColor(11, RgbColor(0, 0, 0));
// 			}
// 		}
// 		else
// 		{
// 			strip.SetPixelColor(9, RgbColor(0, 0, 0));
// 			strip.SetPixelColor(10, RgbColor(0, 0, 0));
// 			strip.SetPixelColor(11, RgbColor(0, 0, 0));
// 		}
// 	}
// 	else if (adc2 < 778)
// 	{
// 		strip.SetPixelColor(8, RgbColor(255, 255, 100));
// 		if (adc2 < 618)
// 		{
// 			strip.SetPixelColor(9, RgbColor(255, 205, 0));
// 			if (adc2 < 418)
// 			{
// 				strip.SetPixelColor(10, RgbColor(255, 100, 0));
// 				if (adc2 < 218)
// 					strip.SetPixelColor(11, RgbColor(255, 0, 0));
// 				else
// 					strip.SetPixelColor(11, RgbColor(0, 0, 0));
// 			}
// 			else
// 			{
// 				strip.SetPixelColor(10, RgbColor(0, 0, 0));
// 				strip.SetPixelColor(11, RgbColor(0, 0, 0));
// 			}
// 		}
// 		else
// 		{
// 			strip.SetPixelColor(9, RgbColor(0, 0, 0));
// 			strip.SetPixelColor(10, RgbColor(0, 0, 0));
// 			strip.SetPixelColor(11, RgbColor(0, 0, 0));
// 		}
// 	}
// 	else
// 	{
// 		strip.SetPixelColor(8, RgbColor(0, 0, 0));
// 		strip.SetPixelColor(9, RgbColor(0, 0, 0));
// 		strip.SetPixelColor(10, RgbColor(0, 0, 0));
// 		strip.SetPixelColor(11, RgbColor(0, 0, 0));
// 	}
// 	// BMP Track A END
// 	// BMP Track B
// 	if (adc6 > 861)
// 	{
// 		strip.SetPixelColor(31, RgbColor(20, 20, 80));
// 		if (adc6 > 1020)
// 		{
// 			strip.SetPixelColor(23, RgbColor(20, 20, 80));
// 			if (adc6 > 1220)
// 			{
// 				strip.SetPixelColor(30, RgbColor(20, 20, 80));
// 				if (adc6 > 1420)
// 					strip.SetPixelColor(22, RgbColor(20, 20, 80));
// 				else
// 					strip.SetPixelColor(22, RgbColor(0, 0, 0));
// 			}
// 			else
// 			{
// 				strip.SetPixelColor(30, RgbColor(0, 0, 0));
// 				strip.SetPixelColor(22, RgbColor(0, 0, 0));
// 			}
// 		}
// 		else
// 		{
// 			strip.SetPixelColor(23, RgbColor(0, 0, 0));
// 			strip.SetPixelColor(30, RgbColor(0, 0, 0));
// 			strip.SetPixelColor(22, RgbColor(0, 0, 0));
// 		}
// 	}
// 	else if (adc6 < 778)
// 	{
// 		strip.SetPixelColor(31, RgbColor(255, 255, 100));
// 		if (adc6 < 618)
// 		{
// 			strip.SetPixelColor(23, RgbColor(255, 205, 0));
// 			if (adc6 < 418)
// 			{
// 				strip.SetPixelColor(30, RgbColor(255, 100, 0));
// 				if (adc6 < 218)
// 					strip.SetPixelColor(22, RgbColor(255, 0, 0));
// 				else
// 					strip.SetPixelColor(22, RgbColor(0, 0, 0));
// 			}
// 			else
// 			{
// 				strip.SetPixelColor(30, RgbColor(0, 0, 0));
// 				strip.SetPixelColor(22, RgbColor(0, 0, 0));
// 			}
// 		}
// 		else
// 		{
// 			strip.SetPixelColor(23, RgbColor(0, 0, 0));
// 			strip.SetPixelColor(30, RgbColor(0, 0, 0));
// 			strip.SetPixelColor(22, RgbColor(0, 0, 0));
// 		}
// 	}
// 	else
// 	{
// 		strip.SetPixelColor(31, RgbColor(0, 0, 0));
// 		strip.SetPixelColor(23, RgbColor(0, 0, 0));
// 		strip.SetPixelColor(30, RgbColor(0, 0, 0));
// 		strip.SetPixelColor(22, RgbColor(0, 0, 0));
// 	}
// 	// BMP Track B end
// 	// Fader
// 	if (adc3 >= 700 && adc3 <= 820)
// 	{
// 		strip.SetPixelColor(4, RgbColor(200, 0, 0));
// 		strip.SetPixelColor(5, RgbColor(200, 0, 0));
// 		strip.SetPixelColor(6, RgbColor(0, 0, 0));
// 		strip.SetPixelColor(7, RgbColor(0, 0, 0));
// 		strip.SetPixelColor(29, RgbColor(0, 0, 200));
// 		strip.SetPixelColor(21, RgbColor(0, 0, 200));
// 		strip.SetPixelColor(28, RgbColor(0, 0, 0));
// 		strip.SetPixelColor(20, RgbColor(0, 0, 0));
// 	}
// 	else if (adc3 > 820 && adc3 < 1300)
// 	{
// 		strip.SetPixelColor(4, RgbColor(200, 0, 0));
// 		strip.SetPixelColor(5, RgbColor(200, 0, 0));
// 		strip.SetPixelColor(6, RgbColor(200, 0, 0));
// 		strip.SetPixelColor(7, RgbColor(0, 0, 0));
// 		strip.SetPixelColor(29, RgbColor(0, 0, 200));
// 		strip.SetPixelColor(21, RgbColor(0, 0, 0));
// 		strip.SetPixelColor(28, RgbColor(0, 0, 0));
// 		strip.SetPixelColor(20, RgbColor(0, 0, 0));
// 	}
// 	else if (adc3 >= 1300)
// 	{
// 		strip.SetPixelColor(4, RgbColor(200, 0, 0));
// 		strip.SetPixelColor(5, RgbColor(200, 0, 0));
// 		strip.SetPixelColor(6, RgbColor(200, 0, 0));
// 		strip.SetPixelColor(7, RgbColor(200, 0, 0));
// 		strip.SetPixelColor(29, RgbColor(0, 0, 0));
// 		strip.SetPixelColor(21, RgbColor(0, 0, 0));
// 		strip.SetPixelColor(28, RgbColor(0, 0, 0));
// 		strip.SetPixelColor(20, RgbColor(0, 0, 0));
// 	}
// 	else if (adc3 > 300 && adc3 < 700)
// 	{
// 		strip.SetPixelColor(4, RgbColor(200, 0, 0));
// 		strip.SetPixelColor(5, RgbColor(0, 0, 0));
// 		strip.SetPixelColor(6, RgbColor(0, 0, 0));
// 		strip.SetPixelColor(7, RgbColor(0, 0, 0));
// 		strip.SetPixelColor(29, RgbColor(0, 0, 200));
// 		strip.SetPixelColor(21, RgbColor(0, 0, 200));
// 		strip.SetPixelColor(28, RgbColor(0, 0, 200));
// 		strip.SetPixelColor(20, RgbColor(0, 0, 0));
// 	}
// 	else
// 	{
// 		strip.SetPixelColor(4, RgbColor(0, 0, 0));
// 		strip.SetPixelColor(5, RgbColor(0, 0, 0));
// 		strip.SetPixelColor(6, RgbColor(0, 0, 0));
// 		strip.SetPixelColor(7, RgbColor(0, 0, 0));
// 		strip.SetPixelColor(29, RgbColor(0, 0, 200));
// 		strip.SetPixelColor(21, RgbColor(0, 0, 200));
// 		strip.SetPixelColor(28, RgbColor(0, 0, 200));
// 		strip.SetPixelColor(20, RgbColor(0, 0, 200));
// 	}
// 	//Fader END
// 	//Vu meter
// 	if (vuShow < 30)
// 	{
// 		strip.SetPixelColor(12, RgbColor(0, 0, 0));
// 		strip.SetPixelColor(13, RgbColor(0, 0, 0));
// 		strip.SetPixelColor(14, RgbColor(0, 0, 0));
// 		strip.SetPixelColor(15, RgbColor(0, 0, 0));
// 		strip.SetPixelColor(16, RgbColor(0, 0, 0));
// 		strip.SetPixelColor(24, RgbColor(0, 0, 0));
// 		strip.SetPixelColor(17, RgbColor(0, 0, 0));
// 		strip.SetPixelColor(25, RgbColor(0, 0, 0));
// 	}
// 	else if (vuShow >= 30 && vuShow < 55)
// 	{
// 		strip.SetPixelColor(12, RgbColor(120, 200, 100).Dim(60));
// 		strip.SetPixelColor(13, RgbColor(0, 0, 0));
// 		strip.SetPixelColor(14, RgbColor(0, 0, 0));
// 		strip.SetPixelColor(15, RgbColor(0, 0, 0));
// 		strip.SetPixelColor(16, RgbColor(120, 200, 100).Dim(60));
// 		strip.SetPixelColor(24, RgbColor(0, 0, 0));
// 		strip.SetPixelColor(17, RgbColor(0, 0, 0));
// 		strip.SetPixelColor(25, RgbColor(0, 0, 0));
// 	}
// 	else if (vuShow >= 55 && vuShow < 80)
// 	{
// 		strip.SetPixelColor(12, RgbColor(120, 200, 100).Dim(100));
// 		strip.SetPixelColor(13, RgbColor(55, 230, 55).Dim(100));
// 		strip.SetPixelColor(14, RgbColor(0, 0, 0));
// 		strip.SetPixelColor(15, RgbColor(0, 0, 0));
// 		strip.SetPixelColor(16, RgbColor(120, 200, 100).Dim(100));
// 		strip.SetPixelColor(24, RgbColor(55, 230, 55).Dim(100));
// 		strip.SetPixelColor(17, RgbColor(0, 0, 0));
// 		strip.SetPixelColor(25, RgbColor(0, 0, 0));
// 	}
// 	else if (vuShow >= 80 && vuShow < 90)
// 	{
// 		strip.SetPixelColor(12, RgbColor(120, 200, 100).Dim(140));
// 		strip.SetPixelColor(13, RgbColor(55, 230, 55).Dim(140));
// 		strip.SetPixelColor(14, RgbColor(255, 155, 0).Dim(140));
// 		strip.SetPixelColor(15, RgbColor(0, 0, 0));
// 		strip.SetPixelColor(16, RgbColor(120, 200, 100).Dim(140));
// 		strip.SetPixelColor(24, RgbColor(55, 230, 55).Dim(140));
// 		strip.SetPixelColor(17, RgbColor(255, 155, 0).Dim(140));
// 		strip.SetPixelColor(25, RgbColor(0, 0, 0));
// 	}
// 	else
// 	{
// 		strip.SetPixelColor(12, RgbColor(120, 200, 100));
// 		strip.SetPixelColor(13, RgbColor(55, 230, 55));
// 		strip.SetPixelColor(14, RgbColor(255, 155, 0));
// 		strip.SetPixelColor(15, RgbColor(255, 55, 0));
// 		strip.SetPixelColor(16, RgbColor(120, 200, 100));
// 		strip.SetPixelColor(24, RgbColor(55, 230, 55));
// 		strip.SetPixelColor(17, RgbColor(255, 155, 0));
// 		strip.SetPixelColor(25, RgbColor(255, 55, 0));
// 	}
// 	//Vu meter - end
// 	/*
// 	strip.ClearTo(RgbColor(0, 0, 0));
// 	for (int i = 0; i < 32; i++)
// 	{
// 		strip.SetPixelColor(i, RgbColor(200, 0, 0));
// 		delay(300);
// 	}*/
// 	strip.SetBrightness(20);
// 	strip.Show();
// 	vTaskDelay(1);
// }


