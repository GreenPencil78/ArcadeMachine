#ifndef ASTEROID_H
#define ASTEROID_H

#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Arduino.h>
#include <stdlib.h>
#include "pitches.h"
#include "pong.h"

bool startAsteroid(Adafruit_SSD1306 &display);
void resetAsteroids();
void fireBullet(float shipX, float shipY, float angle);
void drawShip(float x, float y, float angle, int shipSize, Adafruit_SSD1306 &display);

#endif