#ifndef Buffer_h
#define Buffer_h
#include "Arduino.h"
class Buffer {
public:
  float addValue(float value);
  
  float getValue();

private:
  static const int BUFFER_SIZE = 3;
  float buffer[BUFFER_SIZE];
  int bufferPos;
};
#endif
