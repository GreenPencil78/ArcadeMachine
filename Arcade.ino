#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "pitches.h"
#include "pong.h"
#include "asteroid.h"

// OLED SPI display pins
#define OLED_MOSI 16  // Hardware MOSI
#define OLED_CLK  15  // Hardware SCK
#define OLED_DC    9
#define OLED_CS   10
#define OLED_RST   8

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

#define PUSH_BUTTON 2
#define SECOND_BUTTON 7
#define SPEAKER_PIN 3
#define X_PIN A0
#define Y_PIN A1
#define SWITCH_PIN 4

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &SPI, OLED_DC, OLED_RST, OLED_CS);

void setup() {
  Serial.begin(9600);
  pinMode(PUSH_BUTTON, INPUT_PULLUP);
  pinMode(SECOND_BUTTON, INPUT_PULLUP);
  pinMode(X_PIN, INPUT);
  pinMode(Y_PIN, INPUT);
  pinMode(SWITCH_PIN, INPUT_PULLUP);

  if (!display.begin(SSD1306_SWITCHCAPVCC)) {
    for (;;);
  }
  
  randomSeed(analogRead(3));

  bool pressed = false;
  bool replay = false;
  while(pressed == false){
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.println("Press 1 to play Pong and 2 for Asteroid");
    display.display();

    if (digitalRead(PUSH_BUTTON) == 0){
      pressed = true;
      delay(500);
      replay = startGame(display);
      if (replay == true){
        pressed = false;
        replay = false;
      }
    }
    if (digitalRead(SECOND_BUTTON) == 0){
      pressed = true;
      delay(500);
      replay = startAsteroid(display);
      if (replay == true){
        pressed = false;
        replay = false;
      }
    }
    delay(10);
  }
  //Serial.println("Press 1 to play Pong and 2 to play Asteroid");
}

void loop(){
  
}