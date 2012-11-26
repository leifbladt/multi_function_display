#include <LiquidCrystal.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include "LM35.h"
#include "Button.h"

const int tempPin = A0;
const int temp2Pin = 6;
const int bgPin = A1;
const int buttonPin = 7;

long lastMeasureTime = 0;
long lastBgTime = 0;

DeviceAddress thermometer = { 
  0x10, 0x5E, 0x98, 0x74, 0x02, 0x08, 0x00, 0xD0 };

class Buffer {
public:
  float addValue(float value) {
    buffer[bufferPos] = value;
    bufferPos = (bufferPos + 1) % BUFFER_SIZE;
  }
  
  float getValue() {
    // TODO Handle stroed values < BFFER_SIZE 
    int sum = 0;
    for (int i = 0; i < BUFFER_SIZE; i++) {
      sum += buffer[i];
    }
    return formatOutput(sum / (float)BUFFER_SIZE);
  }

private:
  static const int BUFFER_SIZE = 3;
  float buffer[BUFFER_SIZE];
  int bufferPos;

  float formatOutput(const float input) {
    return round(input * 10) / float(10);
  }  
};

class Measurements {
  LM35 _lm35;
  OneWire _ds;
  DallasTemperature _sensors;

public:
  Measurements(int tmp) : 
  _lm35(tempPin), _ds(temp2Pin), _sensors(&_ds), _temp2() {
    _sensors.begin();
  }

  void update() {
    _temp1 = _lm35.getTemp();
    _sensors.requestTemperatures();
    _temp2.addValue(_sensors.getTempC(thermometer));
  }

  float getTemp1() {
    return _temp1;
  }

  float getTemp2() {
    return _temp2.getValue();
  }

private:
  float _temp1;
  Buffer _temp2;  
};

class Display {
  // TODO Make those private
  static const int PAGES = 3;
  LiquidCrystal _lcd;
  Measurements& _m;

  int _currentPage;

public:
  Display(Measurements& m) :
  _lcd(12, 11, 5, 4, 3, 2), _m(m)
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

  void clear() {
    _lcd.clear();
    _lcd.setCursor(0, 0);
  }

  void render() {
    if (_currentPage == 0) {
      _lcd.setCursor(0, 0);
      _lcd.print("Temp 1");
      _lcd.setCursor(11, 0);
      _lcd.print(_m.getTemp1());
    } 
    else if (_currentPage == 1) {
      _lcd.setCursor(0, 0);
      _lcd.print("Temp 1: ");
      _lcd.setCursor(11, 0);
      _lcd.print(_m.getTemp1());
      _lcd.setCursor(0, 1);
      _lcd.print("Temp 2:");
      _lcd.setCursor(11, 1);
      _lcd.print(_m.getTemp2());
    } 
    else {
      _lcd.setCursor(0, 0);
      _lcd.print("Temp 2:");
      _lcd.setCursor(11, 0);
      _lcd.print(_m.getTemp2());
    }
  }
};


Button button(buttonPin);
Measurements m(0);
Display display(m);

void setup() {
  pinMode(bgPin, INPUT);
  display.render();
  Serial.begin(9600);
  setBackground();
}

void loop() {
  if ((millis() - lastBgTime) >= 3000) {
    setBackground();
  }

  if (button.released()) {
    display.switchPage();
  }

  // Update screens every 15 seconds
  if ((millis() - lastMeasureTime) >= 15000) {
    m.update();
    display.render();
    lastMeasureTime = millis();
  }
}

void setBackground() {
  const int ldr = map(analogRead(bgPin), 0, 511, 0, 191) + 64;
  analogWrite(9, ldr);
  lastBgTime = millis();
}

