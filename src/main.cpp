/**
 * Blink
 *
 * Turns on an LED on for one second,
 * then off for one second, repeatedly.
 */
#include "Arduino.h"
#include "heltec.h"
#include <OneWire.h>
#include <DallasTemperature.h>
#include "batt.h"

//#define PRINT_TEMPERATURES
#define ONE_WIRE_BUS 17
#define NO_OF_TEMP_SENSORS 3
#define BUTTON_PIN 18
#define MODE_SCREEN_OFF 0
#define MODE_SCREEN_ON  1
#define MODE_DEFAULT     MODE_SCREEN_ON

#define READ_BATT_TIME_MS (30*1000)
#define LOOP_TIME_MS (500)
#define SKIP_BATT_READINGS (READ_BATT_TIME_MS/LOOP_TIME_MS)
// Sensors are always read in same order and this list
// puts them so that bottom sensor is always in the first place
// and top sensor in last
uint8_t sensor_index_to_disp_index[3] = {1,2,0};

static int skip_batt_reading_counter = 0;
static float lastTemperature = 0.0f;
static float temperatureChangeSinceLastReading = 0.0f;
static int skipTemperatureReadings = 0;
static int skipTemperatureReadingsCounter = 0;
static volatile uint8_t screenMode;
static volatile bool screenModeChanged;
float temperatures[NO_OF_TEMP_SENSORS];
float battVoltage = 0.0f;
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

void printAddress(DeviceAddress deviceAddress) {
  for (uint8_t i = 0; i < 8; i++){
    if (deviceAddress[i] < 16) Serial.print("0");
      Serial.print(deviceAddress[i], HEX);
  }
}

void buttonPressed() {
  Serial.printf("MODE=%u\n", screenMode);
  if(screenMode == MODE_SCREEN_ON) {
    Serial.print("Setting screen mode OFF\n");
    screenMode = MODE_SCREEN_OFF;
    screenModeChanged = true;
  } else {
    Serial.print("Setting screen mode ON\n");
    screenMode = MODE_SCREEN_ON;
    screenModeChanged = true;
  }
}

void setup()
{
  for(int i=0; i<NO_OF_TEMP_SENSORS; i++) { temperatures[i] = 0; }
  sensors.begin();
  Serial.begin(115200);
  Heltec.begin(true /*DisplayEnable Enable*/, false /*LoRa Disable*/, false /*Serial Enable*/);
  delay(1000);
  screenMode = MODE_DEFAULT;
  screenModeChanged = false;
  Heltec.display->flipScreenVertically();
  Heltec.display->setFont(ArialMT_Plain_10);

  // Button is broken
  //pinMode(BUTTON_PIN, INPUT_PULLUP);
  //attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), buttonPressed, RISING);
  sample_batt_level();
}

float getAvgTemp() {
  float sum = 0;
  for(int i=0; i<NO_OF_TEMP_SENSORS; i++) { sum += temperatures[i]; }
  return sum/NO_OF_TEMP_SENSORS;
}

String buildTemperatureString(){
  String temperatureString = "";
  for(int i=0; i<NO_OF_TEMP_SENSORS; i++) {
    temperatureString += String(temperatures[i], 1);
    if((i+1)<NO_OF_TEMP_SENSORS) {
      temperatureString += String("  ");
    }
  }
  return temperatureString;
}

String buildVoltageString(){
  return String(get_avg_batt_voltage()) + "mV";
}

void draw() {
    if(screenMode == MODE_SCREEN_ON) {
      Heltec.display->clear();
      Heltec.display->setTextAlignment(TEXT_ALIGN_LEFT);
      Heltec.display->setFont(ArialMT_Plain_24);
      Heltec.display->drawString(25, 10, String(getAvgTemp(), 2) + String("C"));
      Heltec.display->setFont(ArialMT_Plain_10);
      Heltec.display->drawString(5, 40, buildTemperatureString());
      Heltec.display->drawString(88, 40, buildVoltageString());
      Heltec.display->display();
    }
}

void readTemperatures(){ 
  sensors.requestTemperatures();
  delay(1); // Seems to help with random -127 readings
#ifdef PRINT_TEMPERATURES
  Serial.println("Readings:");
#endif
  float smallest = 999;
  float greatest = -999;
  DeviceAddress tempDeviceAddress; 
  for(int i=0;i<NO_OF_TEMP_SENSORS; i++){
    if(sensors.getAddress(tempDeviceAddress, i)){
#ifdef PRINT_TEMPERATURES
      Serial.printf("%u, ", i);
      printAddress(tempDeviceAddress);
#endif
      float tempC = sensors.getTempC(tempDeviceAddress);
#ifdef PRINT_TEMPERATURES
      Serial.print("  Temp C: ");
      Serial.println(tempC);
#endif
      if(tempC>greatest) { greatest = tempC; }
      if(tempC<smallest) { smallest = tempC; }
      temperatures[sensor_index_to_disp_index[i]] = tempC;
    }
  }
  float currentTemperature = getAvgTemp();
  temperatureChangeSinceLastReading = fabs(lastTemperature-currentTemperature);
  lastTemperature = currentTemperature;

#ifdef PRINT_TEMPERATURES
  Serial.print("Greatest diff is ");
  Serial.print(greatest-smallest);
  Serial.println("C");
#endif
}

void determineChangeRatio() {
  if(temperatureChangeSinceLastReading > 0.08) {
    skipTemperatureReadings = 0;
  } else if (temperatureChangeSinceLastReading > 0.03) {
    return;
  } else {
    if(skipTemperatureReadings < 20) {
      skipTemperatureReadings++;
    }
  }
}

void loop()
{
  if(skip_batt_reading_counter++ > SKIP_BATT_READINGS) {
    skip_batt_reading_counter = 0;
    sample_batt_level();
  }
  if(skipTemperatureReadingsCounter++ > skipTemperatureReadings) {
    readTemperatures();
    determineChangeRatio();
    skipTemperatureReadingsCounter = 0;
    if(screenMode == MODE_SCREEN_ON && screenModeChanged) {
      Serial.print("Turning screen ON\n");
      screenModeChanged = false;
      // TODO: No idea how these should be used. Docs do not help.
      Heltec.display->resetDisplay();
      Heltec.display->init();
      Heltec.display->displayOn();
      Heltec.display->flipScreenVertically();
    } else if(screenModeChanged) {
      Serial.print("Turning screen OFF\n");
      screenModeChanged = false;
      Heltec.display->resetDisplay();
      Heltec.display->sleep();
      Heltec.display->displayOff();
    }
    draw();
  }
  delay(LOOP_TIME_MS);
}
