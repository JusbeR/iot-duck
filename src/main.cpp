/**
 * Blink
 *
 * Turns on an LED on for one second,
 * then off for one second, repeatedly.
 */
#include "Arduino.h"
#include "heltec.h"

#ifndef LED_BUILTIN
#define LED_BUILTIN 13
#endif

void setup()
{
  // initialize LED digital pin as an output.
  //pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(9600);
  Heltec.begin(true /*DisplayEnable Enable*/, false /*LoRa Disable*/, false /*Serial Enable*/);



  Heltec.display->flipScreenVertically();
Heltec.display->setFont(ArialMT_Plain_10);
}

void drawFontFaceDemo() {
    // Font Demo1
    // create more fonts at http://oleddisplay.squix.ch/
    Heltec.display->clear();
    Heltec.display->setTextAlignment(TEXT_ALIGN_LEFT);
    /*Heltec.display->setFont(ArialMT_Plain_10);
    Heltec.display->drawString(0, 0, "Hello world");
    Heltec.display->setFont(ArialMT_Plain_16);
    Heltec.display->drawString(0, 10, "Hello world");*/
    Heltec.display->setFont(ArialMT_Plain_24);
    Heltec.display->drawString(5, 20, "LoopDuck");
    Heltec.display->setFont(ArialMT_Plain_10);
    Heltec.display->drawString(40, 50, "is alive!");
    Heltec.display->display();
}

void loop()
{
  drawFontFaceDemo();
  // turn the LED on (HIGH is the voltage level)
  //digitalWrite(LED_BUILTIN, HIGH);
  Serial.println("Hello world!");
  

  // wait for a second
  delay(1000);

  // turn the LED off by making the voltage LOW
  //digitalWrite(LED_BUILTIN, LOW);

   // wait for a second
  delay(1000);
}
