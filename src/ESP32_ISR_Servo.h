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

#ifndef ESP32_ISR_SERVO_H
#define ESP32_ISR_SERVO_H

#include "ESP32_ISR_Servo.hpp"
#include "ESP32_ISR_Servo_Impl.h"


#endif
