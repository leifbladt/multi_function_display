#include <LiquidCrystal.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include "Buffer.h"
#include "Button.h"

const int tempPin = 6;
const int bgPin = A1;
const int buttonPin = 7;
const int pwmPin = 9;

long lastMeasureTime = 0;

DeviceAddress thermometer = { 
  0x10, 0x5E, 0x98, 0x74, 0x02, 0x08, 0x00, 0xD0 };

class Measurements {
public:
  Measurements() : 
  _temp(), _brightness(), _ds(tempPin), _sensors(&_ds) {
    _sensors.begin();
  }

  void update() {
    updateTemp();
    updateBrightness();
  }

  float getTemp() {
    return _temp.getValue();
  }

  int getBrightness() {
    Serial.print("read: ");
    Serial.println(_brightness.getValue());
    return _brightness.getValue();
  }

private:
  Buffer <float> _temp;
  Buffer <int> _brightness;
  OneWire _ds;
  DallasTemperature _sensors;

  void updateTemp() {
    _sensors.requestTemperatures();
    _temp.addValue(_sensors.getTempC(thermometer));
  }

  void updateBrightness() {
    _brightness.addValue(analogRead(bgPin));
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
    setBackground();

    if (_currentPage == 0) {
      _lcd.setCursor(0, 0);
      _lcd.print("Temp:");
      _lcd.setCursor(12, 0);
      _lcd.print(formatFloat(_m->getTemp()));
    } 
    else {
      _lcd.setCursor(0, 0);
      _lcd.print("Hello World!");
    }
  }

private:
  static const int PAGES = 2;
  LiquidCrystal _lcd;
  Measurements* _m;

  int _currentPage;
  
  char* formatFloat(float input) {
    char c[6];
    dtostrf(round(input * 10) / float(10), 4, 1, c);
    return c;
  }
  
  void setBackground() {
    const int ldr = map(_m->getBrightness(), 0, 511, 0, 191) + 64;
    analogWrite(pwmPin, ldr);
  }
};

Button button(buttonPin);
Measurements m;
Display display(&m);

void setup() {
  delay(2000);
  pinMode(bgPin, INPUT);
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
