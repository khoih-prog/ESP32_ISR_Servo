/****************************************************************************************************************************
  multiFileProject.ino
  For ESP32 boards
  Written by Khoi Hoang
  
  Built by Khoi Hoang https://github.com/khoih-prog/ESP32_ISR_Servo
  Licensed under MIT license
*****************************************************************************************************************************/

// To demo how to include files in multi-file Projects

#ifndef ESP32
  #error This code is designed to run on ESP32 platform, not Arduino nor ESP32! Please check your Tools->Board setting.
#endif

#define ESP32_ISR_SERVO_VERSION_MIN_TARGET      "ESP32_ISR_Servo v1.2.0"
#define ESP32_ISR_SERVO_VERSION_MIN             1002000

#include "multiFileProject.h"

// To be included only in main(), .ino with setup() to avoid `Multiple Definitions` Linker Error
#include "ESP32_ISR_Servo.h"

void setup() 
{
  Serial.begin(115200);
  while (!Serial);
  
  Serial.println("\nStart multiFileProject");
  Serial.println(ESP32_ISR_SERVO_VERSION);

#if defined(ESP32_ISR_SERVO_VERSION_MIN)
  if (ESP32_ISR_SERVO_VERSION_INT < ESP32_ISR_SERVO_VERSION_MIN)
  {
    Serial.print("Warning. Must use this example on Version equal or later than : ");
    Serial.println(ESP32_ISR_SERVO_VERSION_MIN_TARGET);
  }
#endif
}

void loop() 
{
  // put your main code here, to run repeatedly:
}
