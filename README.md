# Multifunktionsanzeige

## TODO Disclaimer

## Temperatur

 * Genauigkeit des Sensors


## Bordspannung

Es sollen beide Stromkreise überwacht werden. 

### Spannungsteiler

Um Spannungen über 5 V messen zu können, wird ein Spannungsteiler benötigt. 
R1 = 3 kOhm, R2 = 1 kOhm

 * "Stromverbrauch", typischerweise 3 mA
 * Genauigkeit, Auflösung
 * 0..20 V eigentlich nicht nötig


### Schutzschaltung

 * Einfluss der Zener-Diode (kleiner Strom)
 * > 16 V (ungültig)


## Links

### Bibliotheken

 * http://www.pjrc.com/teensy/td_libs_OneWire.html
 * http://www.milesburton.com/?title=Dallas_Temperature_Control_Library

### Datenblätter

 * http://datasheets.maximintegrated.com/en/ds/DS18B20.pdf
 * http://www.fairchildsemi.com/ds/1N/1N4733A.pdf 

## Teileliste

### Allgemein

* Arduino Leonardo
* Taster
* Widerstand 10 kOhm

### Temperatur

* Temperatursensor DS18B20 (3x)
* Widerstand 4.7 kOhm

### Bordspannung

* Widerstand 3 kOhm (2x)
* Widerstand 1 kOhm (2x)
* Z-Diode 1N4733A (2x)
