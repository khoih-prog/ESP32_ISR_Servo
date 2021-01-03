/****************************************************************************************************************************
   For ESP32 boards
   Written by Khoi Hoang

   Built by Khoi Hoang https://github.com/khoih-prog/ESP32_ISR_Servo
   Licensed under MIT license
   
   Now with these new 16 ISR-based timers, the maximum interval is practically unlimited (limited only by unsigned long miliseconds)
   The accuracy is nearly perfect compared to software timers. The most important feature is they're ISR-based timers
   Therefore, their executions are not blocked by bad-behaving functions / tasks.
   This important feature is absolutely necessary for mission-critical tasks.

   Loosely based on SimpleTimer - A timer library for Arduino.
   Author: mromani@ottotecnica.com
   Copyright (c) 2010 OTTOTECNICA Italy

   Based on BlynkTimer.h
   Author: Volodymyr Shymanskyy
   
   Version: 1.1.0

   Version Modified By   Date      Comments
   ------- -----------  ---------- -----------
    1.0.0   K Hoang      12/12/2019 Initial coding
    1.0.1   K Hoang      13/12/2019 Add more features getPosition and getPulseWidth. Optimize.
    1.0.2   K Hoang      20/12/2019 Add more Blynk examples.Change example names to avoid duplication.
    1.1.0   K Hoang      03/01/2021 Fix bug. Add TOC and Version String.
 *****************************************************************************************************************************/

#pragma once

#ifndef ESP32FastTimerInterrupt_h
#define ESP32FastTimerInterrupt_h

#ifndef ESP32
  #error This code is designed to run on ESP32 platform, not Arduino nor ESP8266! Please check your Tools->Board setting.
#endif

#ifndef TIMER_INTERRUPT_DEBUG
  #define TIMER_INTERRUPT_DEBUG         1
#endif

#ifndef ISR_SERVO_DEBUG
  #define ISR_SERVO_DEBUG               1
#endif

//////////////////////////////////////////

#if !defined(ISR_SERVO_DEBUG_OUTPUT)
  #define ISR_SERVO_DEBUG_OUTPUT    Serial
#endif

#define ISR_SERVO_LOGERROR(x)         if(ISR_SERVO_DEBUG>0) { ISR_SERVO_DEBUG_OUTPUT.print("[ISR_SERVO] "); ISR_SERVO_DEBUG_OUTPUT.println(x); }
#define ISR_SERVO_LOGERROR0(x)        if(ISR_SERVO_DEBUG>0) { ISR_SERVO_DEBUG_OUTPUT.print(x); }
#define ISR_SERVO_LOGERROR1(x,y)      if(ISR_SERVO_DEBUG>0) { ISR_SERVO_DEBUG_OUTPUT.print("[ISR_SERVO] "); ISR_SERVO_DEBUG_OUTPUT.print(x); ISR_SERVO_DEBUG_OUTPUT.print(" "); ISR_SERVO_DEBUG_OUTPUT.println(y); }
#define ISR_SERVO_LOGERROR2(x,y,z)    if(ISR_SERVO_DEBUG>0) { ISR_SERVO_DEBUG_OUTPUT.print("[ISR_SERVO] "); ISR_SERVO_DEBUG_OUTPUT.print(x); ISR_SERVO_DEBUG_OUTPUT.print(" "); ISR_SERVO_DEBUG_OUTPUT.print(y); ISR_SERVO_DEBUG_OUTPUT.print(" "); ISR_SERVO_DEBUG_OUTPUT.println(z); }
#define ISR_SERVO_LOGERROR3(x,y,z,w)  if(ISR_SERVO_DEBUG>0) { ISR_SERVO_DEBUG_OUTPUT.print("[ISR_SERVO] "); ISR_SERVO_DEBUG_OUTPUT.print(x); ISR_SERVO_DEBUG_OUTPUT.print(" "); ISR_SERVO_DEBUG_OUTPUT.print(y); ISR_SERVO_DEBUG_OUTPUT.print(" "); ISR_SERVO_DEBUG_OUTPUT.print(z); ISR_SERVO_DEBUG_OUTPUT.print(" "); ISR_SERVO_DEBUG_OUTPUT.println(w); }

#define ISR_SERVO_LOGDEBUG(x)         if(ISR_SERVO_DEBUG>1) { ISR_SERVO_DEBUG_OUTPUT.print("[ISR_SERVO] "); ISR_SERVO_DEBUG_OUTPUT.println(x); }
#define ISR_SERVO_LOGDEBUG0(x)        if(ISR_SERVO_DEBUG>1) { ISR_SERVO_DEBUG_OUTPUT.print(x); }
#define ISR_SERVO_LOGDEBUG1(x,y)      if(ISR_SERVO_DEBUG>1) { ISR_SERVO_DEBUG_OUTPUT.print("[ISR_SERVO] "); ISR_SERVO_DEBUG_OUTPUT.print(x); ISR_SERVO_DEBUG_OUTPUT.print(" "); ISR_SERVO_DEBUG_OUTPUT.println(y); }
#define ISR_SERVO_LOGDEBUG2(x,y,z)    if(ISR_SERVO_DEBUG>1) { ISR_SERVO_DEBUG_OUTPUT.print("[ISR_SERVO] "); ISR_SERVO_DEBUG_OUTPUT.print(x); ISR_SERVO_DEBUG_OUTPUT.print(" "); ISR_SERVO_DEBUG_OUTPUT.print(y); ISR_SERVO_DEBUG_OUTPUT.print(" "); ISR_SERVO_DEBUG_OUTPUT.println(z); }
#define ISR_SERVO_LOGDEBUG3(x,y,z,w)  if(ISR_SERVO_DEBUG>1) { ISR_SERVO_DEBUG_OUTPUT.print("[ISR_SERVO] "); ISR_SERVO_DEBUG_OUTPUT.print(x); ISR_SERVO_DEBUG_OUTPUT.print(" "); ISR_SERVO_DEBUG_OUTPUT.print(y); ISR_SERVO_DEBUG_OUTPUT.print(" "); ISR_SERVO_DEBUG_OUTPUT.print(z); ISR_SERVO_DEBUG_OUTPUT.print(" "); ISR_SERVO_DEBUG_OUTPUT.println(w); }
//////////////////////////////////////////


#include <esp32-hal-timer.h>

/* hw_timer_t defined in harware/espressif/esp32/cores/esp32/esp32-hal-timer.c:
  typedef struct hw_timer_s {
        hw_timer_reg_t * dev;
        uint8_t num;
        uint8_t group;
        uint8_t timer;
        portMUX_TYPE lock;
  } hw_timer_t;

  static hw_timer_t hw_timer[4] = {
        {(hw_timer_reg_t *)(DR_REG_TIMERGROUP0_BASE),0,0,0,portMUX_INITIALIZER_UNLOCKED},
        {(hw_timer_reg_t *)(DR_REG_TIMERGROUP0_BASE + 0x0024),1,0,1,portMUX_INITIALIZER_UNLOCKED},
        {(hw_timer_reg_t *)(DR_REG_TIMERGROUP0_BASE + 0x1000),2,1,0,portMUX_INITIALIZER_UNLOCKED},
        {(hw_timer_reg_t *)(DR_REG_TIMERGROUP0_BASE + 0x1024),3,1,1,portMUX_INITIALIZER_UNLOCKED}
  };

  typedef void (*voidFuncPtr)(void);
  static voidFuncPtr __timerInterruptHandlers[4] = {0,0,0,0};

  void IRAM_ATTR __timerISR(void * arg){
    uint32_t s0 = TIMERG0.int_st_timers.val;
    uint32_t s1 = TIMERG1.int_st_timers.val;
    TIMERG0.int_clr_timers.val = s0;
    TIMERG1.int_clr_timers.val = s1;
    uint8_t status = (s1 & 3) << 2 | (s0 & 3);
    uint8_t i = 4;
    //restart the timers that should autoreload
    while(i--){
        hw_timer_reg_t * dev = hw_timer[i].dev;
        if((status & (1 << i)) && dev->config.autoreload){
            dev->config.alarm_en = 1;
        }
    }
    i = 4;
    //call callbacks
    while(i--){
        if(__timerInterruptHandlers[i] && (status & (1 << i))){
            __timerInterruptHandlers[i]();
        }
    }
  }

*/

#ifndef USE_ESP32_TIMER_NO
  #define USE_ESP32_TIMER_NO          3
#endif


class ESP32FastTimerInterrupt;

typedef ESP32FastTimerInterrupt ESP32FastTimer;

#define MAX_ESP32_NUM_TIMERS      4

typedef void (*timer_callback)  (void);


class ESP32FastTimerInterrupt
{
  private:
    hw_timer_t*     _timer;
    uint8_t         _timerNo;

    timer_callback  _callback;        // pointer to the callback function
    float           _frequency;       // Timer frequency
    uint64_t        _timerCount;      // count to activate timer

  public:

    ESP32FastTimerInterrupt()
    {
      _timer    = NULL;
      // Default timer 3
      _timerNo  = USE_ESP32_TIMER_NO;      //MAX_ESP32_NUM_TIMERS;

      _frequency  = 0;
      _timerCount = 0;
      _callback   = NULL;
    };

    ESP32FastTimerInterrupt(uint8_t timerNo)
    {
      _timer    = NULL;

      if (timerNo < MAX_ESP32_NUM_TIMERS)
        _timerNo  = timerNo;
      else
        _timerNo  = USE_ESP32_TIMER_NO;      //MAX_ESP32_NUM_TIMERS

      _frequency  = 0;
      _timerCount = 0;
      _callback = NULL;
    };

    // frequency (in hertz)
    // No params and duration now. To be added in the future by adding similar functions here or to esp32-hal-timer.c
    bool setFrequency(float frequency, timer_callback callback)
    {
      // select timer frequency is 1MHz => 1us, _timerCount = 10 for 10us pulse
      // Will use later if very low frequency is needed.
      _frequency  = 1000000;
      _timerCount = (uint64_t) _frequency / frequency;
      // count up

//#if (TIMER_INTERRUPT_DEBUG > 0)
      //Serial.println("ESP32TimerInterrupt: _timerNo = " + String(_timerNo) + ", _fre = " + String(_frequency)
      //               + ", _count = " + String((uint32_t) (_timerCount >> 32) ) + " - " + String((uint32_t) (_timerCount)));
      ISR_SERVO_LOGERROR3("ESP32FastTimerInterrupt: _timerNo =", _timerNo, ", _fre =", _frequency);
      ISR_SERVO_LOGERROR3("_count =",(uint32_t) (_timerCount >> 32), " -", (uint32_t) (_timerCount));               
//#endif

      // Clock to timer (prescaler) is F_CPU / 3 = 240MHz / 3 = 80MHz
      _timer = timerBegin(_timerNo, F_CPU / (_frequency * 3), true);
      // Interrupt on EGDE
      timerAttachInterrupt(_timer, callback, true);

      // autoreload = true to run forever
      timerAlarmWrite(_timer, _timerCount, true);

      timerAlarmEnable(_timer);

      _callback   = callback;

      return true;
    }

    // interval (in microseconds) and duration (in milliseconds). Duration = 0 or not specified => run indefinitely
    // No params and duration now. To be addes in the future by adding similar functions here or to esp32-hal-timer.c
    bool attachInterruptInterval(unsigned long interval, timer_callback callback)
    {
      return setFrequency( (float) ( 1000000.0f / interval), callback);
    }

    void detachInterrupt()
    {
      timerDetachInterrupt(_timer);
    }

    // Duration (in milliseconds). Duration = 0 or not specified => run indefinitely
    void reattachInterrupt()
    {
      if ( (_frequency != 0) && (_timerCount != 0) && (_callback != NULL) )
        setFrequency(_frequency, _callback);
    }
}; // class ESP32FastTimerInterrupt


#endif      //#ifndef ESP32FastTimerInterrupt_h
