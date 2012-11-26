#include "Arduino.h"
#include "Buffer.h"

float Buffer::addValue(float value) {
  buffer[bufferPos] = value;
  bufferPos = (bufferPos + 1) % BUFFER_SIZE;
}
  
float Buffer::getValue() {
  // TODO Handle stroed values < BFFER_SIZE 
  int sum = 0;
  for (int i = 0; i < BUFFER_SIZE; i++) {
    sum += buffer[i];
  }
  return formatOutput(sum / (float)BUFFER_SIZE);
}

float Buffer::formatOutput(const float input) {
  return round(input * 10) / float(10);
}
