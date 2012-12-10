#include "Arduino.h"
#include "Button.h"

Button::Button(const int pin) {
  pinMode(_pin, INPUT);
  _pin = pin;
  lastButtonState = LOW;
}

boolean Button::released() {
  const int buttonState = digitalRead(_pin);

  const boolean released = (buttonState == LOW) && (lastButtonState == HIGH);
  lastButtonState = buttonState;
  delay(50); // TODO Debounce without delay

  return released;
}
