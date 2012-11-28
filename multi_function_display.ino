#include <LiquidCrystal.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include "Buffer.h"
#include "Button.h"

const int tempPin = 6;
const int bgPin = A1;
const int buttonPin = 7;

long lastMeasureTime = 0;
long lastBgTime = 0;

DeviceAddress thermometer = { 
  0x10, 0x5E, 0x98, 0x74, 0x02, 0x08, 0x00, 0xD0 };

class Broker {
public:
  Broker() : 
  pos(0) {
  }

  void subscribe(void (*f)(float)) {
    _f[pos] = f;
    pos++;
  }

  void notify(float value) {
    for (int i = 0; i < pos; i++) {
      (_f[i])(value);
    }
  }

private:
  void (*_f[10])(float);
  int pos;  
};

class Measurements {

public:
  Measurements(Broker* b) : 
  _ds(tempPin), _sensors(&_ds), _temp() {
    _b = b;
    _sensors.begin();
  }

  void update() {
    _sensors.requestTemperatures();
    _temp.addValue(_sensors.getTempC(thermometer));
    _b->notify(getTemp());
  }

  float getTemp() {
    return _temp.getValue();
  }

private:
  Buffer _temp;
  OneWire _ds;
  DallasTemperature _sensors;
  Broker* _b;
};

class Display {
  // TODO Make those private
  static const int PAGES = 2;
  LiquidCrystal _lcd;
  Measurements* _m;

  int _currentPage;

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

  char* formatFloat(float input) {
    char c[6];
    dtostrf(round(input * 10) / float(10), 4, 1, c);
    return c;
  }
};

Button button(buttonPin);
Broker b;
Measurements m(&b);
Display display(&m);

void setup() {
  delay(2000);
  pinMode(bgPin, INPUT);
  display.render();
  Serial.begin(9600);
  setBackground();

  b.subscribe(&updateDisplay);
}

void updateDisplay(float value) {
  // Update display when the values changed
  // TODO Use value from broker
  display.render();
}

void loop() {
  if ((millis() - lastBgTime) >= 3000) {
    setBackground();
  }

  if (button.released()) {
    display.switchPage();
  }

  // Update measurements every 15 seconds
  if ((millis() - lastMeasureTime) >= 15000) {
    m.update();
    lastMeasureTime = millis();
  }

  if (m.getTemp() > 25.0) {
    display.switchToPage(0);
  }
}

void setBackground() {
  const int ldr = map(analogRead(bgPin), 0, 511, 0, 191) + 64;
  analogWrite(9, ldr);
  lastBgTime = millis();
}

