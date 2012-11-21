const int tempPin = A0;

const int BUFFER_SIZE = 3;
int buffer[BUFFER_SIZE];
int bufferPos = 0;

void setup() {
  delay(500);
  Serial.begin(9600);
}

void loop() {
  Serial.println(formatOutput(getTemp()));
  delay(1000);
}

float getTemp() {
  buffer[bufferPos] = analogRead(tempPin);
  bufferPos = (bufferPos + 1) % BUFFER_SIZE;

  int sum = 0;
  for (int i = 0; i < BUFFER_SIZE; i++) {
    sum += buffer[i];
  }

  return sum / BUFFER_SIZE * 0.488;
}

float formatOutput(const float input) {
  return round(input * 10) / float(10);
}
