#ifndef Button_h
#define Button_h
#include "Arduino.h"

class Button {
public:
  Button(const int pin);

  boolean released();

private:
  int _pin;
  int lastButtonState;   // the previous reading from the input pin
};
#endif
