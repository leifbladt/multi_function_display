#include <OneWire.h>
#include <DallasTemperature.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "Buffer.h"
#include "Button.h"

//#define DEBUG_MODE

#define R1 3000.0
#define R2 1000.0

#define TEMP1_PIN 6
#define VOLTAGE1_PIN A0
#define BUTTON_PIN 7
#define OLED_DC 11
#define OLED_CS 12
#define OLED_CLK 10
#define OLED_MOSI 9
#define OLED_RESET 13

const float voltageFactor = 5.0 * (R1 + R2) / R2 / 1023.0;

long lastMeasureTime = 0;

DeviceAddress temp1Addr = { 
  0x10, 0x5E, 0x98, 0x74, 0x02, 0x08, 0x00, 0xD0 };

DeviceAddress temp2Addr = {   
  0x28, 0x29, 0x5F, 0x37, 0x04, 0x00, 0x00, 0x94 };

class Measurements {
public:
  Measurements() : 
  _temp1(), _temp2(), _voltage(), _ds(TEMP1_PIN), _sensors(&_ds) {
    _sensors.begin();
  }

  void update() {
    updateTemp();
    updateVoltage();
  }

  float getTemp1() {
    return _temp1.getValue();
  }
  
  float getTemp2() {
    return _temp2.getValue();
  }  

  float getVoltage() {
    int reading = _voltage.getValue();
    return reading * voltageFactor;
  }
  
  #ifdef DEBUG_MODE
  void discoverOneWireDevices() {
    // http://www.hacktronics.com/Tutorials/arduino-1-wire-address-finder.html
    byte i;
    byte present = 0;
    byte data[12];
    byte addr[8];
    
    while(_ds.search(addr)) {
      Serial.print("Found \'1-Wire\' device with address:\n\r");
      for( i = 0; i < 8; i++) {
        Serial.print("0x");
        if (addr[i] < 16) {
          Serial.print('0');
        }
        Serial.print(addr[i], HEX);
        if (i < 7) {
          Serial.print(", ");
        }
      }
      Serial.println();
      if (OneWire::crc8(addr, 7) != addr[7]) {
        Serial.print("CRC is not valid!\n");
        return;
      }
    }
    _ds.reset_search();
  }
 #endif 

private:
  Buffer <float> _temp1;
  Buffer <float> _temp2;
  Buffer <int> _voltage;
  OneWire _ds;
  DallasTemperature _sensors;

  void updateTemp() {
    _sensors.requestTemperatures();
    _temp1.addValue(_sensors.getTempC(temp1Addr));
    _temp2.addValue(_sensors.getTempC(temp2Addr));
  }

  void updateVoltage() {
    _voltage.addValue(analogRead(VOLTAGE1_PIN));
  }
};

class Display {
public:
  Display(Measurements* m, Adafruit_SSD1306* oled) 
  {
    _m = m;
    _oled = oled;
    _currentPage = 0;
  }

  void switchPage() {
    _currentPage = (_currentPage + 1) % PAGES;
    render();
  }

  void switchToPage(int page) {
    _currentPage = page;
    render();
  }

  void clear() {
    _oled->clearDisplay();
    _oled->setCursor(0, 0);
  }

  void render() {
    clear();
    if (_currentPage == 0) {
      char label[] = "T1";
      char c[8];
      formatTemperature(_m->getTemp1(), c);
      show(label, c);
    }
    else if (_currentPage == 1) {
      char label[] = "T2";
      char c[8];
      formatTemperature(_m->getTemp2(), c);
      show(label, c);
    } 
    else if (_currentPage == 2) {
      char label[] = "U1";
      char c[8];
      formatVoltage(_m->getVoltage(), c);
      show(label, c);
    }
    #ifdef DEBUG_MODE
    else if (_currentPage == 3) {
      char label[] = "Mem";
      char c[5];
      show(label, itoa(freeRam(), c, 10));
    }
    #endif
    
    // Show bottom bar
    _oled->setTextColor(WHITE);
    _oled->setTextSize(2);
    _oled->drawLine(0, 46, 127, 46, WHITE);
    _oled->setCursor(36, 50);
    _oled->println("12:21");
    
    _oled->display();
  }

private:
  #ifdef DEBUG_MODE
  static const int PAGES = 4;
  #else
  static const int PAGES = 3;
  #endif

  Adafruit_SSD1306* _oled;
  Measurements* _m;
  int _currentPage;

  void formatTemperature(const float input, char* s) {
    float t1 = round(input * 10) / 10.0;
    dtostrf(round(t1 * 2) / 2.0, 5, 1, s);
    s[5] = char(247);
    s[6] = 'C';
    s[7] = '\0';
  }

  void formatVoltage(const float input, char* s) {
    float t1 = round(input * 10) / 10.0;
    dtostrf(t1, 5, 1, s);
    s[5] = ' ';
    s[6] = 'V';
    s[7] = '\0';
  }

  void show(char* label, char* value) {
    _oled->setTextColor(WHITE);
    _oled->setTextSize(2);
    _oled->setCursor(0, 15);
    _oled->println(label);
    _oled->setCursor(30, 15);    
    _oled->print(value);
    
    #ifdef DEBUG_MODE
    Serial.print(label);
    Serial.print(" ");
    Serial.println(value);
    #endif
  }

  #ifdef DEBUG_MODE
  int freeRam () {
    // http://playground.arduino.cc/Code/AvailableMemory
    extern int __heap_start, *__brkval; 
    int v; 
    return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval); 
  }
  #endif
};

Button button(BUTTON_PIN);
Measurements m;
Adafruit_SSD1306 oled(OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);
Display display(&m, &oled);

void setup() {
  delay(2000);
  oled.begin(SSD1306_SWITCHCAPVCC);
  oled.clearDisplay();
  oled.display();

  pinMode(VOLTAGE1_PIN, INPUT);
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
    
    #ifdef DEBUG_MODE
    m.discoverOneWireDevices();
    #endif
  }

  if (m.getTemp() > 25.0) {
    display.switchToPage(0);
  }
}
