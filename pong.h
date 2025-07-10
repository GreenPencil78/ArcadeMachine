#ifndef PONG_H
#define PONG_H

#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Arduino.h>
#include <stdlib.h>
#include "pitches.h"

bool startGame(Adafruit_SSD1306 &display);
void onePlayer(Adafruit_SSD1306 &display);
void playPaddleSound();
void playWallSound();
int randomizeStart();
int movePaddle(int X);
void gameOver(Adafruit_SSD1306 &display);
void playGameOverSound();
void drawBricks(Adafruit_SSD1306 &display);
void spawnBricks();
void initBricks();
int twoPlayer(Adafruit_SSD1306 &display);
int movePaddle1TwoPlayer(int Y);
int movePaddle2TwoPlayer(int Y);
void gameOverTwo(Adafruit_SSD1306 & display, int player);

#endif