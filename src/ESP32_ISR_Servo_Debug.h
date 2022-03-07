/****************************************************************************************************************************
  ESP32_ISR_Servo_Debug.h
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

#ifndef ESP32_ISR_Servo_Debug_h
#define ESP32_ISR_Servo_Debug_h

//////////////////////////////////////////

#ifndef ISR_SERVO_DEBUG
  #define ISR_SERVO_DEBUG               1
#endif

//////////////////////////////////////////

#if !defined(ISR_SERVO_DEBUG_OUTPUT)
  #define ISR_SERVO_DEBUG_OUTPUT    Serial
#endif

//////////////////////////////////////////////////////

const char ISR_SERVO_MARK[]  = "[ISR_SERVO] ";
const char ISR_SERVO_SP[]    = " ";

#define ISR_SERVO_PRINT          ISR_SERVO_DEBUG_OUTPUT.print
#define ISR_SERVO_PRINTLN        ISR_SERVO_DEBUG_OUTPUT.println
#define ISR_SERVO_FLUSH          ISR_SERVO_DEBUG_OUTPUT.flush

#define ISR_SERVO_PRINT_MARK     ISR_SERVO_PRINT(ISR_SERVO_MARK)
#define ISR_SERVO_PRINT_SP       ISR_SERVO_PRINT(ISR_SERVO_SP)

//////////////////////////////////////////////////////

#define ISR_SERVO_LOGERROR(x)         if(ISR_SERVO_DEBUG>0) { ISR_SERVO_PRINT_MARK; ISR_SERVO_PRINTLN(x); }
#define ISR_SERVO_LOGERROR0(x)        if(ISR_SERVO_DEBUG>0) { ISR_SERVO_PRINT(x); }
#define ISR_SERVO_LOGERROR1(x,y)      if(ISR_SERVO_DEBUG>0) { ISR_SERVO_PRINT_MARK; ISR_SERVO_PRINT(x); ISR_SERVO_PRINT_SP; ISR_SERVO_PRINTLN(y); }
#define ISR_SERVO_LOGERROR2(x,y,z)    if(ISR_SERVO_DEBUG>0) { ISR_SERVO_PRINT_MARK; ISR_SERVO_PRINT(x); ISR_SERVO_PRINT_SP; ISR_SERVO_PRINT(y); ISR_SERVO_PRINT_SP; ISR_SERVO_PRINTLN(z); }
#define ISR_SERVO_LOGERROR3(x,y,z,w)  if(ISR_SERVO_DEBUG>0) { ISR_SERVO_PRINT_MARK; ISR_SERVO_PRINT(x); ISR_SERVO_PRINT_SP; ISR_SERVO_PRINT(y); ISR_SERVO_PRINT_SP; ISR_SERVO_PRINT(z); ISR_SERVO_PRINT_SP; ISR_SERVO_PRINTLN(w); }

//////////////////////////////////////////////////////

#define ISR_SERVO_LOGDEBUG(x)         if(ISR_SERVO_DEBUG>1) { ISR_SERVO_PRINT_MARK; ISR_SERVO_PRINTLN(x); }
#define ISR_SERVO_LOGDEBUG0(x)        if(ISR_SERVO_DEBUG>1) { ISR_SERVO_PRINT(x); }
#define ISR_SERVO_LOGDEBUG1(x,y)      if(ISR_SERVO_DEBUG>1) { ISR_SERVO_PRINT_MARK; ISR_SERVO_PRINT(x); ISR_SERVO_PRINT_SP; ISR_SERVO_PRINTLN(y); }
#define ISR_SERVO_LOGDEBUG2(x,y,z)    if(ISR_SERVO_DEBUG>1) { ISR_SERVO_PRINT_MARK; ISR_SERVO_PRINT(x); ISR_SERVO_PRINT_SP; ISR_SERVO_PRINT(y); ISR_SERVO_PRINT_SP; ISR_SERVO_PRINTLN(z); }
#define ISR_SERVO_LOGDEBUG3(x,y,z,w)  if(ISR_SERVO_DEBUG>1) { ISR_SERVO_PRINT_MARK; ISR_SERVO_PRINT(x); ISR_SERVO_PRINT_SP; ISR_SERVO_PRINT(y); ISR_SERVO_PRINT_SP; ISR_SERVO_PRINT(z); ISR_SERVO_PRINT_SP; ISR_SERVO_PRINTLN(w); }

//////////////////////////////////////////


#endif      // ESP8266_ISR_Servo_Debug_h
