# ESP32_ISR_Servo Library

[![arduino-library-badge](https://www.ardu-badge.com/badge/ESP32_ISR_Servo.svg?)](https://www.ardu-badge.com/ESP32_ISR_Servo)
[![GitHub release](https://img.shields.io/github/release/khoih-prog/ESP32_ISR_Servo.svg)](https://github.com/khoih-prog/ESP32_ISR_Servo/releases)
[![GitHub](https://img.shields.io/github/license/mashape/apistatus.svg)](https://github.com/khoih-prog/ESP32_ISR_Servo/blob/master/LICENSE)
[![contributions welcome](https://img.shields.io/badge/contributions-welcome-brightgreen.svg?style=flat)](#Contributing)
[![GitHub issues](https://img.shields.io/github/issues/khoih-prog/ESP32_ISR_Servo.svg)](http://github.com/khoih-prog/ESP32_ISR_Servo/issues)

This library enables you to use `1 Hardware Timer` on an ESP32-based board to control up to `16 independent servo motors`.

***Why do we need this ISR-based Servo controller?***

Imagine you have a system with a `mission-critical function`, controlling a `robot arm` or doing something much more important. You normally use a software timer to poll, or even place the function in loop(). But what if another function is blocking the loop() or setup().

So your function might not be executed, and the result would be disastrous.

You'd prefer to have your function called, no matter what happening with other functions (busy loop, bug, etc.).

The correct choice is to use a Hardware Timer with Interrupt to call your function.

These hardware timers, using interrupt, still work even if other functions are blocking. Moreover, they are much more precise (certainly depending on clock frequency accuracy) than other software timers using millis() or micros(). That's necessary if you need to measure some data requiring better accuracy.

Functions using normal software timers, relying on loop() and calling millis(), won't work if the loop() or setup() is blocked by certain operation. For example, certain function is blocking while it's connecting to WiFi or some services.

The catch is your function is now part of an `ISR (Interrupt Service Routine)`, and must be `lean / mean`, and follow certain rules. More to read on:

[Attach Interrupt](https://www.arduino.cc/reference/en/language/functions/external-interrupts/attachinterrupt/)

**Important Notes:**
1. Inside the attached function, `delay()` won’t work and the value returned by `millis()` will not increment. Serial data received while in the function may be lost. You should declare as volatile any variables that you modify within the attached function.

2. Typically global variables are used to pass data between an ISR and the main program. To make sure variables shared between an ISR and the main program are updated correctly, declare them as `volatile`.

## Installation

It's better to use `Arduino Library Manager` to install this library.

Otherwise, you can install it using the following steps:
1. Navigate to [ESP32_ISR_Servo](https://github.com/khoih-prog/ESP32_ISR_Servo) page.
2. Download the latest release `ESP32_ISR_Servo-master.zip`.
3. Extract the zip file to `ESP32_ISR_Servo-master` directory 
4. Copy whole folder to Arduino libraries' directory such as `.Arduino/libraries/ESP32_ISR_Servo-master`.

## More useful Information

The ESP32 has two timer groups, each one with two general purpose hardware timers. 
All the timers are based on 64 bits counters and 16 bit prescalers. 
The timer counters can be configured to count up or down and support automatic reload and software reload.
They can also generate alarms when they reach a specific value, defined by the software. The value of the counter can be read by 
the software program.

## New from v1.0.1

1. Add functions `getPosition()` and `getPulseWidth()`
2. Optimize the code
3. Add more complicated example

Now these new `16 ISR-based Servo controllers` just use one ESP32 Hardware Timer. The number 16 is just arbitrarily chosen, and depending 
on application, you can increase that number to 32, 48, etc. without problem.
The accuracy is nearly perfect compared to software timers. The most important feature is they're ISR-based timers
Therefore, their executions are not blocked by bad-behaving functions / tasks.
This important feature is absolutely necessary for mission-critical tasks. 

The `MultipleServos` example, which controls 6 servos independently, will demonstrate the nearly perfect accuracy.
Being ISR-based servo controllers, their executions are not blocked by bad-behaving functions / tasks, such as connecting to WiFi, Internet
and Blynk services.

This non-being-blocked important feature is absolutely necessary for mission-critical tasks. 
You'll see blynkTimer Software is blocked while system is connecting to WiFi / Internet / Blynk, as well as by blocking task 
in loop(), using delay() function as an example. The elapsed time then is very unaccurate

## Supported Boards

- ESP32

## Usage

How to use:

```
#ifndef ESP32
#error This code is designed to run on ESP32 platform, not Arduino nor ESP8266! Please check your Tools->Board setting.
#endif

#define TIMER_INTERRUPT_DEBUG       1
#define ISR_SERVO_DEBUG             1

// Select different ESP32 timer number (0-3) to avoid conflict
#define USE_ESP32_TIMER_NO          3

#include "ESP32_ISR_Servo.h"

#define PIN_D25           25        // Pin D25 mapped to pin GPIO25/ADC18/DAC1 of ESP32
#define PIN_D26           26        // Pin D26 mapped to pin GPIO26/ADC19/DAC2 of ESP32

// Published values for SG90 servos; adjust if needed
#define MIN_MICROS      800  //544
#define MAX_MICROS      2450

int servoIndex1  = -1;
int servoIndex2  = -1;

void setup() 
{
  Serial.begin(115200);
  Serial.println("\nStarting");
  
  //Select ESP32 timer USE_ESP32_TIMER_NO
  ESP32_ISR_Servos.useTimer(USE_ESP32_TIMER_NO);
  
  servoIndex1 = ESP32_ISR_Servos.setupServo(PIN_D25, MIN_MICROS, MAX_MICROS);
  servoIndex2 = ESP32_ISR_Servos.setupServo(PIN_D26, MIN_MICROS, MAX_MICROS);
  
  if (servoIndex1 != -1)
    Serial.println("Setup Servo1 OK");
  else
    Serial.println("Setup Servo1 failed");

  if (servoIndex2 != -1)
    Serial.println("Setup Servo2 OK");
  else
    Serial.println("Setup Servo2 failed");    
}

void loop() 
{
  int position;
  
  if ( ( servoIndex1 != -1) && ( servoIndex2 != -1) )
  {
    for (position = 0; position <= 180; position++) 
    { 
      // goes from 0 degrees to 180 degrees
      // in steps of 1 degree

      if (position %30 == 0)
      {
        Serial.println("Servo1 pos = " + String(position) + ", Servo2 pos = " + String(180 - position) );
      }
          
      ESP32_ISR_Servos.setPosition(servoIndex1, position);
      ESP32_ISR_Servos.setPosition(servoIndex2, 180 - position);
      // waits 30ms for the servo to reach the position
      delay(30);
    }
    delay(5000);
    
    for (position = 180; position >= 0; position--) 
    { 
      // goes from 180 degrees to 0 degrees
      if (position %30 == 0)
      {
        Serial.println("Servo1 pos = " + String(position) + ", Servo2 pos = " + String(180 - position) );
      }

      ESP32_ISR_Servos.setPosition(servoIndex1, position);
      ESP32_ISR_Servos.setPosition(servoIndex2, 180 - position);
      // waits 30ms for the servo to reach the position
      delay(30);                                  
    }
    delay(5000);

  }
}

```
## TO DO

1. Search for bug and improvement.
2. Similar features for Arduino (UNO, Mega, etc...)


## DONE

#### For current version v1.0.2

1. Add example using [Blynk](http://docs.blynk.cc/) to control servos. Change example names to avoid duplication.

#### For current version v1.0.1

1. Basic 16 ISR-based servo controllers using 1 hardware timer for ESP32.


## Contributing
If you want to contribute to this project:
- Report bugs and errors
- Ask for enhancements
- Create issues and pull requests
- Tell other people about this library

## Copyright
Copyright 2019- Khoi Hoang
