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

class Display {
  // TODO Make those private
  static const int PAGES = 2;
  LiquidCrystal _lcd;
  LM35 _lm35;
  int _currentPage;
  OneWire _ds;
  DallasTemperature sensors;

public:
  // TODO Default constructor?
  Display(int tmp) : 
  _lcd(12, 11, 5, 4, 3, 2), _lm35(tempPin), _ds(temp2Pin), sensors(&_ds)
  {
    _lcd.begin(16, 2);
    _currentPage = 0;
    sensors.begin();
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
      _lcd.print(_lm35.getTemp());
    } 
    else {
      _lcd.setCursor(0, 0);
      _lcd.print("Temp 2");
      _lcd.setCursor(11, 0);
      _lcd.print(temp2());
    }
  }

  float temp2() {
    sensors.requestTemperatures();
    return sensors.getTempC(thermometer);
  }
};


Button button(buttonPin);
Display display(0);

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

  // Update screens every 3 seconds
  if ((millis() - lastMeasureTime) >= 3000) {
    display.render();
    lastMeasureTime = millis();
  }
}

void setBackground() {
  const int ldr = map(analogRead(bgPin), 0, 511, 0, 191) + 64;
  analogWrite(9, ldr);
  lastBgTime = millis();
}

