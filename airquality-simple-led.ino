/*
  Arduino Office Climate CO2 Alert
  Warns you when CO2 levels are too high with an alert LED.

  Notice!
  The sensors used for this project need some time to "warm up". They need to run for about 48 hours
  once before you can expect useful results. After that they need some time to
  adjust. (About 1-2 hours).
  The sensors don't calculate the CO2 directly, but by interpolating it from other values.
  The gathered values just an indicator it's time to open the window, not an exact measurement.

  Parts required
  - 1 Arduino UNO (others may work too)
  - 1 CCS811 sensor (needs to get soldered to
  - 1 BME280 sensor
  - 1 LED, red

  Libraries required -- install via Sketch -> Include library --> Manage libraries
  - Adafruit_BME280
  - Adafruit_CCS811

  Wiring -- Left device, right Arduino
  - LED PLUS (the longer wire) -> 13, LED MINUS ->GND
  - BME280:
  -- VIN -> POWER 3.3 (together with CCS811 VCC)
  -- GDN -> POWER GND (together with CCS811 GND)
  -- SCL -> A5 (together with CCS811 SCL)
  -- SDA -> A4 (together with CCS811 SCA)
  - CCS811:
  -- VCC -> POWER 3.3 (together with BME280 VIN)
  -- GDN -> POWER GND (together with BME280 GND)
  -- SCL -> A5 (together with BME280 SCL)
  -- SDA -> A4 (together with BME280 SCA)
  -- WAK -> POWER GND (together with BME280 GND)

  If everything is correct the LED should light up if the CO2 value is over 1000ppm. That means: it's time to open your window.
  This program also shows all kinds of interesting values (temperature, air pressure, humidity and volatile organic compounds)
  when using the Arduino Serial Monitor (Tools -> Serial Monitor).

*/

/**
  LIBRARIES
*/

#include <Adafruit_BME280.h> // BME = the small one
#include <Adafruit_CCS811.h> // CCS = the big one

/**
  VARS AND DEFAULTS
*/
#define LED 13 // LED PIN

// BME280
#define BME_I2C_ADDR 0x76
Adafruit_BME280 bme;
float bmeTemp = 0;
float bmeHum = 0;
float bmePres = 0;
long lastBMESensorRead = 0;


// CCS811
Adafruit_CCS811 ccs;
uint16_t ccsCO2 = 0;
uint16_t ccsTVOC = 0;
long lastCCSSensorRead = 0;

// Measure every.. (ms). Needs to be a value over 30000 (30s). The sensors get imprecise otherwise
unsigned int delayTime = 30000;

/**
  SETUP
*/
void setup() {
  Serial.begin(9600);
  Serial.println("Setup...");

  initBME();
  initCCS();

  // LED pin
  pinMode(13, OUTPUT);

  Serial.println("Setup complete!");
  Serial.println();
}

void initBME() {
  // Configure sensors (recommended settings for weather monitoring)
  if (!bme.begin(BME_I2C_ADDR)) {
    Serial.println("Error starting BME280 sensor");
    while (1);
  } else {
    bme.setSampling(
      Adafruit_BME280::MODE_FORCED,
      Adafruit_BME280::SAMPLING_X1, // temperature
      Adafruit_BME280::SAMPLING_X1, // pressure
      Adafruit_BME280::SAMPLING_X1, // humidity
      Adafruit_BME280::FILTER_OFF
    );
    Serial.println("BME280 initialized.");
  }
}

void initCCS() {
  if (!ccs.begin()) {
    Serial.println("Error starting CCS811 sensor  ");
    while (1);
  } else {
    //calibrate temperature sensor
    while (!ccs.available());
    Serial.println("CCS811 initialized.");
  }
}

/**
  LOOP
*/
void loop() {
  long now = millis();

  // BME280 read interval = 30sek (recommended 60sek for weather monitoring)
  if (now - lastBMESensorRead > delayTime || lastBMESensorRead == 0) {
    lastBMESensorRead = now;

    bme.takeForcedMeasurement();
    bmeTemp = bme.readTemperature();
    bmePres = bme.readPressure() / 100.0F;
    bmeHum = bme.readHumidity();
  }

  if (now - lastCCSSensorRead > delayTime || lastCCSSensorRead == 0) {
    lastCCSSensorRead = now;
    ccs.setEnvironmentalData((int)bme.readHumidity(), bme.readTemperature());
    if (ccs.available()) {
      if (!ccs.readData()) {
        ccsCO2 = ccs.geteCO2();
        ccsTVOC = ccs.getTVOC();
        float temp = ccs.calculateTemperature();
      } else {
        Serial.print("CCS811 Read error");
      }
    } else {
      Serial.print("Not ready");
    }
  }

  // Turn on the LED if CO2 is higher than 1000ppm
  if (ccsCO2 > 1000) {
    Serial.println("CO2 High!");
    digitalWrite(LED, HIGH);
  } else {
    digitalWrite(LED, LOW);
  }

  String output = generateOutput();
  Serial.println(output);
  delay(delayTime);
}


/**
  OUTPUT
*/
String generateOutput() {
  String output = "";
  output = output + "Temperature: " + bmeTemp + "°C | ";
  output = output + "Pressure: " + (bmePres / 100.0F) + "hPa | ";
  output = output + "Humidity: " + bmeHum + "% | ";
  output = output + "CO2: " + ccsCO2 + "ppm | ";
  output = output + "VOC: " + ccsTVOC + "tvoc | ";
  return output;
}
