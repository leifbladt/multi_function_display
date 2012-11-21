const int tempPin = A0;

void setup() {
  delay(500);
  Serial.begin(9600);
}

void loop() {
  Serial.println(getTemp());
  delay(1000);
}

float getTemp() {
  const int temp = analogRead(tempPin);
  return temp * 0.488;
}




