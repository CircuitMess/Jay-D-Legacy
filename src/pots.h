#ifndef DJTHING_POTS_H
#define DJTHING_POTS_H

#include <Arduino.h>
#include <Adafruit_ADS1015.h>

#include <RotaryEncoder.h>
#include <Sync/Mutex.h>
#include <SPI.h>
#include "DjThing.h"

extern Mutex adsMutex;

void scanADC();
void setEncoderChangedEvent(unsigned char btn, void (*fun)());
void adcChanged(uint8_t pot, float val);
void setPotChangedEvent(uint8_t pot, void (*fun)(float));
void initPots();
void scanEncoder();
float getPotValue(uint8_t pot);
#endif