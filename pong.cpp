#include "pong.h"

#define PUSH_BUTTON 2
#define SECOND_BUTTON 7
#define SPEAKER_PIN 3
#define X_PIN A0
#define Y_PIN A1
#define SWITCH_PIN 4

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

#define PADDLE_WIDTH 16
#define PADDLE_HEIGHT 3
#define PADDLETWO_HEIGHT 15
#define PADDLETWO_WIDTH 3
#define BALL_SIZE 3
#define PADDLE_STEP 2
#define LEFT_THRESHOLD 400
#define DOWN_THRESHOLD 600
#define RIGHT_THRESHOLD 600
#define UP_THRESHOLD 400
#define TIME 50

#define NUM_BRICKS 3
struct Brick {
  int x;
  int y;
  int w;
  int h;
  bool active;
};
Brick bricks[NUM_BRICKS];

bool startGame(Adafruit_SSD1306 &display) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("1: 1 player");
  display.println("2: 2 players");
  display.display();

  bool pressedOne = false;
  bool pressedTwo = false;
  while (pressedOne == false && pressedTwo == false) {
    if (digitalRead(PUSH_BUTTON) == 0) {
      pressedOne = true;
      display.clearDisplay();
      display.setCursor(0, 0);
      display.println("1 player game chosen");
      display.display();
      delay(1500);
    } else if (digitalRead(SECOND_BUTTON) == 0) {
      pressedTwo = true;
      display.clearDisplay();
      display.setCursor(0, 0);
      display.println("2 player game chosen");
      display.println("First to 5 points");
      display.println("wins");
      display.display();
      delay(1500);
    }
    delay(10);
  }

  bool replay = false;
  if (pressedOne == true) {
    onePlayer(display);
    gameOver(display);
    while (replay == false) {
      if (digitalRead(PUSH_BUTTON) == 0) {
        replay = true;
        delay(500);
        break;
      }
    }
  } else if (pressedTwo == true) {
    int winner = twoPlayer(display);
    gameOverTwo(display, winner);
    while (replay == false) {
      if (digitalRead(PUSH_BUTTON) == 0) {
        replay = true;
        delay(500);
        break;
      }
    }
  }
  return replay;
}

int twoPlayer(Adafruit_SSD1306 &display) {
  int paddle1Y = (SCREEN_HEIGHT - PADDLETWO_HEIGHT) / 2;  // Left paddle (Player 1)
  int paddle2Y = (SCREEN_HEIGHT - PADDLETWO_HEIGHT) / 2;  // Right paddle (Player 2)
  #define PADDLE1_X 0
  #define PADDLE2_X SCREEN_WIDTH - PADDLETWO_WIDTH
  float ballX = 64;
  float ballY = 32;
  float ballSpeed = 0.5;
  int ballDX = randomizeStart();
  int ballDY = randomizeStart();
  int p1Score = 0;
  int p2Score = 0;

  while (true) {
    //Move paddles
    paddle1Y = movePaddle1TwoPlayer(paddle1Y);
    paddle2Y = movePaddle2TwoPlayer(paddle2Y);

    // Update ball position
    ballX += ballDX * ballSpeed;
    ballY += ballDY * ballSpeed;

    //If ball bounces on top or bottom
    if (ballY <= 0 || ballY + BALL_SIZE >= SCREEN_HEIGHT) {
      ballDY = -ballDY;
    }

    // Left paddle collision
    if (ballDX < 0 && ballX <= PADDLE1_X + PADDLETWO_WIDTH && ballY + BALL_SIZE >= paddle1Y && ballY <= paddle1Y + PADDLETWO_HEIGHT) {
      ballDX = -ballDX;
      playPaddleSound();
      ballSpeed += 0.05;
      if (ballSpeed > 5.0) {
        ballSpeed = 5.0;
      }
    }

    // Right paddle collision
    if (ballDX > 0 && ballX + BALL_SIZE >= PADDLE2_X && ballY + BALL_SIZE >= paddle2Y && ballY <= paddle2Y + PADDLETWO_HEIGHT) {
      ballDX = -ballDX;
      playPaddleSound();
      ballSpeed += 0.05;
      if (ballSpeed > 5.0) {
        ballSpeed = 5.0;
      }
    }
    if (ballX < 0) {
      p2Score++;
      playWallSound();
      if (p2Score == 5) {
        return 2;
      }
      ballX = 64;
      ballY = 32;
      ballSpeed = 0.5;
      ballDX = randomizeStart();
      ballDY = randomizeStart();
    }

    if (ballX > SCREEN_WIDTH) {
      p1Score++;
      playWallSound();
      if (p1Score == 5){
        return 1;
      }
      ballX = 64;
      ballY = 32;
      ballSpeed = 0.5;
      ballDX = randomizeStart();
      ballDY = randomizeStart();
    }

    //Draw everything
    display.clearDisplay();
    display.fillRect(PADDLE1_X, paddle1Y, PADDLETWO_WIDTH, PADDLETWO_HEIGHT, SSD1306_WHITE);
    display.fillRect(PADDLE2_X, paddle2Y, PADDLETWO_WIDTH, PADDLETWO_HEIGHT, SSD1306_WHITE);
    // Draw ball
    display.fillRect(ballX, ballY, BALL_SIZE, BALL_SIZE, SSD1306_WHITE);

    // Draw score
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.print("P1 Score:");
    display.print(p1Score);
    display.setCursor(68, 0);
    display.print("P2 Score:");
    display.print(p2Score);

    display.display();
  }
}

  void onePlayer(Adafruit_SSD1306 & display) {
    int paddleX = (SCREEN_WIDTH - PADDLE_WIDTH) / 2;
    float ballX = 64;
    float ballY = 32;
    float ballSpeed = 1.0;
    int ballDY = -1;
    int ballDX = randomizeStart();
    int score = 0;
    int lives = 3;
    initBricks();

    while (true) {
      spawnBricks();
      //Move paddle
      paddleX = movePaddle(paddleX);

      // Update ball position
      ballX += ballDX * ballSpeed;
      ballY += ballDY * ballSpeed;

      // Ball bounce on left/right
      if (ballX <= 0 || ballX >= SCREEN_WIDTH - BALL_SIZE) {
        ballDX = -ballDX;
      }

      // Ball hits paddle
      if (ballY >= SCREEN_HEIGHT - PADDLE_HEIGHT - BALL_SIZE) {
        if (ballX + BALL_SIZE >= paddleX && ballX <= paddleX + PADDLE_WIDTH) {
          ballDY = -ballDY;
          score++;
          ballSpeed += 0.1;
          if (ballSpeed > 5.0) {
            ballSpeed = 5.0;
          }
          playPaddleSound();
        } else if (ballY >= 64 - BALL_SIZE) {
          // Missed paddle
          ballX = 64;
          ballY = 32;
          lives--;
          playWallSound();

          if (lives == 0) {
            break;
          }

          ballSpeed = 1.0;
          ballDY = -1;  // Always start moving up
          ballDX = randomizeStart();
        }
      }

      // Ball hits top
      if (ballY <= 0) {
        ballDY = -ballDY;
      }

      //Checks for brick collisions
      for (int i = 0; i < NUM_BRICKS; i++) {
        if (bricks[i].active && ballX + BALL_SIZE >= bricks[i].x && ballX <= bricks[i].x + bricks[i].w && ballY + BALL_SIZE >= bricks[i].y && ballY <= bricks[i].y + bricks[i].h) {

          bricks[i].active = false;
          score += 5;
          ballDY = -ballDY;

          // Play sound
          playPaddleSound();
        }
      }

      // Draw everything
      display.clearDisplay();

      // Draw paddle at bottom
      display.fillRect(paddleX, 64 - PADDLE_HEIGHT, PADDLE_WIDTH, PADDLE_HEIGHT, SSD1306_WHITE);

      // Draw ball
      display.fillRect(ballX, ballY, BALL_SIZE, BALL_SIZE, SSD1306_WHITE);

      drawBricks(display);

      // Draw score
      display.setTextSize(1);
      display.setTextColor(SSD1306_WHITE);
      display.setCursor(10, 0);
      display.print("Score:");
      display.print(score);
      display.setCursor(75, 0);
      display.print("Lives:");
      display.print(lives);

      display.display();

      delay(20);
    }
  }

  void gameOverTwo(Adafruit_SSD1306 & display, int player){
    char myText[30];  // Make sure buffer is large enough
    sprintf(myText, "Player %d wins!", player);
    int textSize = 1;  // or 1, 3, etc

    int textWidth = strlen(myText) * 6 * textSize;
    int textHeight = 8 * textSize;

    int x = (SCREEN_WIDTH - textWidth) / 2;
    int y = (SCREEN_HEIGHT - textHeight) / 2;

    display.clearDisplay();
    display.setTextSize(textSize);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(x, y);
    display.println(myText);
    display.display();
    playGameOverSound();
    display.setTextSize(1);
    display.setCursor(0, SCREEN_HEIGHT - textHeight);
    display.print("Press 1 to go to menu");
    display.display();
  }

  void gameOver(Adafruit_SSD1306 & display) {
    const char *myText = "GAME OVER";
    int textSize = 2;  // or 1, 3, etc

    int textWidth = strlen(myText) * 6 * textSize;
    int textHeight = 8 * textSize;

    int x = (SCREEN_WIDTH - textWidth) / 2;
    int y = (SCREEN_HEIGHT - textHeight) / 2;

    display.clearDisplay();
    display.setTextSize(textSize);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(x, y);
    display.println(myText);
    display.display();
    playGameOverSound();
    display.setTextSize(1);
    display.setCursor(0, SCREEN_HEIGHT - textHeight);
    display.print("Press 1 to go to menu");
    display.display();
  }

  void playGameOverSound() {
    tone(SPEAKER_PIN, NOTE_G4, 500);
    tone(SPEAKER_PIN, NOTE_F4, 500);
    tone(SPEAKER_PIN, NOTE_E4, 500);
  }

  void playPaddleSound() {
    tone(SPEAKER_PIN, NOTE_C4, TIME);
  }

  void playWallSound() {
    tone(SPEAKER_PIN, NOTE_A1, TIME);
  }

  int randomizeStart() {
    int start = random(-1, 2);
    if (start == 0) {
      start = 1;
    }
    return start;
  }

  int movePaddle1TwoPlayer(int paddleY) {
    int yVal = analogRead(Y_PIN);

    //Avoiding the edge
    if (yVal > DOWN_THRESHOLD) {
      paddleY += (PADDLE_STEP - 1);
      if (paddleY > SCREEN_HEIGHT - PADDLETWO_HEIGHT) {
        paddleY = SCREEN_HEIGHT - PADDLETWO_HEIGHT;
      }
    } else if (yVal < UP_THRESHOLD) {
      paddleY -= (PADDLE_STEP - 1);
      if (paddleY < 0) {
        paddleY = 0;
      }
    }
    return paddleY;
  }

  int movePaddle2TwoPlayer(int paddleY) {
    //Move down
    if (digitalRead(PUSH_BUTTON) == 0) {
      paddleY += PADDLE_STEP;
      if (paddleY > SCREEN_HEIGHT - PADDLETWO_HEIGHT) {
        paddleY = SCREEN_HEIGHT - PADDLETWO_HEIGHT;
      }
      delay(5);
    }
    //Move up
    if (digitalRead(SECOND_BUTTON) == 0) {
      paddleY -= PADDLE_STEP;
      if (paddleY < 0) {
        paddleY = 0;
      }
      delay(5);
    }
    return paddleY;
  }

  int movePaddle(int paddleX) {
    int xVal = analogRead(X_PIN);

    if (xVal < LEFT_THRESHOLD) {
      paddleX -= PADDLE_STEP;
      if (paddleX < 0) paddleX = 0;
    } else if (xVal > RIGHT_THRESHOLD) {
      paddleX += PADDLE_STEP;
      if (paddleX > SCREEN_WIDTH - PADDLE_WIDTH) {
        paddleX = SCREEN_WIDTH - PADDLE_WIDTH;
      }
    }

    return paddleX;
  }

  // Initialize bricks
  void initBricks() {
    for (int i = 0; i < NUM_BRICKS; i++) {
      bricks[i].w = 10;
      bricks[i].h = 4;
      bricks[i].active = false;
    }
  }

  void spawnBricks() {
    for (int i = 0; i < NUM_BRICKS; i++) {
      if (!bricks[i].active && random(0, 1000) < 5) {
        bricks[i].x = random(0, SCREEN_WIDTH - bricks[i].w);
        bricks[i].y = random(10, SCREEN_HEIGHT / 2);
        bricks[i].active = true;
      }
    }
  }

  void drawBricks(Adafruit_SSD1306 & display) {
    for (int i = 0; i < NUM_BRICKS; i++) {
      if (bricks[i].active) {
        display.fillRect(bricks[i].x, bricks[i].y, bricks[i].w, bricks[i].h, SSD1306_WHITE);
      }
    }
  }