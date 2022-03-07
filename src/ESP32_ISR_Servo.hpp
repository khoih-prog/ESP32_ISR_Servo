/****************************************************************************************************************************
  ESP32_ISR_Servo.h
  For ESP32 boards
  Written by Khoi Hoang

  Built by Khoi Hoang https://github.com/khoih-prog/ESP32_ISR_Servo
  Licensed under MIT license

  Now with these new 16 ISR-based timers, the maximum interval is practically unlimited (limited only by unsigned long miliseconds)
  The accuracy is nearly perfect compared to software timers. The most important feature is they're ISR-based timers
  Therefore, their executions are not blocked by bad-behaving functions / tasks.
  This important feature is absolutely necessary for mission-critical tasks.

  Version: 1.2.1

  Version Modified By   Date      Comments
  ------- -----------  ---------- -----------
  1.0.0   K Hoang      12/12/2019 Initial coding
  1.0.1   K Hoang      13/12/2019 Add more features getPosition and getPulseWidth. Optimize.
  1.0.2   K Hoang      20/12/2019 Add more Blynk examples.Change example names to avoid duplication.
  1.1.0   K Hoang      03/01/2021 Fix bug. Add TOC and Version String.
  1.2.0   K Hoang      06/03/2022 Convert to `h-only` style. Optimize code by using passing by `reference`
  1.2.1   K Hoang      07/03/2022 Fix bug
 *****************************************************************************************************************************/

#pragma once

#ifndef ESP32_ISR_SERVO_HPP
#define ESP32_ISR_SERVO_HPP

#if ( ARDUINO_ESP32S2_DEV || ARDUINO_FEATHERS2 || ARDUINO_ESP32S2_THING_PLUS || ARDUINO_MICROS2 || \
        ARDUINO_METRO_ESP32S2 || ARDUINO_MAGTAG29_ESP32S2 || ARDUINO_FUNHOUSE_ESP32S2 || \
        ARDUINO_ADAFRUIT_FEATHER_ESP32S2_NOPSRAM )
  #define USING_ESP32_S2_TIMERINTERRUPT         true
#elif ( defined(ARDUINO_ESP32S3_DEV) || defined(ARDUINO_ESP32_S3_BOX) || defined(ARDUINO_TINYS3) || \
        defined(ARDUINO_PROS3) || defined(ARDUINO_FEATHERS3) )
  #define USING_ESP32_S3_TIMERINTERRUPT         true
#elif ( ARDUINO_ESP32C3_DEV )
  #define USING_ESP32_C3_TIMERINTERRUPT         true  
#elif defined(ESP32)
  #define USING_ESP32_TIMERINTERRUPT            true  
#else
  #error This code is intended to run on the ESP32 platform! Please check your Tools->Board setting.  
#endif

#if !defined(ESP32_ISR_SERVO_VERSION)
  #define ESP32_ISR_SERVO_VERSION             "ESP32_ISR_Servo v1.2.1"
  
  #define ESP32_ISR_SERVO_VERSION_MAJOR       1
  #define ESP32_ISR_SERVO_VERSION_MINOR       2
  #define ESP32_ISR_SERVO_VERSION_PATCH       1

  #define ESP32_ISR_SERVO_VERSION_INT         1002001
  
#endif

#include <stddef.h>

#include <inttypes.h>

#if defined(ARDUINO)
  #if ARDUINO >= 100
    #include <Arduino.h>
  #else
    #include <WProgram.h>
  #endif
#endif

#ifndef ISR_SERVO_DEBUG
  #define ISR_SERVO_DEBUG       0
#endif

#include "ESP32_ISR_Servo_Debug.h"

#include "ESP32FastTimerInterrupt.hpp"

#define ESP32_MAX_PIN           39
#define ESP32_WRONG_PIN         255

// From Servo.h - Copyright (c) 2009 Michael Margolis.  All right reserved.

#define MIN_PULSE_WIDTH         544       // the shortest pulse sent to a servo  
#define MAX_PULSE_WIDTH         2400      // the longest pulse sent to a servo 
#define DEFAULT_PULSE_WIDTH     1500      // default pulse width when servo is attached
#define REFRESH_INTERVAL        20000     // minumim time to refresh servos in microseconds 

extern void IRAM_ATTR ESP32_ISR_Servo_Handler();

class ESP32_ISR_Servo
{

  public:
    // maximum number of servos
    const static int MAX_SERVOS = 16;

    // constructor
    ESP32_ISR_Servo();

    // destructor
    ~ESP32_ISR_Servo()
    {
      if (ESP32_ITimer)
      {
        ESP32_ITimer->detachInterrupt();
        delete ESP32_ITimer;
      }
    }

    void IRAM_ATTR run();

    // useTimer select which timer (0-3) of ESP32 to use for Servos
    //Return true if timerN0 in range
    bool useTimer(const uint8_t& timerNo)
    {
      if (timerNo < MAX_ESP32_NUM_TIMERS)
      {
        _timerNo = timerNo;
        return true;
      }
      return false;
    }

    // Bind servo to the timer and pin, return servoIndex
    int8_t setupServo(const uint8_t& pin, const uint16_t& min = MIN_PULSE_WIDTH, const uint16_t& max = MAX_PULSE_WIDTH);

    // setPosition will set servo to position in degrees
    // by using PWM, turn HIGH 'duration' microseconds within REFRESH_INTERVAL (20000us)
    // returns true on success or -1 on wrong servoIndex
    bool setPosition(const uint8_t& servoIndex, const uint16_t& position);

    // returns last position in degrees if success, or -1 on wrong servoIndex
    int getPosition(const uint8_t& servoIndex);

    // setPulseWidth will set servo PWM Pulse Width in microseconds, correcponding to certain position in degrees
    // by using PWM, turn HIGH 'pulseWidth' microseconds within REFRESH_INTERVAL (20000us)
    // min and max for each individual servo are enforced
    // returns true on success or -1 on wrong servoIndex
    bool setPulseWidth(const uint8_t& servoIndex, uint16_t& pulseWidth);

    // returns pulseWidth in microsecs (within min/max range) if success, or 0 on wrong servoIndex
    unsigned int getPulseWidth(const uint8_t& servoIndex);

    // destroy the specified servo
    void deleteServo(const uint8_t& servoIndex);

    // returns true if the specified servo is enabled
    bool isEnabled(const uint8_t& servoIndex);

    // enables the specified servo
    bool enable(const uint8_t& servoIndex);

    // disables the specified servo
    bool disable(const uint8_t& servoIndex);

    // enables all servos
    void enableAll();

    // disables all servos
    void disableAll();

    // enables the specified servo if it's currently disabled,
    // and vice-versa
    bool toggle(const uint8_t& servoIndex);

    // returns the number of used servos
    int8_t getNumServos();

    // returns the number of available servos
    int8_t getNumAvailableServos() 
    {
      if (numServos <= 0)
        return MAX_SERVOS;
      else 
        return MAX_SERVOS - numServos;
    };

  private:

    // Use 10 microsecs timer, just fine enough to control Servo, normally requiring pulse width (PWM) 500-2000us in 20ms.
#define TIMER_INTERVAL_MICRO        10

    void init()
    {

      ESP32_ITimer = new ESP32FastTimer(_timerNo);

      // Interval in microsecs
      if ( ESP32_ITimer && ESP32_ITimer->attachInterruptInterval(TIMER_INTERVAL_MICRO, (timer_callback) ESP32_ISR_Servo_Handler ) )
      {
        ISR_SERVO_LOGERROR("Starting  ITimer OK");
      }
      else
      {
        ISR_SERVO_LOGERROR("Fail setup ESP32_ITimer");      }

      for (int servoIndex = 0; servoIndex < MAX_SERVOS; servoIndex++)
      {
        memset((void*) &servo[servoIndex], 0, sizeof (servo_t));
        servo[servoIndex].count    = 0;
        servo[servoIndex].enabled  = false;
        // Intentional bad pin
        servo[servoIndex].pin      = ESP32_WRONG_PIN;
      }

      numServos   = 0;

      // Init timerCount
      timerCount  = 1;

      timerMux = portMUX_INITIALIZER_UNLOCKED;
    }

    // find the first available slot
    int8_t findFirstFreeSlot();

    typedef struct
    {
      uint8_t       pin;                  // pin servo connected to
      unsigned long count;                // In microsecs
      uint16_t      position;             // In degrees
      bool          enabled;              // true if enabled
      uint16_t      min;
      uint16_t      max;
    } servo_t;

    volatile servo_t servo[MAX_SERVOS];

    // actual number of servos in use (-1 means uninitialized)
    volatile int8_t numServos;

    // timerCount starts at 1, and counting up to (REFRESH_INTERVAL / TIMER_INTERVAL_MICRO) = (20000 / 10) = 2000
    // then reset to 1. Use this to calculate when to turn ON / OFF pulse to servo
    // For example, servo1 uses pulse width 1000us => turned ON when timerCount = 1, turned OFF when timerCount = 1000 / TIMER_INTERVAL_MICRO = 100
    volatile unsigned long timerCount;

    // ESP32 is a multi core / multi processing chip. It is mandatory to disable task switches during ISR
    portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;

    // For ESP32 timer
    uint8_t _timerNo;
    ESP32FastTimer* ESP32_ITimer;
};


#endif    // ESP32_ISR_SERVO_HPP
