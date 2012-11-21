const int tempPin = A0;

void setup() {
  delay(500);
  Serial.begin(9600);
}

void loop() {
  Serial.println(formatOutput(getTemp()));
  delay(1000);
}

float getTemp() {
  const int temp = analogRead(tempPin);
  return temp * 0.488;
}

float formatOutput(const float input) {
  return round(input * 10) / float(10);
}



