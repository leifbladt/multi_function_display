#ifndef LM35_h
#define LM35_h
#include "Arduino.h"

class LM35 {
public:
  LM35(const int pin);

  float getTemp();

private:
  static const int BUFFER_SIZE = 3;
  int buffer[BUFFER_SIZE];
  int bufferPos;
  int _pin;

  float formatOutput(const float input);
};
#endif
