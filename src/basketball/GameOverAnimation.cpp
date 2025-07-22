#include "GameOverAnimation.h"

// 7-segment display patterns for "GAME OVER" characters
// Each byte represents the segments: 
//  7 6 5 4 3 2 1 0
// DP A B C D E F G 
//      A(6)
//  F(1)   B(5)
//      G(0)
//  E(2)   C(4)
//      D(3)    DP (8)

const byte GameOverAnimation::_gameOverChars[] = {
  //76543210
  0b01011111, // G
  0b01110111, // A
  0b00010101, // M (approximation - looks like "n")
  0b01001111, // E
  0b00000000, // Space
  0b01111110, // O
  0b00111110, // V (approximation)
  0b01001111, // E
  0b00000101  // R (approximation)
};

// ----- Constructor -----
GameOverAnimation::GameOverAnimation(LedControl* ledControl, const int digitIndices[6]) 
  : _animationState(IDLE), _scrollPosition(0), _lastUpdateTime(0), _ledControl(ledControl) {
  
  // Copy the digit indices array
  for (int i = 0; i < 6; i++) {
    _digitIndices[i] = digitIndices[i];
  }
}

// ----- Public Methods -----

void GameOverAnimation::start() {
  _animationState = AnimationState::SCROLLING;
  _scrollPosition = 0;  // Start from the rightmost position
  _lastUpdateTime = millis();
  clearDisplay();
  updateDisplay();
}

void GameOverAnimation::tick() {
  if (_animationState != AnimationState::SCROLLING) {
    return;
  }

  unsigned long currentTime = millis();

  // Check if enough time has passed for the next scroll step
  if (currentTime - _lastUpdateTime >= _scrollInterval) {
    _lastUpdateTime = currentTime;
    
    // Move to the next scroll position
    _scrollPosition++;
    
    // Check if we've scrolled all the way through
    // We need to scroll until all characters have passed through the display
    // Total positions = _gameOverLength + 6 (to clear the display completely)
    if (_scrollPosition >= _gameOverLength + 6) {
      _animationState = AnimationState::COMPLETED;
      clearDisplay();
      return;
    }
    
    updateDisplay();
  }
}

// ----- Private Methods -----

void GameOverAnimation::updateDisplay() {
  // Clear all digits first
  clearDisplay();
  
  // Display characters based on current scroll position
  for (int displayPos = 0; displayPos < 6; displayPos++) {
    // Calculate which character should be at this display position
    int charIndex = _scrollPosition - (5 - displayPos);
    
    if (charIndex >= 0 && charIndex < _gameOverLength) {
      // Display the character at this position
      _ledControl->setRow(0, _digitIndices[displayPos], _gameOverChars[charIndex]);
    }
  }
}

void GameOverAnimation::clearDisplay() {
  for (int i = 0; i < 6; i++) {
    _ledControl->setRow(0, _digitIndices[i], 0b00000000);
  }
}
