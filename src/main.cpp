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

#define ONE_WIRE_BUS 17
#define NO_OF_TEMP_SENSORS 4

float temperatures[NO_OF_TEMP_SENSORS];
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

void printAddress(DeviceAddress deviceAddress) {
  for (uint8_t i = 0; i < 8; i++){
    if (deviceAddress[i] < 16) Serial.print("0");
      Serial.print(deviceAddress[i], HEX);
  }
}

void setup()
{
  for(int i=0; i<NO_OF_TEMP_SENSORS; i++) { temperatures[i] = 0; }
  sensors.begin();
  Serial.begin(9600);
  Heltec.begin(true /*DisplayEnable Enable*/, false /*LoRa Disable*/, false /*Serial Enable*/);

  Heltec.display->flipScreenVertically();
  Heltec.display->setFont(ArialMT_Plain_10);
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
      temperatureString += String(", ");
    }
  }
  return temperatureString;
}

void drawTemperatures() {
    Heltec.display->clear();
    Heltec.display->setTextAlignment(TEXT_ALIGN_LEFT);
    Heltec.display->setFont(ArialMT_Plain_24);
    Heltec.display->drawString(25, 10, String(getAvgTemp(), 2) + String("C"));
    Heltec.display->setFont(ArialMT_Plain_10);
    Heltec.display->drawString(5, 40, buildTemperatureString());
    Heltec.display->display();
}

void readTemperatures(){ 
  sensors.requestTemperatures();
  delay(1); // Seems to help with random -127 readings
  Serial.println("Readings:");
  float smallest = 999;
  float greatest = -999;
  DeviceAddress tempDeviceAddress; 
  for(int i=0;i<NO_OF_TEMP_SENSORS; i++){
    if(sensors.getAddress(tempDeviceAddress, i)){
      Serial.printf("%u, ", i);
      printAddress(tempDeviceAddress);
      float tempC = sensors.getTempC(tempDeviceAddress);
      Serial.print("  Temp C: ");
      Serial.println(tempC);
      if(tempC>greatest) { greatest = tempC; }
      if(tempC<smallest) { smallest = tempC; }
      temperatures[i] = tempC;
    }
  }
  Serial.print("Greatest diff is ");
  Serial.print(greatest-smallest);
  Serial.println("C");
}

void loop()
{
  readTemperatures();
  drawTemperatures();
  delay(2000);
}
