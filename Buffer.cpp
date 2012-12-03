#include "Arduino.h"
#include "Buffer.h"

void Buffer::addValue(float value) {
  buffer[bufferPos] = value;
  bufferPos = (bufferPos + 1) % BUFFER_SIZE;
}
  
float Buffer::getValue() {
  // TODO Handle stroed values < BFFER_SIZE 
  int sum = 0;
  for (int i = 0; i < BUFFER_SIZE; i++) {
    sum += buffer[i];
  }
  return sum / (float)BUFFER_SIZE;
}
