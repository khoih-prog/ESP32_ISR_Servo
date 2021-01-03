/****************************************************************************************************************************
   ESP32_BlynkServoControl.ino
   For ESP32 boards
   Written by Khoi Hoang

   Built by Khoi Hoang https://github.com/khoih-prog/ESP32_ISR_Servo
   Licensed under MIT license

   The ESP32 has two timer groups, each one with two general purpose hardware timers. All the timers
   are based on 64 bits counters and 16 bit prescalers
   The timer counters can be configured to count up or down and support automatic reload and software reload
   They can also generate alarms when they reach a specific value, defined by the software.
   The value of the counter can be read by the software program.

   Now these new 16 ISR-based PWM servo contro uses only 1 hardware timer.
   The accuracy is nearly perfect compared to software timers. The most important feature is they're ISR-based timers
   Therefore, their executions are not blocked by bad-behaving functions / tasks.
   This important feature is absolutely necessary for mission-critical tasks.

   Notes:
   Special design is necessary to share data between interrupt code and the rest of your program.
   Variables usually need to be "volatile" types. Volatile tells the compiler to avoid optimizations that assume
   variable can not spontaneously change. Because your function may change variables while your program is using them,
   the compiler needs this hint. But volatile alone is often not enough.
   When accessing shared variables, usually interrupts must be disabled. Even with volatile,
   if the interrupt changes a multi-byte variable between a sequence of instructions, it can be read incorrectly.
   If your data is multiple variables, such as an array and a count, usually interrupts need to be disabled
   or the entire sequence of your code which accesses the data.

   Version: 1.1.0

   Version Modified By   Date      Comments
   ------- -----------  ---------- -----------
    1.0.0   K Hoang      12/12/2019 Initial coding
    1.0.1   K Hoang      13/12/2019 Add more features getPosition and getPulseWidth. Optimize.
    1.0.2   K Hoang      20/12/2019 Add more Blynk examples.Change example names to avoid duplication.
    1.1.0   K Hoang      03/01/2021 Fix bug. Add TOC and Version String.
*****************************************************************************************************************************/

/****************************************************************************************************************************
   This example will demonstrate the nearly perfect accuracy compared to software timers by printing the actual elapsed millisecs.
   Being ISR-based timers, their executions are not blocked by bad-behaving functions / tasks, such as connecting to WiFi, Internet
   and Blynk services. You can also have many (up to 16) timers to use.
   This non-being-blocked important feature is absolutely necessary for mission-critical tasks.
   You'll see blynkTimer is blocked while connecting to WiFi / Internet / Blynk, and elapsed time is very unaccurate
   In this super simple example, you don't see much different after Blynk is connected, because of no competing task is
   written

   From ESP32 Servo Example Using Arduino ESP32 Servo Library
   John K. Bennett
   March, 2017

   Different servos require different pulse widths to vary servo angle, but the range is
   an approximately 500-2500 microsecond pulse every 20ms (50Hz). In general, hobbyist servos
   sweep 180 degrees, so the lowest number in the published range for a particular servo
   represents an angle of 0 degrees, the middle of the range represents 90 degrees, and the top
   of the range represents 180 degrees. So for example, if the range is 1000us to 2000us,
   1000us would equal an angle of 0, 1500us would equal 90 degrees, and 2000us would equal 1800
   degrees.

   Circuit:
   Servo motors have three wires: power, ground, and signal. The power wire is typically red,
   the ground wire is typically black or brown, and the signal wire is typically yellow,
   orange or white. Since the ESP32 can supply limited current at only 3.3V, and servos draw
   considerable power, we will connect servo power to the VBat pin of the ESP32 (located
   near the USB connector). THIS IS ONLY APPROPRIATE FOR SMALL SERVOS.

   We could also connect servo power to a separate external
   power source (as long as we connect all of the grounds (ESP32, servo, and external power).
   In this example, we just connect ESP32 ground to servo ground. The servo signal pins
   connect to any available GPIO pins on the ESP32 (in this example, we use pins
   22, 19, 23, & 18).

   In this example, we assume four Tower Pro SG90 small servos.
   The published min and max for this servo are 500 and 2400, respectively.
   These values actually drive the servos a little past 0 and 180, so
   if you are particular, adjust the min and max values to match your needs.
   Experimentally, 550 and 2350 are pretty close to 0 and 180.
*****************************************************************************************************************************/

#include "defines.h"
#include "Credentials.h"
#include "dynamicParams.h"

//See file .../hardware/espressif/esp32/variants/(esp32|doitESP32devkitV1)/pins_arduino.h
#define LED_BUILTIN       2         // Pin D2 mapped to pin GPIO2/ADC12 of ESP32, control on-board LED
#define PIN_LED           2         // Pin D2 mapped to pin GPIO2/ADC12 of ESP32, control on-board LED

#define PIN_D0            0         // Pin D0 mapped to pin GPIO0/BOOT/ADC11/TOUCH1 of ESP32
#define PIN_D1            1         // Pin D1 mapped to pin GPIO1/TX0 of ESP32
#define PIN_D2            2         // Pin D2 mapped to pin GPIO2/ADC12/TOUCH2 of ESP32
#define PIN_D3            3         // Pin D3 mapped to pin GPIO3/RX0 of ESP32
#define PIN_D4            4         // Pin D4 mapped to pin GPIO4/ADC10/TOUCH0 of ESP32
#define PIN_D5            5         // Pin D5 mapped to pin GPIO5/SPISS/VSPI_SS of ESP32
#define PIN_D6            6         // Pin D6 mapped to pin GPIO6/FLASH_SCK of ESP32
#define PIN_D7            7         // Pin D7 mapped to pin GPIO7/FLASH_D0 of ESP32
#define PIN_D8            8         // Pin D8 mapped to pin GPIO8/FLASH_D1 of ESP32
#define PIN_D9            9         // Pin D9 mapped to pin GPIO9/FLASH_D2 of ESP32

#define PIN_D10           10        // Pin D10 mapped to pin GPIO10/FLASH_D3 of ESP32
#define PIN_D11           11        // Pin D11 mapped to pin GPIO11/FLASH_CMD of ESP32
#define PIN_D12           12        // Pin D12 mapped to pin GPIO12/HSPI_MISO/ADC15/TOUCH5/TDI of ESP32
#define PIN_D13           13        // Pin D13 mapped to pin GPIO13/HSPI_MOSI/ADC14/TOUCH4/TCK of ESP32
#define PIN_D14           14        // Pin D14 mapped to pin GPIO14/HSPI_SCK/ADC16/TOUCH6/TMS of ESP32
#define PIN_D15           15        // Pin D15 mapped to pin GPIO15/HSPI_SS/ADC13/TOUCH3/TDO of ESP32
#define PIN_D16           16        // Pin D16 mapped to pin GPIO16/TX2 of ESP32
#define PIN_D17           17        // Pin D17 mapped to pin GPIO17/RX2 of ESP32     
#define PIN_D18           18        // Pin D18 mapped to pin GPIO18/VSPI_SCK of ESP32
#define PIN_D19           19        // Pin D19 mapped to pin GPIO19/VSPI_MISO of ESP32

#define PIN_D21           21        // Pin D21 mapped to pin GPIO21/SDA of ESP32
#define PIN_D22           22        // Pin D22 mapped to pin GPIO22/SCL of ESP32
#define PIN_D23           23        // Pin D23 mapped to pin GPIO23/VSPI_MOSI of ESP32
#define PIN_D24           24        // Pin D24 mapped to pin GPIO24 of ESP32
#define PIN_D25           25        // Pin D25 mapped to pin GPIO25/ADC18/DAC1 of ESP32
#define PIN_D26           26        // Pin D26 mapped to pin GPIO26/ADC19/DAC2 of ESP32
#define PIN_D27           27        // Pin D27 mapped to pin GPIO27/ADC17/TOUCH7 of ESP32     

#define PIN_D32           32        // Pin D32 mapped to pin GPIO32/ADC4/TOUCH9 of ESP32
#define PIN_D33           33        // Pin D33 mapped to pin GPIO33/ADC5/TOUCH8 of ESP32
#define PIN_D34           34        // Pin D34 mapped to pin GPIO34/ADC6 of ESP32
#define PIN_D35           35        // Pin D35 mapped to pin GPIO35/ADC7 of ESP32
#define PIN_D36           36        // Pin D36 mapped to pin GPIO36/ADC0/SVP of ESP32
#define PIN_D39           39        // Pin D39 mapped to pin GPIO39/ADC3/SVN of ESP32

#define PIN_RX0            3        // Pin RX0 mapped to pin GPIO3/RX0 of ESP32
#define PIN_TX0            1        // Pin TX0 mapped to pin GPIO1/TX0 of ESP32

#define PIN_SCL           22        // Pin SCL mapped to pin GPIO22/SCL of ESP32
#define PIN_SDA           21        // Pin SDA mapped to pin GPIO21/SDA of ESP32  

#define TIMER_INTERRUPT_DEBUG       1
#define ISR_SERVO_DEBUG             1

// Select different ESP32 timer number (0-3) to avoid conflict
#define USE_ESP32_TIMER_NO          3

#include "ESP32_ISR_Servo.h"

// MG996R servo has a running current of  500mA to 900mA @6V and a stall current of 2.5A @ 6V
// Power supply must be adequate
// Published values for SG90 servos; adjust if needed
#define MIN_MICROS      800  //544
#define MAX_MICROS      2450

int servoIndex1  = -1;
int servoIndex2  = -1;
int servoIndex3  = -1;

int servo1Pin = PIN_D25; //SERVO1 PIN
int servo2Pin = PIN_D26; //SERVO2 PIN
int servo3Pin = PIN_D27; //SERVO3 PIN

BlynkTimer timer;

// These are Blynk Slider or any Widget (STEP, Numeric Input, being able to output (unsigned) int value from 0-180.
// You have to map from 0-180 inside widget or in your code. Otherwise, the library will remap the input for you.
#define BLYNK_VPIN_SERVO1_CONTROL       V21
#define BLYNK_VPIN_SERVO2_CONTROL       V22
#define BLYNK_VPIN_SERVO3_CONTROL       V23

//READING FROM VIRTUAL PINS
// SERVO1
BLYNK_WRITE(BLYNK_VPIN_SERVO1_CONTROL)
{
  ESP32_ISR_Servos.setPosition(servoIndex1, param.asInt());
}

//SERVO2
BLYNK_WRITE(BLYNK_VPIN_SERVO2_CONTROL)
{
  ESP32_ISR_Servos.setPosition(servoIndex2, param.asInt());
}

//SERVO3
BLYNK_WRITE(BLYNK_VPIN_SERVO3_CONTROL)
{
  ESP32_ISR_Servos.setPosition(servoIndex3, param.asInt());
}

void heartBeatPrint(void)
{
  static int num = 1;

  if (WiFi.status() == WL_CONNECTED)
  {
    if (Blynk.connected())
      Serial.print(F("B"));        // B means connected to Blynk
    else
      Serial.print(F("H"));        // H means connected to WiFi but no Blynk
  }
  else
    Serial.print(F("F"));          // F means not connected to WiFi and Blynk

  if (num == 80)
  {
    Serial.println();
    num = 1;
  }
  else if (num++ % 10 == 0)
  {
    Serial.print(F(" "));
  }
}

void setup()
{
  // Debug console
  Serial.begin(115200);
  while (!Serial);

  delay(200);

#if (USE_LITTLEFS)
  Serial.print(F("\nStarting ESP32_BlynkServoControl using LITTLEFS"));
#elif (USE_SPIFFS)
  Serial.print(F("\nStarting ESP32_BlynkServoControl using SPIFFS"));  
#else
  Serial.print(F("\nStarting ESP32_BlynkServoControl using EEPROM"));
#endif

#if USE_SSL
  Serial.print(F(" with SSL on ")); Serial.println(ARDUINO_BOARD);
#else
  Serial.print(F(" without SSL on ")); Serial.println(ARDUINO_BOARD);
#endif

  Serial.println(BLYNK_WM_VERSION);
  Serial.println(ESP_DOUBLE_RESET_DETECTOR_VERSION);
  Serial.println(ESP32_ISR_SERVO_VERSION);
  
  Blynk.begin(HOST_NAME);

  //Select ESP32 timer USE_ESP32_TIMER_NO
  ESP32_ISR_Servos.useTimer(USE_ESP32_TIMER_NO);

  servoIndex1 = ESP32_ISR_Servos.setupServo(servo1Pin, MIN_MICROS, MAX_MICROS);
  servoIndex2 = ESP32_ISR_Servos.setupServo(servo2Pin, MIN_MICROS, MAX_MICROS);
  servoIndex3 = ESP32_ISR_Servos.setupServo(servo3Pin, MIN_MICROS, MAX_MICROS);

  if (servoIndex1 != -1)
    Serial.println(F("Setup Servo1 OK"));
  else
    Serial.println(F("Setup Servo1 failed"));

  if (servoIndex2 != -1)
    Serial.println(F("Setup Servo2 OK"));
  else
    Serial.println(F("Setup Servo2 failed"));

  if (servoIndex3 != -1)
    Serial.println(F("Setup Servo3 OK"));
  else
    Serial.println(F("Setup Servo3 failed"));

  timer.setInterval(30000L, heartBeatPrint);
}

void loop()
{
  Blynk.run();
  timer.run();
}
