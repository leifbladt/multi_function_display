#include "Arduino.h"
#include "LM35.h"

LM35::LM35(const int pin) {
  _pin = pin;
  bufferPos = 0;
}

float LM35::getTemp() {
  buffer[bufferPos] = analogRead(_pin);
  bufferPos = (bufferPos + 1) % BUFFER_SIZE;

  int sum = 0;
  for (int i = 0; i < BUFFER_SIZE; i++) {
    sum += buffer[i];
  }

  return formatOutput(sum / BUFFER_SIZE * 0.488);
}

float LM35::formatOutput(const float input) {
  return round(input * 10) / float(10);
}

