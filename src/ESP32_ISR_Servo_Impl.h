/****************************************************************************************************************************
  ESP32_ISR_Servo.cpp
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

#include "ESP32_ISR_Servo.h"
#include <string.h>

#ifndef ISR_SERVO_DEBUG
  #define ISR_SERVO_DEBUG               1
#endif

#define DEFAULT_ESP32_TIMER_NO          3

static ESP32_ISR_Servo ESP32_ISR_Servos;  // create servo object to control up to 16 servos

void IRAM_ATTR ESP32_ISR_Servo_Handler()
{
  ESP32_ISR_Servos.run();
}

ESP32_ISR_Servo::ESP32_ISR_Servo()
  : numServos (-1), _timerNo(DEFAULT_ESP32_TIMER_NO)
{
}

void IRAM_ATTR ESP32_ISR_Servo::run()
{
  static int servoIndex;

  // ESP32 is a multi core / multi processing chip. It is mandatory to disable task switches during ISR
  portENTER_CRITICAL_ISR(&timerMux);

  for (servoIndex = 0; servoIndex < MAX_SERVOS; servoIndex++)
  {
    if ( servo[servoIndex].enabled  && (servo[servoIndex].pin <= ESP32_MAX_PIN) )
    {
      if ( timerCount == servo[servoIndex].count )
      {
        // PWM to LOW, will be HIGH again when timerCount = 1
        digitalWrite(servo[servoIndex].pin, LOW);
      }
      else if (timerCount == 1)
      {
        // PWM to HIGH, will be LOW again when timerCount = servo[servoIndex].count
        digitalWrite(servo[servoIndex].pin, HIGH);
      }
    }
  }

  // Reset when reaching 20000us / 10us = 2000
  if (timerCount++ >= REFRESH_INTERVAL / TIMER_INTERVAL_MICRO)
  {
    ISR_SERVO_LOGDEBUG("Reset count");

    timerCount = 1;
  }

  // ESP32 is a multi core / multi processing chip. It is mandatory to disable task switches during ISR
  portEXIT_CRITICAL_ISR(&timerMux);
}

// find the first available slot
// return -1 if none found
int8_t ESP32_ISR_Servo::findFirstFreeSlot()
{
  // all slots are used
  if (numServos >= MAX_SERVOS)
    return -1;

  // return the first slot with no count (i.e. free)
  for (int servoIndex = 0; servoIndex < MAX_SERVOS; servoIndex++)
  {
    if (servo[servoIndex].enabled == false)
    {
      ISR_SERVO_LOGDEBUG1("Index =", servoIndex);

      return servoIndex;
    }
  }

  // no free slots found
  return -1;
}

int8_t ESP32_ISR_Servo::setupServo(const uint8_t& pin, const uint16_t& min, const uint16_t& max)
{
  int servoIndex;

  if (pin > ESP32_MAX_PIN)
    return -1;

  if (numServos < 0)
    init();

  servoIndex = findFirstFreeSlot();

  if (servoIndex < 0)
    return -1;

  servo[servoIndex].pin        = pin;
  servo[servoIndex].min        = min;
  servo[servoIndex].max        = max;
  servo[servoIndex].count      = min / TIMER_INTERVAL_MICRO;
  servo[servoIndex].position   = 0;
  servo[servoIndex].enabled    = true;

  pinMode(pin, OUTPUT);

  numServos++;

  ISR_SERVO_LOGDEBUG3("Index =", servoIndex, ", count =", servo[servoIndex].count);
  ISR_SERVO_LOGDEBUG3("min =", servo[servoIndex].min, ", max =", servo[servoIndex].max);

  return servoIndex;
}

bool ESP32_ISR_Servo::setPosition(const uint8_t& servoIndex, const uint16_t& position)
{
  if (servoIndex >= MAX_SERVOS)
    return false;

  // Updates interval of existing specified servo
  if ( servo[servoIndex].enabled && (servo[servoIndex].pin <= ESP32_MAX_PIN) )
  {
    // ESP32 is a multi core / multi processing chip.
    // It is mandatory to disable task switches during modifying shared vars
    portENTER_CRITICAL(&timerMux);

    servo[servoIndex].position  = position;
    servo[servoIndex].count     = map(position, 0, 180, servo[servoIndex].min, servo[servoIndex].max) / TIMER_INTERVAL_MICRO;

    // ESP32 is a multi core / multi processing chip.
    // It is mandatory to disable task switches during modifying shared vars
    portEXIT_CRITICAL(&timerMux);

    ISR_SERVO_LOGERROR1("Idx =", servoIndex);
    ISR_SERVO_LOGERROR3("cnt =", servo[servoIndex].count, ", pos =",servo[servoIndex].position);

    return true;
  }

  // false return for non-used numServo or bad pin
  return false;
}

// returns last position in degrees if success, or -1 on wrong servoIndex
int ESP32_ISR_Servo::getPosition(const uint8_t& servoIndex)
{
  if (servoIndex >= MAX_SERVOS)
    return -1;

  // Updates interval of existing specified servo
  if ( servo[servoIndex].enabled && (servo[servoIndex].pin <= ESP32_MAX_PIN) )
  {
    ISR_SERVO_LOGERROR1("Idx =", servoIndex);
    ISR_SERVO_LOGERROR3("cnt =", servo[servoIndex].count, ", pos =",servo[servoIndex].position);

    return (servo[servoIndex].position);
  }

  // return 0 for non-used numServo or bad pin
  return -1;
}


// setPulseWidth will set servo PWM Pulse Width in microseconds, correcponding to certain position in degrees
// by using PWM, turn HIGH 'pulseWidth' microseconds within REFRESH_INTERVAL (20000us)
// min and max for each individual servo are enforced
// returns true on success or -1 on wrong servoIndex
bool ESP32_ISR_Servo::setPulseWidth(const uint8_t& servoIndex, uint16_t& pulseWidth)
{
  if (servoIndex >= MAX_SERVOS)
    return false;

  // Updates interval of existing specified servo
  if ( servo[servoIndex].enabled && (servo[servoIndex].pin <= ESP32_MAX_PIN) )
  {
    if (pulseWidth < servo[servoIndex].min)
      pulseWidth = servo[servoIndex].min;
    else if (pulseWidth > servo[servoIndex].max)
      pulseWidth = servo[servoIndex].max;

    // ESP32 is a multi core / multi processing chip.
    // It is mandatory to disable task switches during modifying shared vars
    portENTER_CRITICAL(&timerMux);

    servo[servoIndex].count     = pulseWidth / TIMER_INTERVAL_MICRO;
    servo[servoIndex].position  = map(pulseWidth, servo[servoIndex].min, servo[servoIndex].max, 0, 180);

    // ESP32 is a multi core / multi processing chip.
    // It is mandatory to disable task switches during modifying shared vars
    portEXIT_CRITICAL(&timerMux);

    ISR_SERVO_LOGERROR1("Idx =", servoIndex);
    ISR_SERVO_LOGERROR3("cnt =", servo[servoIndex].count, ", pos =",servo[servoIndex].position);

    return true;
  }

  // false return for non-used numServo or bad pin
  return false;
}

// returns pulseWidth in microsecs (within min/max range) if success, or 0 on wrong servoIndex
unsigned int ESP32_ISR_Servo::getPulseWidth(const uint8_t& servoIndex)
{
  if (servoIndex >= MAX_SERVOS)
    return 0;

  // Updates interval of existing specified servo
  if ( servo[servoIndex].enabled && (servo[servoIndex].pin <= ESP32_MAX_PIN) )
  {
    ISR_SERVO_LOGERROR1("Idx =", servoIndex);
    ISR_SERVO_LOGERROR3("cnt =", servo[servoIndex].count, ", pos =",servo[servoIndex].position);

    return (servo[servoIndex].count * TIMER_INTERVAL_MICRO );
  }

  // return 0 for non-used numServo or bad pin
  return 0;
}


void ESP32_ISR_Servo::deleteServo(const uint8_t& servoIndex)
{
  if ( (numServos == 0) || (servoIndex >= MAX_SERVOS) )
  {
    return;
  }

  // ESP32 is a multi core / multi processing chip.
  // It is mandatory to disable task switches during modifying shared vars
  portENTER_CRITICAL(&timerMux);

  // don't decrease the number of servos if the specified slot is already empty
  if (servo[servoIndex].enabled)
  {
    memset((void*) &servo[servoIndex], 0, sizeof (servo_t));

    servo[servoIndex].enabled   = false;
    servo[servoIndex].position  = 0;
    servo[servoIndex].count     = 0;
    // Intentional bad pin, good only from 0-16 for Digital, A0=17
    servo[servoIndex].pin       = ESP32_WRONG_PIN;

    // update number of servos
    numServos--;
  }

  // ESP32 is a multi core / multi processing chip.
  // It is mandatory to disable task switches during modifying shared vars
  portEXIT_CRITICAL(&timerMux);
}

bool ESP32_ISR_Servo::isEnabled(const uint8_t& servoIndex)
{
  if (servoIndex >= MAX_SERVOS)
    return false;

  if (servo[servoIndex].pin > ESP32_MAX_PIN)
  {
    // Disable if something wrong
    servo[servoIndex].pin     = ESP32_WRONG_PIN;
    servo[servoIndex].enabled = false;
    return false;
  }

  return servo[servoIndex].enabled;
}

bool ESP32_ISR_Servo::enable(const uint8_t& servoIndex)
{
  if (servoIndex >= MAX_SERVOS)
    return false;

  // ESP32 is a multi core / multi processing chip.
  // It is mandatory to disable task switches during modifying shared vars
  portENTER_CRITICAL(&timerMux);

  if (servo[servoIndex].pin > ESP32_MAX_PIN)
  {
    // Disable if something wrong
    servo[servoIndex].pin     = ESP32_WRONG_PIN;
    servo[servoIndex].enabled = false;
    return false;
  }

  // Bug fix. See "Fixed count >= min comparison for servo enable."
  // (https://github.com/khoih-prog/ESP32_ISR_Servo/pull/1)
  if ( servo[servoIndex].count >= servo[servoIndex].min / TIMER_INTERVAL_MICRO )
    servo[servoIndex].enabled = true;

  // ESP32 is a multi core / multi processing chip.
  // It is mandatory to disable task switches during modifying shared vars
  portEXIT_CRITICAL(&timerMux);

  return true;
}

bool ESP32_ISR_Servo::disable(const uint8_t& servoIndex)
{
  if (servoIndex >= MAX_SERVOS)
    return false;

  if (servo[servoIndex].pin > ESP32_MAX_PIN)
    servo[servoIndex].pin     = ESP32_WRONG_PIN;

  servo[servoIndex].enabled = false;

  return true;
}

void ESP32_ISR_Servo::enableAll()
{
  // Enable all servos with a enabled and count != 0 (has PWM) and good pin

  // ESP32 is a multi core / multi processing chip.
  // It is mandatory to disable task switches during modifying shared vars
  portENTER_CRITICAL(&timerMux);

  for (int servoIndex = 0; servoIndex < MAX_SERVOS; servoIndex++)
  {
    // Bug fix. See "Fixed count >= min comparison for servo enable."
    // (https://github.com/khoih-prog/ESP32_ISR_Servo/pull/1)
    if ( (servo[servoIndex].count >= servo[servoIndex].min / TIMER_INTERVAL_MICRO ) && !servo[servoIndex].enabled 
      && (servo[servoIndex].pin <= ESP32_MAX_PIN) )
    {
      servo[servoIndex].enabled = true;
    }
  }

  // ESP32 is a multi core / multi processing chip.
  // It is mandatory to disable task switches during modifying shared vars
  portEXIT_CRITICAL(&timerMux);
}

void ESP32_ISR_Servo::disableAll()
{
  // ESP32 is a multi core / multi processing chip.
  // It is mandatory to disable task switches during modifying shared vars
  portENTER_CRITICAL(&timerMux);

  // Disable all servos
  for (int servoIndex = 0; servoIndex < MAX_SERVOS; servoIndex++)
  {
    servo[servoIndex].enabled = false;
  }

  // ESP32 is a multi core / multi processing chip.
  // It is mandatory to disable task switches during modifying shared vars
  portEXIT_CRITICAL(&timerMux);
}

bool ESP32_ISR_Servo::toggle(const uint8_t& servoIndex)
{
  if (servoIndex >= MAX_SERVOS)
    return false;

  // ESP32 is a multi core / multi processing chip.
  // It is mandatory to disable task switches during modifying shared vars
  portENTER_CRITICAL(&timerMux);

  servo[servoIndex].enabled = !servo[servoIndex].enabled;

  // ESP32 is a multi core / multi processing chip.
  // It is mandatory to disable task switches during modifying shared vars
  portEXIT_CRITICAL(&timerMux);

  return true;
}

int8_t ESP32_ISR_Servo::getNumServos()
{
  return numServos;
}
