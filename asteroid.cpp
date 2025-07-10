#include "asteroid.h"

#define PUSH_BUTTON 2
#define SECOND_BUTTON 7
#define SPEAKER_PIN 3
#define X_PIN A0
#define Y_PIN A1
#define SWITCH_PIN 4

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

#define MAX_BULLETS 5
#define MAX_ASTEROIDS 3
#define ASTEROID_SIZE 4

struct Bullet {
  float x, y;
  float dx, dy;
  bool active;
};

struct Asteroid {
  float x, y;
  float dx, dy;
};

Bullet bullets[MAX_BULLETS];
Asteroid asteroids[MAX_ASTEROIDS];

bool startAsteroid(Adafruit_SSD1306 &display){
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("Press 1 to fire and 2 to go");
  display.display();
  delay(2000);

  resetAsteroids();
  float shipX = SCREEN_WIDTH / 2;
  float shipY = SCREEN_HEIGHT / 2;
  int shipSize = 8;
  float velocityX = 0;
  float velocityY = 0;
  int lives = 3;
  int score = 0;

  while (true){
  int xVal = analogRead(X_PIN) - 512;
  int yVal = analogRead(Y_PIN) - 512;
  float angle = 0;
  //dead zone
  if (abs(xVal) > 20 || abs(yVal) > 20) {
    angle = atan2(yVal, xVal);
  }

  if (digitalRead(SECOND_BUTTON) == 0) {
    velocityX += cos(angle) * 0.1;
    velocityY += sin(angle) * 0.1;
  }

  if (digitalRead(PUSH_BUTTON) == 0) {
    fireBullet(shipX, shipY, angle);
    tone(SPEAKER_PIN, NOTE_G6, 50);
    delay(100);  // simple debounce / rate limit
  }

  shipX += velocityX;
  shipY += velocityY;

  //if the ship goes off the edge of the screen it wraps around
  if (shipX < 0) shipX = SCREEN_WIDTH;
  if (shipX > SCREEN_WIDTH) shipX = 0;
  if (shipY < 0) shipY = SCREEN_HEIGHT;
  if (shipY > SCREEN_HEIGHT) shipY = 0;

  //deactivates bullet if it goes off the edge
  for (int i = 0; i < MAX_BULLETS; i++) {
    if (bullets[i].active) {
      bullets[i].x += bullets[i].dx;
      bullets[i].y += bullets[i].dy;
      if (bullets[i].x < 0 || bullets[i].x > SCREEN_WIDTH || bullets[i].y < 0 || bullets[i].y > SCREEN_HEIGHT) {
        bullets[i].active = false;
      }
    }
  }

  //updates asteroid position
  for (int i = 0; i < MAX_ASTEROIDS; i++) {
    asteroids[i].x += asteroids[i].dx;
    asteroids[i].y += asteroids[i].dy;
    if (asteroids[i].x < 0) asteroids[i].x = SCREEN_WIDTH;
    if (asteroids[i].x > SCREEN_WIDTH) asteroids[i].x = 0;
    if (asteroids[i].y < 0) asteroids[i].y = SCREEN_HEIGHT;
    if (asteroids[i].y > SCREEN_HEIGHT) asteroids[i].y = 0;
  }
  for (int i = 0; i < MAX_ASTEROIDS; i++) {
    float dx = asteroids[i].x - shipX;
    float dy = asteroids[i].y - shipY;
    float dist = sqrt(dx * dx + dy * dy);

    //Ship hits asteroid
    if (dist < ASTEROID_SIZE) {  // adjust collision radius as needed
      lives--;
      tone(SPEAKER_PIN, NOTE_A1, 50);
      // Reset ship position & velocity after hit
      shipX = SCREEN_WIDTH / 2;
      shipY = SCREEN_HEIGHT / 2;
      velocityX = 0;
      velocityY = 0;
    }
  }
  if (lives <= 0) {
    gameOver(display);
    break;
  }

  // Check bullet-asteroid collisions
  for (int i = 0; i < MAX_BULLETS; i++) {
    if (bullets[i].active) {
      for (int j = 0; j < MAX_ASTEROIDS; j++) {
        float dx = bullets[i].x - asteroids[j].x;
        float dy = bullets[i].y - asteroids[j].y;
        if (sqrt(dx*dx + dy*dy) < ASTEROID_SIZE) {
          tone(SPEAKER_PIN, NOTE_C4, 100);
          score+=10;
          bullets[i].active = false;
          asteroids[j].x = random(SCREEN_WIDTH);
          asteroids[j].y = random(SCREEN_HEIGHT);
          asteroids[j].dx = random(-2, 3);
          asteroids[j].dy = random(-2, 3);
        }
      }
    }
  }

  // Draw everything
  display.clearDisplay();
  drawShip(shipX, shipY, angle, shipSize, display);
  //display.fillCircle(shipX, shipY, 3, SSD1306_WHITE);
  for (int i = 0; i < MAX_BULLETS; i++) {
    if (bullets[i].active) {
      display.fillCircle(bullets[i].x, bullets[i].y, 1, SSD1306_WHITE);
    }
  }
  for (int i = 0; i < MAX_ASTEROIDS; i++) {
    display.drawCircle(asteroids[i].x, asteroids[i].y, ASTEROID_SIZE, SSD1306_WHITE);
  }

  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.print("Lives: ");
  display.print(lives);
  display.setCursor(64, 0);
  display.print("Score: ");
  display.print(score);
  display.display();
  // Apply friction
  velocityX *= 0.97;
  velocityY *= 0.97;

  delay(30);  // control frame rate
  }

  //Goes back to home screen
  bool replay = false;
  while (replay == false) {
    if (digitalRead(PUSH_BUTTON) == 0) {
      replay = true;
      delay(500);
      break;
    }
  }
  return replay;  
}

void resetAsteroids() {
  for (int i = 0; i < MAX_ASTEROIDS; i++) {
    asteroids[i].x = random(SCREEN_WIDTH);
    asteroids[i].y = random(SCREEN_HEIGHT);
    asteroids[i].dx = random(-1, 2);
    asteroids[i].dy = random(-1, 2);
  }
}

void fireBullet(float shipX, float shipY, float angle) {
  for (int i = 0; i < MAX_BULLETS; i++) {
    if (!bullets[i].active) {
      bullets[i].x = shipX;
      bullets[i].y = shipY;
      bullets[i].dx = cos(angle) * 4;
      bullets[i].dy = sin(angle) * 4;
      bullets[i].active = true;
      break;
    }
  }
}

void drawShip(float x, float y, float angle, int shipSize, Adafruit_SSD1306 &display) {
  // Tip point
  float tipX = x + cos(angle) * shipSize;
  float tipY = y + sin(angle) * shipSize;

  // Left corner (120 degrees from tip)
  float leftX = x + cos(angle + 2.5) * shipSize * 0.6;
  float leftY = y + sin(angle + 2.5) * shipSize * 0.6;

  // Right corner (-120 degrees from tip)
  float rightX = x + cos(angle - 2.5) * shipSize * 0.6;
  float rightY = y + sin(angle - 2.5) * shipSize * 0.6;

  // Draw triangle
  display.drawLine(tipX, tipY, leftX, leftY, SSD1306_WHITE);
  display.drawLine(leftX, leftY, rightX, rightY, SSD1306_WHITE);
  display.drawLine(rightX, rightY, tipX, tipY, SSD1306_WHITE);
}