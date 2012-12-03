#define Buffer_h
#include "Arduino.h"


template <class T>
class Buffer {
public:
  void addValue(T value);
  
  T getValue();

private:
  static const int BUFFER_SIZE = 3;
  T buffer[BUFFER_SIZE];
  int bufferPos;
};

template <class T>
void Buffer<T>::addValue(T value) {
  buffer[bufferPos] = value;
  bufferPos = (bufferPos + 1) % BUFFER_SIZE;
}
  
template <class T>
T Buffer<T>::getValue() {
  // TODO Handle stored values < BUFFER_SIZE 
  int sum = 0;
  for (int i = 0; i < BUFFER_SIZE; i++) {
    sum += buffer[i];
  }
  return sum / (T)BUFFER_SIZE;
}
