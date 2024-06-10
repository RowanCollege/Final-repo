#include <Adafruit_CircuitPlayground.h>

// Define game variables
int score = 0;
const int maxCommands = 10;
char commands[maxCommands]; // using characters becasue they are new and make it easy to call to later.
unsigned long commandStartTime;
const unsigned long commandTimeout = 2000; 
int soundP = 85;
int timeDropNum = 100;
int timeDrop = commandTimeout;
bool endlessMode = false;
bool prevSlideSwitchState = CircuitPlayground.slideSwitch(); // so slide switch reads a change 
void setup() {
  CircuitPlayground.begin();
  randomSeed(analogRead(0)); // Seed the random number generator
  Serial.begin(9600);
  while (!CircuitPlayground.leftButton() && !CircuitPlayground.rightButton()) { //showing which button is endless or 10 game count via leds
        CircuitPlayground.setPixelColor(0, 255, 0, 0);
        CircuitPlayground.setPixelColor(1, 255, 0, 0);
        CircuitPlayground.setPixelColor(2, 255, 0, 0);
        CircuitPlayground.setPixelColor(3, 255, 0, 0);
        CircuitPlayground.setPixelColor(4, 255, 0, 0);
        CircuitPlayground.setPixelColor(5, 255, 0, 255);
        CircuitPlayground.setPixelColor(6, 255, 0, 255);
        CircuitPlayground.setPixelColor(7, 255, 0, 255);
        CircuitPlayground.setPixelColor(8, 255, 0, 255);
        CircuitPlayground.setPixelColor(9, 255, 0, 255);

        delay(10); 
    }

    // Determine mode based on button press
    endlessMode = CircuitPlayground.rightButton();
}

void loop() {
  startGame();
}

void startGame() {
  score = 0;
  if (!endlessMode) { // 10 game mode (from the first snippet)
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
    else { 
  int loopCount = endlessMode ? 0 : maxCommands;  // Loop indefinitely 

    do {
        generateCommand(score); // Use score as index for endless mode makes code simpler
        if (!performCommand(commands[score % maxCommands])) { // Wrap around for endless mode
            gameOver();
            return;
        }
        score++;
        indicateScore(score);
    } while (endlessMode || loopCount++ < maxCommands); // just to keep track of loops 

    winGame();
}
}
void generateCommand(int index) { // indez commands to call to them later in the switch case
  int commandType = random(0, 5);
  switch (commandType) {
    case 0:
      commands[index] = 'L'; // Left button
      timeDrop = timeDrop - timeDropNum; //make it progressivley harder 
      break;
    case 1:
      commands[index] = 'R'; // Right button
      timeDrop = timeDrop - timeDropNum;
      break;
    case 2:
      commands[index] = 'S'; // Slide switch
      timeDrop = timeDrop - timeDropNum;
      break;
    case 3:
      commands[index] = 'M'; // Microphone
      timeDrop = timeDrop - timeDropNum;
      break;
    case 4:
      commands[index] = 'A'; // Accelerometer
      timeDrop = timeDrop - timeDropNum;
      break;
  }
  indicateCommand(commands[index]);
}

bool performCommand(char command) { // this was split out as a suggestions i found it allowed me to index earlier and then call again here for the actual function.
  commandStartTime = millis();
  switch (command) {
    case 'L':
      return waitForLeftButton();
    case 'R':
      return waitForRightButton();
    case 'S':
      return waitForSlideSwitchChange();
    case 'M':
      return waitForClap();
    case 'A':
      return waitForShake();
    default:
      return false;
  }
}

bool waitForLeftButton() { // waits for button
  while (millis() - commandStartTime < timeDrop) {
    if (CircuitPlayground.leftButton()) {
      playSuccessSound();
      return true;
    }
  }
  playFailSound();
  return false;
}

bool waitForRightButton() {
  while (millis() - commandStartTime < timeDrop) {
    if (CircuitPlayground.rightButton()) {
      playSuccessSound();
      return true;
    }
  }
  playFailSound();
  return false;
}

bool waitForSlideSwitchChange() { // check for  change in the slide switch
  while (millis() - commandStartTime < timeDrop) {
    bool currentSlideSwitchState = CircuitPlayground.slideSwitch();
    if (currentSlideSwitchState != prevSlideSwitchState) {
      prevSlideSwitchState = currentSlideSwitchState;
      playSuccessSound();
      return true;
    }
  }
  playFailSound();
  return false;
}

bool waitForClap() {
  while (millis() - commandStartTime < timeDrop) {
    float soundLevel = CircuitPlayground.mic.soundPressureLevel(10);
    Serial.println(soundLevel);
    if (soundLevel > soundP) {
      playSuccessSound();
      return true;
    }
  }
  playFailSound();
  return false;
}

bool waitForShake() {
  while (millis() - commandStartTime < timeDrop) {
    if (CircuitPlayground.motionX() > 17.5 || CircuitPlayground.motionY() > 17.5 || CircuitPlayground.motionZ() > 17.5) { // would have used total accelration but found this adn decided to try it it is less code so :^)
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
    CircuitPlayground.setPixelColor(i, 55, 55, 55); // White for score going around the board doesn't do anything past ten on endless
  }
  CircuitPlayground.strip.show();
  delay(500);
}

void playSuccessSound() {
  CircuitPlayground.playTone(1000, 200); 
}

void playFailSound() {
  CircuitPlayground.playTone(200, 200);
}

void gameOver() {
  CircuitPlayground.clearPixels();
  for (int i = 0; i < 10; i++) {
    CircuitPlayground.setPixelColor(i, 255, 0, 0); // Red for game over
  }
  CircuitPlayground.strip.show();
  timeDrop = commandTimeout;
  CircuitPlayground.playTone(200, 500); 
  delay(1000);
}

void winGame() {
  CircuitPlayground.clearPixels();
  for (int i = 0; i < 10; i++) {
    CircuitPlayground.setPixelColor(i, 0, 0, 255); // Blue for win
  }
  CircuitPlayground.strip.show();
  CircuitPlayground.playTone(1000, 500); 
  timeDrop = commandTimeout;
  delay(1000);
}
