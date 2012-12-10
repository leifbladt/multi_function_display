#include <LiquidCrystal.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include "Buffer.h"
#include "Button.h"

const int tempPin = 6;
const int voltPin = A0;
const int buttonPin = 7;

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
    // TODO Calculate scaling factor only once
    return reading * 5.0 * (2400000.0 + 1000000.0) / 1000000.0 / 1023;
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
      char c[6];
      formatTemperature(_m->getTemp(), c);
      show(label, c);
    } 
    else {
      char label[] = "Spannung:";
      char c[6];
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
    dtostrf(round(t1 * 2) / 2.0, 4, 1, s);
    s[4] = char(223);
    s[5] = 'C';
  }

  void formatVoltage(const float input, char* s) {
    float t1 = round(input * 10) / 10.0;
    dtostrf(t1, 4, 1, s);
    s[4] = ' ';
    s[5] = 'V';
  }
  
  void show(char* label, char* value) {
    _lcd.setCursor(0, 0);
    _lcd.print(label);
    _lcd.setCursor(10, 0);    
    _lcd.print(value);    
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
  //  Serial.begin(9600);
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

