#ifndef GameOverAnimation_h
#define GameOverAnimation_h

#include <Arduino.h>
#include <LedControl.h>

class GameOverAnimation {
public:
  // ----- Constructor -----
  GameOverAnimation(LedControl* ledControl, const int digitIndices[6]);

  // ----- Public Methods -----
  void tick();  // Update the animation state
  void start(); // Reset the animation to the beginning
  bool isCompleted() {
    return _animationState == AnimationState::COMPLETED;
  }
  void setScrollInterval(unsigned long intervalMs) {
    _scrollInterval = intervalMs;
  }

private:
  // ----- Private Members -----
  enum AnimationState { IDLE, SCROLLING, COMPLETED };

  AnimationState _animationState = IDLE; // Current state of the animation
  int _scrollPosition;                   // Current scroll position (0 = rightmost)
  unsigned long _lastUpdateTime;         // Last time the animation was updated
  unsigned long _scrollInterval = 300;   // Duration between scroll steps in milliseconds
  
  LedControl* _ledControl;               // Pointer to the LED control object
  int _digitIndices[6];                  // Array of digit indices from left to right
  
  // "GAME OVER" character patterns for 7-segment display
  static const byte _gameOverChars[];
  static const int _gameOverLength = 9;  // "GAME OVER" (including space)
  
  void updateDisplay();
  void clearDisplay();
};

#endif
