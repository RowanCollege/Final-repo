#include <Adafruit_CircuitPlayground.h>

// Define game variables
int score = 0;
const int maxCommands = 10;
char commands[maxCommands];
unsigned long commandStartTime;
const unsigned long commandTimeout = 2000; // 2 seconds for each command
int soundP = 75;

void setup() {
  CircuitPlayground.begin();
  randomSeed(analogRead(0)); // Seed the random number generator
  Serial.begin(9600);
}

void loop() {
  startGame();
}

void startGame() {
  score = 0;
  for (int i = 0; i < maxCommands; i++) {
    generateCommand(i);
    if (!performCommand(commands[i])) {
      gameOver();
      return;
    }
    score++;
    indicateScore(score);
  }
  winGame();
}

void generateCommand(int index) {
  int commandType = random(0, 3);
  switch (commandType) {
    case 0:
      commands[index] = 'L'; // Left button
      break;
    case 1:
      commands[index] = 'R'; // Right button
      break;
    case 2:
      commands[index] = 'S'; // Slide switch
      break;
    case 3:
      commands[index] = 'M'; // Microphone
      break;
    case 4:
      commands[index] = 'A'; // Accelerometer
      break;
  }
  indicateCommand(commands[index]);
}

bool performCommand(char command) {
  commandStartTime = millis();
  switch (command) {
    case 'L':
      return waitForLeftButton();
    case 'R':
      return waitForRightButton();
    case 'S':
      return waitForSlideSwitch();
    case 'M':
      return waitForClap();
    case 'A':
      return waitForShake();
    default:
      return false;
  }
}

bool waitForLeftButton() {
  while (millis() - commandStartTime < commandTimeout) {
    if (CircuitPlayground.leftButton()) {
      playSuccessSound();
      return true;
    }
  }
  playFailSound();
  return false;
}

bool waitForRightButton() {
  while (millis() - commandStartTime < commandTimeout) {
    if (CircuitPlayground.rightButton()) {
      playSuccessSound();
      return true;
    }
  }
  playFailSound();
  return false;
}

bool waitForSlideSwitch() {
  while (millis() - commandStartTime < commandTimeout) {
    if (CircuitPlayground.slideSwitch()) {
      playSuccessSound();
      return true;
    }
  }
  playFailSound();
  return false;
}

bool waitForClap() {
  float soundLevel = CircuitPlayground.mic.soundPressureLevel(10);
  Serial.println(soundLevel);
  while (millis() - commandStartTime < commandTimeout) {
      if (soundLevel > soundP) {
      playSuccessSound();
      return true;
    }
  }
  playFailSound();
  return false;
}

bool waitForShake() {
  while (millis() - commandStartTime < commandTimeout) {
    if (CircuitPlayground.motionX() > 1.5 || CircuitPlayground.motionY() > 1.5 || CircuitPlayground.motionZ() > 1.5) {
      playSuccessSound();
      return true;
    }
  }
  playFailSound();
  return false;
}

void indicateCommand(char command) {
  CircuitPlayground.clearPixels();
  switch (command) {
    case 'L':
      CircuitPlayground.setPixelColor(0, 255, 0, 0); // Red for Left Button
      break;
    case 'R':
      CircuitPlayground.setPixelColor(1, 0, 255, 0); // Green for Right Button
      break;
    case 'S':
      CircuitPlayground.setPixelColor(2, 0, 0, 255); // Blue for Slide Switch
      break;
    case 'M':
      CircuitPlayground.setPixelColor(3, 255, 255, 0); // Yellow for Microphone
      break;
    case 'A':
      CircuitPlayground.setPixelColor(4, 0, 255, 255); // Cyan for Accelerometer
      break;
  }
  CircuitPlayground.strip.show();
}

void indicateScore(int score) {
  CircuitPlayground.clearPixels();
  for (int i = 0; i < score; i++) {
    CircuitPlayground.setPixelColor(i, 0, 255, 0); // Green for score
  }
  CircuitPlayground.strip.show();
}

void playSuccessSound() {
  CircuitPlayground.playTone(1000, 200); // High tone
}

void playFailSound() {
  CircuitPlayground.playTone(200, 200); // Low tone
}

void gameOver() {
  CircuitPlayground.clearPixels();
  for (int i = 0; i < 10; i++) {
    CircuitPlayground.setPixelColor(i, 255, 0, 0); // Red for game over
  }
  CircuitPlayground.strip.show();
  CircuitPlayground.playTone(200, 500); // Long low tone
  delay(1000);
}

void winGame() {
  CircuitPlayground.clearPixels();
  for (int i = 0; i < 10; i++) {
    CircuitPlayground.setPixelColor(i, 0, 0, 255); // Blue for win
  }
  CircuitPlayground.strip.show();
  CircuitPlayground.playTone(1000, 500); // Long high tone
  delay(1000);
}
