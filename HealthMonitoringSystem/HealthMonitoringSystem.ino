// ~~~~~~ Constant Variables ~~~~~~
#define BLYNK_TEMPLATE_ID "~~template id~~"
#define BLYNK_DEVICE_NAME "~~device name~~"
#define BLYNK_AUTH_TOKEN "~~auth token~~"
#define BLYNK_PRINT Serial
#define TEMPERATURE_INPUT_PIN 14       // GPIO14 => D5
#define SHOULD_READ_TEMPERATURE_PIN 13 // GPIO13 => D7
#define TEMPERATURE_VIRTUAL_PIN V5
#define SHOULD_READ_PULSEO2 12 // GPIO 12 => D6
#define BPM_VIRTUAL_PIN V0
#define SPO2_VIRTUAL_PIN V1

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Wire.h>
#include "MAX30100_PulseOximeter.h"
#include <SimpleTimer.h>

#define REPORTING_PERIOD_MS 1000
SimpleTimer simpleTimer;
PulseOximeter pox;
uint32_t tsLastReport = 0;

// ~~~~~~ System Initializations ~~~~~~
OneWire oneWire(TEMPERATURE_INPUT_PIN);
DallasTemperature sensors(&oneWire);

// ~~~~~~ Network Credentials ~~~~~~
char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "OnePlus";
char pass[] = "HelloWorld";
BlynkTimer timer;

// ~~~~~~ Global Variables ~~~~~~
int shouldReadTemperature;
int shouldReadPulseO2;

void uploadTempReadings()
{
  if (shouldReadTemperature == 1)
  {
    sensors.requestTemperatures();
    float temperatureF = sensors.getTempFByIndex(0);
    Serial.print(temperatureF);
    Serial.println("ºF");
    Blynk.virtualWrite(TEMPERATURE_VIRTUAL_PIN, temperatureF);
  }
}

void uploadSpO2Readings()
{
  if (shouldReadPulseO2 == 1)
  {
    float bpm = pox.getHeartRate();
    int spo2 = pox.getSpO2();
    Serial.print(bpm);
    Serial.print("  SpO2: ");
    Serial.print(spo2);
    Serial.println("%");
    Blynk.virtualWrite(BPM_VIRTUAL_PIN, bpm);
    Blynk.virtualWrite(SPO2_VIRTUAL_PIN, spo2);
  }
}

void setup()
{
  Serial.begin(115200);

  // ~~~~~~~~~~~~~~ INTIALIZING TEMPERATURE SENSOR ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

  Serial.println("Starting temperature Setup");
  sensors.begin(); // Start the DS18B20 Temperature sensor
  Blynk.begin(auth, ssid, pass);
  timer.setInterval(1000L, uploadTempReadings);
  Serial.println("Temperature Setup Complete");

  // ~~~~~~~~~~~~~~ INTIALIZING PULSE OXYMETER ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  Serial.print("Initializing pulse oximeter..");
  if (!pox.begin())
  {
    Serial.println("FAILED");
    for (;;)
      ;
  }
  else
  {
    Serial.println("SUCCESS");
    digitalWrite(1, HIGH); // 1 = TX PIN
  }

  pox.setIRLedCurrent(MAX30100_LED_CURR_24MA);
  // Register a callback for the beat detection
  pox.setOnBeatDetectedCallback(onBeatDetected);
  timer.setInterval(2000L, uploadSpO2Readings);
  // simpleTimer.setInterval(1000L, getSendData);
}

void loop()
{
  Blynk.run();
  timer.run();
  shouldReadTemperature = digitalRead(SHOULD_READ_TEMPERATURE_PIN);
  shouldReadPulseO2 = digitalRead(SHOULD_READ_PULSEO2);
  // Serial.println(shouldReadTemperature);

  // simpleTimer.run(); // Initiates SimpleTimer
  pox.update(); // Make sure to call update as fast as possible

  // if (shouldReadTemperature == 1)
  // {
  //   sensors.requestTemperatures();
  //   float temperatureF = sensors.getTempFByIndex(0);
  //   Serial.print(temperatureF);
  //   Serial.println("ºF");
  // }

  // if ((shouldReadPulseO2 == 1) && (millis() - tsLastReport > REPORTING_PERIOD_MS))
  // {
  //   float bpm = pox.getHeartRate();
  //   int spo2 = pox.getSpO2();
  //   // Serial.print("BPM: %f SpO2: %d %", bpm, spo2);
  //   Serial.print(bpm);
  //   Serial.print("  SpO2: ");
  //   Serial.print(spo2);
  //   Serial.println("%");
  //   // Blynk.virtualWrite(V2, pox.getHeartRate());
  //   // Blynk.virtualWrite(V3, pox.getSpO2());
  //   tsLastReport = millis();
  // }
}

void onBeatDetected()
{
  ;
}

void getSendData()
{
}