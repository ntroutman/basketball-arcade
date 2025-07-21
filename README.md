# 🏀 Desktop Basketball Arcade

A 3D printed desktop basketball arcade game powered by Arduino Nano with LED score display and IR ball detection.

## 📋 Overview

This project creates a miniature basketball arcade game featuring:
- **90-second timed gameplay** with countdown display
- **Automatic ball detection** using IR sensor
- **LED 7-segment display** showing score, high score, and time remaining
- **Game over animation** with scrolling "GAME OVER" text
- **High score tracking** (with EEPROM storage capability)
- **Sleep mode** for power conservation

## 🎮 Game Features

### Game States
- **IDLE**: Displays high score and dashes, waiting for start button
- **PLAYING**: Active 15-second game with score tracking and countdown
- **GAME OVER**: Scrolling animation followed by return to idle
- **SLEEPING**: Low power mode activated after inactivity

### Display Layout
The 8-digit 7-segment display shows:
- **Digits 0-2**: Current score (3 digits)
- **Digits 3-5**: High score (3 digits) 
- **Digits 6-7**: Time remaining (2 digits with decimal point when under 10 seconds)

### Gameplay
1. Press start button to begin 15-second game
2. 3-2-1 countdown appears before game starts
3. Shoot basketballs - IR sensor detects successful shots
4. Score increments automatically for each detected ball
5. Time remaining counts down with tenths of seconds precision
6. Game over animation plays when time expires
7. New high scores are automatically saved

## 🔧 Hardware Requirements

### Components
- **Arduino Nano** (ATmega328P)
- **MAX7219 LED Driver** with 8-digit 7-segment display
- **IR sensor/photointerrupter** for ball detection
- **Push button** for game start
- **Jumper wires** and breadboard/PCB

### Pin Configuration
```cpp
// LED Display (MAX7219)
const int dataPin = 8;    // DIN
const int clkPin = 6;     // CLK  
const int loadPin = 7;    // LOAD/CS

// Input pins
const int startButtonPin = 5;      // Start button (INPUT_PULLUP)
const int ballDetectorPin = 4;     // IR ball sensor (INPUT_PULLUP)
```

### Wiring Diagram
```
Arduino Nano    MAX7219 Display    Components
Pin 8    -----> DIN               
Pin 6    -----> CLK               
Pin 7    -----> LOAD/CS           
5V       -----> VCC               
GND      -----> GND               

Pin 5    -----> Start Button -----> GND
Pin 4    -----> IR Sensor Signal
Pin 13   -----> Onboard LED (debug)
```

## 📦 Dependencies

The project uses PlatformIO with the following libraries:

```ini
lib_deps = 
    mathertel/OneButton@^2.6.1
    wayoda/LedControl@^1.0.6
    arduino-libraries/Arduino Low Power@^1.2.2
```

## 🚀 Getting Started

### Prerequisites
- [PlatformIO](https://platformio.org/) installed
- Arduino Nano or compatible board
- Required electronic components (see hardware section)

### Installation
1. Clone this repository:
   ```bash
   git clone https://github.com/ntroutman/basketball-arcade.git
   cd basketball-arcade
   ```

2. Open in PlatformIO or VS Code with PlatformIO extension

3. Build and upload the main basketball game:
   ```bash
   pio run -e basketball --target upload
   ```

### Testing
To test the LED display functionality separately:
```bash
pio run -e led-control-test --target upload
```

## 🏗️ Project Structure

```
basketball-arcade/
├── src/
│   ├── basketball/              # Main game code
│   │   ├── main.cpp            # Core game logic and state machine
│   │   ├── GameOverAnimation.h  # Game over animation class
│   │   └── GameOverAnimation.cpp
│   └── led-control-test/        # LED display testing
│       └── main.cpp            # Display test patterns
├── platformio.ini              # PlatformIO configuration
└── README.md                   # This file
```

## 🎯 Game Logic

The game implements a finite state machine with four states:

### State Transitions
```
IDLE → PLAYING (start button pressed)
PLAYING → GAMEOVER (time expires)
GAMEOVER → IDLE (animation completes)
IDLE → SLEEPING (inactivity timeout)
SLEEPING → IDLE (start button pressed)
```

### Key Functions
- `gameloop_idle()`: Display high score, wait for start
- `gameloop_playing()`: Handle scoring and time countdown  
- `gameloop_gameover()`: Run scrolling animation
- `scoreSensorTriggered()`: Increment score on ball detection
- `displayScore()`: Format and show 3-digit scores
- `displayTimeLeft()`: Show countdown with decimal precision

## 🔧 Customization

### Game Settings
```cpp
const int gameLengthSeconds = 15;  // Game duration
```

### Display Settings
```cpp
lc->setIntensity(0, 2);           // Brightness (0-15)
gameOverAnimation->setScrollInterval(300); // Animation speed (ms)
```

### Hardware Pins
Modify pin assignments in the constants section of `main.cpp` to match your wiring.

## 🐛 Troubleshooting

### Common Issues
1. **Display not working**: Check MAX7219 wiring and power connections
2. **Ball detection issues**: Verify IR sensor placement and sensitivity
3. **Button not responding**: Ensure pull-up resistor configuration
4. **Compilation errors**: Verify all library dependencies are installed

### Debug Features
- Onboard LED mirrors ball detector state
- Serial output shows game state transitions and scoring
- LED test program helps verify display functionality

## 🤝 Contributing

1. Fork the repository
2. Create a feature branch (`git checkout -b feature/amazing-feature`)
3. Commit your changes (`git commit -m 'Add amazing feature'`)
4. Push to the branch (`git push origin feature/amazing-feature`)
5. Open a Pull Request

## 📄 License

This project is open source. Feel free to modify and distribute according to your needs.

## 🎨 3D Printing

This project is designed to work with a 3D printed basketball arcade cabinet. The electronics fit inside the cabinet with:
- Display visible through the front panel
- IR sensor positioned to detect balls entering the hoop
- Start button accessible on the front or side
- Arduino and wiring hidden inside the cabinet

## 🔮 Future Enhancements

- [ ] EEPROM high score persistence
- [ ] Multiple difficulty levels
- [ ] Sound effects with buzzer
- [ ] WiFi connectivity for online leaderboards
- [ ] Multiple game modes (speed rounds, accuracy challenges)
- [ ] RGB LED strip integration
- [ ] Mobile app integration

---

**Enjoy your desktop basketball arcade! 🏀**
