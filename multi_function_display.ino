#include <LiquidCrystal.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include "Buffer.h"
#include "Button.h"

//#define DEBUG_MODE

#define R1 3000.0
#define R2 1000.0

const int tempPin = 6;
const int voltPin = A0;
const int buttonPin = 7;

const float voltageFactor = 5.0 * (R1 + R2) / R2 / 1023.0;

long lastMeasureTime = 0;

DeviceAddress thermometer = { 
  0x10, 0x5E, 0x98, 0x74, 0x02, 0x08, 0x00, 0xD0 };

class Measurements {
public:
  Measurements() : 
  _temp(), _voltage(), _ds(tempPin), _sensors(&_ds) {
    _sensors.begin();
  }

  void update() {
    updateTemp();
    updateVoltage();
  }

  float getTemp() {
    return _temp.getValue();
  }

  float getVoltage() {
    int reading = _voltage.getValue();
    return reading * voltageFactor;
  }

private:
  Buffer <float> _temp;
  Buffer <int> _voltage;
  OneWire _ds;
  DallasTemperature _sensors;

  void updateTemp() {
    _sensors.requestTemperatures();
    _temp.addValue(_sensors.getTempC(thermometer));
  }

  void updateVoltage() {
    _voltage.addValue(analogRead(voltPin));
  }
};

class Display {
public:
  Display(Measurements* m) :
  _lcd(12, 11, 5, 4, 3, 2)
  {
    _m = m;
    _lcd.begin(16, 2);
    _currentPage = 0;
  }

  void switchPage() {
    _currentPage = (_currentPage + 1) % PAGES;
    clear();
    render();
  }

  void switchToPage(int page) {
    _currentPage = page;
    clear();
    render();
  }

  void clear() {
    _lcd.clear();
    _lcd.setCursor(0, 0);
  }

  void render() {
    if (_currentPage == 0) {
      char label[] = "Temp:";
      char c[8];
      formatTemperature(_m->getTemp(), c);
      show(label, c);
    } 
    else {
      char label[] = "Spannung:";
      char c[8];
      formatVoltage(_m->getVoltage(), c);
      show(label, c);
    }
  }

private:
  static const int PAGES = 2;
  LiquidCrystal _lcd;
  Measurements* _m;

  int _currentPage;

  void formatTemperature(const float input, char* s) {
    float t1 = round(input * 10) / 10.0;
    dtostrf(round(t1 * 2) / 2.0, 5, 1, s);
    s[5] = char(223);
    s[6] = 'C';
  }

  void formatVoltage(const float input, char* s) {
    float t1 = round(input * 10) / 10.0;
    dtostrf(t1, 5, 1, s);
    s[5] = ' ';
    s[6] = 'V';
  }

  void show(char* label, char* value) {
    _lcd.setCursor(0, 0);
    _lcd.print(label);
    _lcd.setCursor(9, 0);    
    _lcd.print(value);
    
    #ifdef DEBUG_MODE
    Serial.print(label);
    Serial.print(" ");
    Serial.println(value);
    #endif
  }
};

Button button(buttonPin);
Measurements m;
Display display(&m);

void setup() {
  delay(2000);
  pinMode(voltPin, INPUT);
  m.update();
  display.render();
  #ifdef DEBUG_MODE
  Serial.begin(9600);
  #endif
}

void loop() {
  if (button.released()) {
    display.switchPage();
  }

  // Update measurements every 15 seconds
  if ((millis() - lastMeasureTime) >= 15000) {
    m.update();
    display.render();
    lastMeasureTime = millis();
  }

  if (m.getTemp() > 25.0) {
    display.switchToPage(0);
  }
}
