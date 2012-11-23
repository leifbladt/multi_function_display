#include <LiquidCrystal.h>

const int tempPin = A0;
const int buttonPin = 7;

long lastTempTime = 0;

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
  int buttonState;             // the current reading from the input pin
  int lastButtonState;   // the previous reading from the input pin

  // the following variables are long's because the time, measured in miliseconds,
  // will quickly become a bigger number than can be stored in an int.
  long lastDebounceTime;  // the last time the output pin was toggled
  static const long debounceDelay = 50;    // the debounce time; increase if the output flickers

public:
  Button(const int pin) {
    _pin = pin;
    lastDebounceTime = 0;
    lastButtonState = LOW;
  }

  boolean pressed() {
    // read the state of the switch into a local variable:
    int reading = digitalRead(buttonPin);

    // check to see if you just pressed the button 
    // (i.e. the input went from LOW to HIGH),  and you've waited 
    // long enough since the last press to ignore any noise:  

    // If the switch changed, due to noise or pressing:
    if (reading != lastButtonState) {
      // reset the debouncing timer
      lastDebounceTime = millis();
    }    

    if ((millis() - lastDebounceTime) > debounceDelay) {
      // whatever the reading is at, it's been there for longer
      // than the debounce delay, so take it as the actual current state:
      buttonState = reading;
    }
    lastButtonState = reading;
    
    return buttonState  == HIGH;
  }
};

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
LM35 lm35(tempPin);
Button button(buttonPin);

void setup() {
  pinMode(buttonPin, INPUT);
  lcd.begin(16, 2);
  lcd.print("Temp 1");
  delay(500);
  Serial.begin(9600);
}

void loop() {
  if ((millis() - lastTempTime) >= 1000) {
    lcd.setCursor(11, 0);
    lcd.print(lm35.getTemp());
    Serial.println("update temp");
    lastTempTime = millis();
  }

  if (button.pressed()) {
    lcd.setCursor(0, 1);
    lcd.print("Button pressed");
  } 
  else {
    lcd.setCursor(0, 1);
    lcd.print("                ");
  }
}
