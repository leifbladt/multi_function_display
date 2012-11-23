#include <LiquidCrystal.h>

const int tempPin = A0;
const int buttonPin = 2;

long lastMeasureTime = 0;

class LM35 {
  // TODO Make those private
  static const int BUFFER_SIZE = 3;
  int buffer[BUFFER_SIZE];
  int bufferPos;
  int _pin;

public:
  LM35(const int pin) {
    _pin = pin;
    bufferPos = 0;
  }

  float getTemp() {
    buffer[bufferPos] = analogRead(_pin);
    bufferPos = (bufferPos + 1) % BUFFER_SIZE;

    int sum = 0;
    for (int i = 0; i < BUFFER_SIZE; i++) {
      sum += buffer[i];
    }

    return formatOutput(sum / BUFFER_SIZE * 0.488);
  }

private:
  float formatOutput(const float input) {
    return round(input * 10) / float(10);
  }
};

// TODO Simplify button class
class Button {
  // TODO Make those private
  int _pin;
  int lastButtonState;   // the previous reading from the input pin

public:
  Button(const int pin) {
    pinMode(_pin, INPUT);
    _pin = pin;
    lastButtonState = LOW;
  }

  boolean released() {
    const int buttonState = digitalRead(_pin);

    const boolean released = (buttonState == LOW) && (lastButtonState == HIGH);
    lastButtonState = buttonState;
    delay(50); // TODO Debounce without delay

    return released;
  }
};

class Display {
  // TODO Make thos private
  static const int PAGES = 2;
  LiquidCrystal _lcd;
  LM35 _lm35;
  int _currentPage;

public:
  // TODO Default constructor?
  Display(int tmp) : _lcd(12, 11, 6, 5, 4, 3), _lm35(tempPin) {
    _lcd.begin(16, 2);
    _currentPage = 0;
  }

  void switchPage() {
    _currentPage = (_currentPage + 1) % PAGES;
    clear();
    render();
  }

  void clear() {
    _lcd.clear();
    _lcd.setCursor(0, 0);
  }

  void render() {
    if (_currentPage == 0) {
      _lcd.setCursor(0, 0);
      _lcd.print("Temp 1");
      _lcd.setCursor(11, 0);
      _lcd.print(_lm35.getTemp());
    } 
    else {
      _lcd.setCursor(0, 0);
      _lcd.print("Hello World!");
    }
  }
};


Button button(buttonPin);
Display display(0);

void setup() {
  display.render();
}

void loop() {
  if (button.released()) {
    display.switchPage();
  }

  // Update screens every 3 seconds
  if ((millis() - lastMeasureTime) >= 3000) {
    display.render();
    lastMeasureTime = millis();
  }
}
