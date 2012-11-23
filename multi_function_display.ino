const int tempPin = A0;

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

LM35 lm35(tempPin);

void setup() {
  delay(500);
  Serial.begin(9600);
}

void loop() {
  Serial.println(lm35.getTemp());
  delay(1000);
}
