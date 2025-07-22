#include <Arduino.h>
#include <LedControl.h>
#include <OneButton.h>
#include <EEPROM.h>

#include "basketball/GameOverAnimation.h"

// LED Control pins
// Pins: DIN, CLK, LOAD (CS)
const int clkPin = 6;
const int loadPin = 7;
const int dataPin = 8;

const int startButtonPin = 5;
const int ballDectectorPin = 4;

const int gameLengthSeconds = 15;

// EEPROM addresses
const int EEPROM_HIGH_SCORE_ADDR = 0;
const int EEPROM_MAGIC_NUMBER_ADDR = 4;
const int EEPROM_MAGIC_NUMBER = 0xBA5E;  // "BASE" in hex - magic number to verify EEPROM data validity

// Digits from least significant to most significant.
// 3-Digit 7-Segment Display goes from left (Dig 1) to right (Dig 3)
const int scoreDigits[] = {0, 1, 2};
const int highScoreDigits[] = {3, 4, 5};
const int timeRemainingDigits[] = {6, 7};

// Combined array of all 6 digits for game over animation (left to right)
const int gameOverDigits[] = {3, 4, 5, 0, 1, 2};

enum GameState { IDLE, PLAYING, GAMEOVER, SLEEPING };

LedControl *lc;
OneButton startButton;
OneButton scoreButton;  // not really a button but an IR sensor

GameState currentState = IDLE;
int highScore = 0;
int currentScore = 0;
unsigned long gameEndTime = 0;
GameOverAnimation* gameOverAnimation = nullptr;

void initLedControl();
void gameloop_idle();
void gameloop_playing();
void gameloop_gameover();
void gameloop_sleeping();

void loadHighScore();
void saveHighscore(int newHighScore);
void resetHighScore();
void startButtonPressed();
void scoreSensorTriggered();

void transitionStateToIdle();
void transitionStateToPlaying();
void transitionStateToGameOver();
void transitionStateToSleeping();

void wakeUp();
void displayScore(int score, const int digits[]);
void displayTimeLeft(unsigned long timeInMillis);

void debugCurrentGameState();

void setup() {
  pinMode(13, OUTPUT);  // Onboard LED for debugging

  Serial.begin(9600);
  Serial.println("Basketball Arcade Game Starting...");

  pinMode(startButtonPin, INPUT_PULLUP);  // Start button pin

  initLedControl();
  Serial.println("LED Control Initialized");

  // Initialize start button
  startButton.setup(startButtonPin, INPUT_PULLUP, true);
  startButton.attachClick(startButtonPressed);

  // Initialize score button (IR sensor)
  scoreButton.setup(ballDectectorPin, INPUT_PULLUP, true);
  scoreButton.attachPress(scoreSensorTriggered);
  scoreButton.setDebounceMs(10);

  // Load high score from EEPROM or set to 0 if not available
  loadHighScore();
  Serial.print("High Score Loaded: ");
  Serial.println(highScore);

  // Initialize game state
  currentState = IDLE;
  Serial.println("Game State Initialized: ");
  debugCurrentGameState();

  Serial.println("Setup Complete. Ready to start the game.");
}

void initLedControl() {
  lc = new LedControl(dataPin, clkPin, loadPin, 1);
  lc->shutdown(0, false);
  lc->setScanLimit(0, 7);
  lc->setIntensity(0, 2);
  lc->clearDisplay(0);
  
  // Initialize game over animation after LED control is set up
  gameOverAnimation = new GameOverAnimation(lc, gameOverDigits);
  gameOverAnimation->setScrollInterval(300); // Set scroll speed to 300ms per step
}

void loadHighScore() {
  // Check if EEPROM has been initialized with valid data
  int magicNumber;
  EEPROM.get(EEPROM_MAGIC_NUMBER_ADDR, magicNumber);
  
  if (magicNumber == EEPROM_MAGIC_NUMBER) {
    // EEPROM has valid data, load the high score
    EEPROM.get(EEPROM_HIGH_SCORE_ADDR, highScore);
    Serial.print("Loaded High Score from EEPROM: ");
    Serial.println(highScore);
  } else {
    // EEPROM not initialized or corrupted, set default high score
    highScore = 0;
    Serial.println("EEPROM not initialized. Setting default high score to 0.");
    
    // Initialize EEPROM with default values
    EEPROM.put(EEPROM_HIGH_SCORE_ADDR, highScore);
    EEPROM.put(EEPROM_MAGIC_NUMBER_ADDR, EEPROM_MAGIC_NUMBER);
    Serial.println("EEPROM initialized with default values.");
  }
}

void saveHighscore(int newHighScore) {
  highScore = newHighScore;
  
  // Save the new high score to EEPROM
  EEPROM.put(EEPROM_HIGH_SCORE_ADDR, highScore);
  
  // Ensure magic number is still valid (in case this is the first save)
  EEPROM.put(EEPROM_MAGIC_NUMBER_ADDR, EEPROM_MAGIC_NUMBER);
  
  Serial.print("High Score saved to EEPROM: ");
  Serial.println(highScore);
}

void resetHighScore() {
  // Reset high score to 0 and save to EEPROM
  highScore = 0;
  EEPROM.put(EEPROM_HIGH_SCORE_ADDR, highScore);
  EEPROM.put(EEPROM_MAGIC_NUMBER_ADDR, EEPROM_MAGIC_NUMBER);
  
  Serial.println("High Score reset to 0 and saved to EEPROM.");
}

unsigned long timeOfNextDebug = 1000;
void loop() {
  digitalWrite(LED_BUILTIN, digitalRead(ballDectectorPin));

  if (millis() > timeOfNextDebug) {
    timeOfNextDebug = millis() + 5000;  // Update next debug time
    debugCurrentGameState();
  }

  switch (currentState) {
    case IDLE:
      gameloop_idle();
      break;
    case PLAYING:
      gameloop_playing();
      break;
    case GAMEOVER:
      gameloop_gameover();
      break;
    case SLEEPING:
      gameloop_sleeping();
      break;

    default:
      break;
  }

  // You can implement other code in here or just wait a while
  delay(10);
}

//
// SLEEPING
//

void gameloop_sleeping() { 
  // do nothing, just wait for start button press
  // or to go to sleep
  startButton.tick(); 
}

//
// IDLE
//

void gameloop_idle() {
  displayScore(highScore, highScoreDigits);

  for (int i = 0; i < 3; i++) {
    lc->setChar(0, scoreDigits[i], '-', false);
  }

  for (int i = 0; i < 2; i++) {
    lc->setChar(0, timeRemainingDigits[i], '-', false);
  }

  startButton.tick();
}

void transitionStateToPlaying() {
  Serial.println("Start Button Pressed. Transitioning to PLAYING state.");
  currentScore = 0;  // Reset score for new game

  // add a bit of extra time so that we actually see the first time remaining
  unsigned long gameLengthMillis = 1000UL * ((unsigned long) gameLengthSeconds) + 250;
  
  //displayTimeLeft(gameLengthMillis);
  debugCurrentGameState();

  // Clear the score display
  for (int i = 0; i < 3; i++) {
    lc->setChar(0, scoreDigits[i], '-', false);
  }

  // Count down from 3 to 0 on the time remaining display to indicate game start
  lc->setChar(0, timeRemainingDigits[0], ' ', false);
  for (int i = 0; i <= 3; i++) {
    lc->setChar(0, timeRemainingDigits[1], '0' + (3-i), false);
    delay(1000);
  }
  lc->setChar(0, timeRemainingDigits[1], ' ', false);
  delay(250);

  gameEndTime = millis() + gameLengthMillis;  
  displayTimeLeft(gameLengthMillis);
  displayScore(currentScore, scoreDigits);

  currentState = PLAYING;  // can't transition until everthing is set up
}

void transitionStateToSleeping() {
  Serial.println("Transitioning to SLEEPING state.");

  lc->clearDisplay(0);  // Clear the display
  lc->shutdown(0, true);

  currentState = SLEEPING;
}

//
// PLAYING
//

void gameloop_playing() {
  scoreButton.tick();

  if (millis() >= gameEndTime) {
    transitionStateToGameOver();
    return;
  }

  unsigned long timeRemaining = gameEndTime - millis();

  displayScore(currentScore, scoreDigits);
  displayTimeLeft(timeRemaining);
}

void transitionStateToGameOver() {
  Serial.println("Game Over! Time's up.");

  if (currentScore > highScore) {
    saveHighscore(currentScore);
  }

  currentState = GAMEOVER;
  gameOverAnimation->start();  // Start the game over animation
}

//
// GAMEOVER
//

void gameloop_gameover() {
  if (gameOverAnimation->isCompleted()) {
    Serial.println("Game Over Animation Completed.");
    transitionStateToIdle();
    return;
  }

  gameOverAnimation->tick();  // Update the game over animation
}

void transitionStateToIdle() {
  currentState = IDLE;  // Transition back to IDLE state after animation
}

//
// Button Handlers
//

void startButtonPressed() {
  switch (currentState) {
    case IDLE:
      transitionStateToPlaying();
      break;
    case SLEEPING:
      wakeUp();
      break;
    default:
      // do nothing
      break;
  }
}

void scoreSensorTriggered() {
  switch (currentState) {
    case PLAYING:
      currentScore++;
      Serial.print("Score Incremented: ");
      Serial.println(currentScore);
      break;
    default:
      break;
  }
}

void wakeUp() {
  Serial.println("Waking up from SLEEPING state.");
  currentState = IDLE;  // Transition back to IDLE state
  loadHighScore();
  displayScore(highScore, highScoreDigits);
}

//
// LED Control Display Functions
//

void displayScore(int score, const int digits[3]) {
  int hundreds = (score / 100) % 10;
  int tens = (score / 10) % 10;
  int ones = score % 10;

  if (score < 100) {
    lc->setChar(0, digits[0], ' ', false);
  } else {
    lc->setChar(0, digits[0], '0' + hundreds, false);
  }

  if (score < 10) {
    lc->setChar(0, digits[1], ' ', false);
  } else {
    lc->setChar(0, digits[1], '0' + tens, false);
  }

  lc->setChar(0, digits[2], '0' + ones, false);
}


void displayTimeLeft(unsigned long timeInMillis) {
  int tenthsLeft = timeInMillis / 100; 
  int tenths = (tenthsLeft  % 10);
  int tens = (tenthsLeft / 100) % 10;
  int ones = (tenthsLeft / 10) % 10;

  if (tens > 0) {
    lc->setChar(0, timeRemainingDigits[0], '0' + tens, false);
    lc->setChar(0, timeRemainingDigits[1], '0' + ones, false);
  } else {
    lc->setChar(0, timeRemainingDigits[0], '0' + ones, true);
    lc->setChar(0, timeRemainingDigits[1], '0' + tenths, false);
  }
}

//
// Debug Utilities
//

void debugCurrentGameState() {
  unsigned long currentTime = millis();

  const char *stateName = "UNKNOWN";
  switch (currentState) {
    case IDLE:
      stateName = "IDLE";
      break;
    case PLAYING:
      stateName = "PLAYING";
      break;
    case GAMEOVER:
      stateName = "GAMEOVER";
      break;
    case SLEEPING:
      stateName = "SLEEPING";
      break;
  }

  Serial.println("--------------------");
  Serial.print("Debugging Game State at ");
  Serial.println(currentTime);
  Serial.print("Current State: ");
  Serial.println(stateName);
  Serial.print("Current Score: ");
  Serial.println(currentScore);
  Serial.print("Game End Time: ");
  Serial.println(gameEndTime);
  unsigned long timeRemaining = gameEndTime - currentTime;
  Serial.print("Time Remaining: ");
  Serial.println(timeRemaining);
}
