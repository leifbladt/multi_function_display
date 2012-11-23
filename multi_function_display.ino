#include <LiquidCrystal.h>

const int tempPin = A0;
const int buttonPin = 2;
const int PAGES = 2;

long lastTempTime = 0;
long lastSwitch = 0;
int currentPage = 0;

class LM35 {
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
    buffer[bufferPos] = analogRead(tempPin);
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
  int _pin;
  int lastButtonState;   // the previous reading from the input pin

public:
  Button(const int pin) {
    _pin = pin;
    lastButtonState = LOW;
  }

  boolean released() {
    const int buttonState = digitalRead(buttonPin);

    const boolean released = (buttonState == LOW) && (lastButtonState == HIGH);
    lastButtonState = buttonState;
    delay(50); // TODO Debounce without delay
    
    return released;
  }
};

LiquidCrystal lcd(12, 11, 6, 5, 4, 3);
LM35 lm35(tempPin);
Button button(buttonPin);

void setup() {
  pinMode(buttonPin, INPUT);
  lcd.begin(16, 2);
  delay(500);
  Serial.begin(9600);
}

void loop() {
   if (button.released()) {
    currentPage = (currentPage + 1) % PAGES;
    Serial.println("released");
  }
  
  if (currentPage == 0) {
    if ((millis() - lastTempTime) >= 1000) {
      // TODO Clear only on page switch
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Temp 1");
      lcd.setCursor(11, 0);
      lcd.print(lm35.getTemp());
      lastTempTime = millis();
    }
  } else {
    if ((millis() - lastTempTime) >= 1000) {
      // TODO Clear only on page switch
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Hello World!");
      lastTempTime = millis();
    }
  }
}
