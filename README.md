﻿# Arduino CO2 Alert

Warns you when CO2 levels are too high with an alert LED.

## Notice!
The sensors used for this project need some time to "warm up". They need to run for about 48 hours
once before you can expect useful results. After that they need some time to
adjust. (About 1-2 hours).
The sensors don't calculate the CO2 directly, but by interpolating it from other values.
The gathered values just an indicator it's time to open the window, not an exact measurement.

##  Parts required
  - 1 Arduino UNO (others may work too)
  - 1 CCS811 sensor (needs to get soldered to
  - 1 BME280 sensor
  - 1 LED, red

## Libraries required 
Install via Sketch -> Include library --> Manage libraries

  - Adafruit_BME280
  - Adafruit_CCS811

## Wiring 
Left sensor, right Arduino
  - LED PLUS (the longer wire) -> 13, LED MINUS ->GND
  - BME280:
    -VIN -> POWER 3.3 (together with CCS811 VCC)
    - GDN -> POWER GND (together with CCS811 GND)
    - SCL -> A5 (together with CCS811 SCL)
    - SDA -> A4 (together with CCS811 SCA)
  - CCS811:
    - VCC -> POWER 3.3 (together with BME280 VIN)
    - GDN -> POWER GND (together with BME280 GND)
    - SCL -> A5 (together with BME280 SCL)
    - SDA -> A4 (together with BME280 SCA)
    - WAK -> POWER GND (together with BME280 GND)

  If everything is correct the LED should light up if the CO2 value is over 1000ppm. That means: it's time to open your window.
  This program also shows all kinds of interesting values (temperature, air pressure, humidity and volatile organic compounds) when using the Arduino Serial Monitor (Tools -> Serial Monitor).
