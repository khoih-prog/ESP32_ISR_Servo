# ESP32_ISR_Servo Library

[![arduino-library-badge](https://www.ardu-badge.com/badge/ESP32_ISR_Servo.svg?)](https://www.ardu-badge.com/ESP32_ISR_Servo)
[![GitHub release](https://img.shields.io/github/release/khoih-prog/ESP32_ISR_Servo.svg)](https://github.com/khoih-prog/ESP32_ISR_Servo/releases)
[![GitHub](https://img.shields.io/github/license/mashape/apistatus.svg)](https://github.com/khoih-prog/ESP32_ISR_Servo/blob/master/LICENSE)
[![contributions welcome](https://img.shields.io/badge/contributions-welcome-brightgreen.svg?style=flat)](#Contributing)
[![GitHub issues](https://img.shields.io/github/issues/khoih-prog/ESP32_ISR_Servo.svg)](http://github.com/khoih-prog/ESP32_ISR_Servo/issues)

---
---

#### Version v1.0.2

1. Add example using [Blynk](http://docs.blynk.cc/) to control servos. 
2. Change example names to avoid duplication.

#### Version v1.0.1

1. Basic 16 ISR-based servo controllers using 1 hardware timer for ESP32.

---

This library enables you to use `1 Hardware Timer` on an ESP32-based board to control up to `16 independent servo motors`.

#### Why do we need this ISR-based Servo controller?

Imagine you have a system with a **mission-critical function** controlling a **robot arm** or doing something much more important. You normally use a software timer to poll, or even place the function in loop(). But what if another function is blocking the loop() or setup().

So your function might not be executed, and the result would be disastrous.

You'd prefer to have your function called, no matter what happening with other functions (busy loop, bug, etc.).

The correct choice is to use a **Hardware Timer with Interrupt** to call your function.

These hardware timers, using interrupt, still work even if other functions are blocking. Moreover, they are **much more precise** (certainly depending on clock frequency accuracy) than other software timers using millis() or micros(). That's necessary if you need to measure some data requiring better accuracy.

Functions using normal software timers, relying on loop() and calling millis(), won't work if the **loop() or setup() is blocked by certain operation**. For example, certain function is blocking while it's connecting to WiFi or some services.

The catch is your function is now part of an **ISR (Interrupt Service Routine)**, and must be `lean / mean`, and follow certain rules. More to read on:

[HOWTO Attach Interrupt](https://www.arduino.cc/reference/en/language/functions/external-interrupts/attachinterrupt/)

---

### Important Notes:

1. Inside the attached function, delay() won’t work and the value returned by millis() will not increment. Serial data received while in the function may be lost. You should declare as volatile any variables that you modify within the attached function.

2. Typically global variables are used to pass data between an ISR and the main program. To make sure variables shared between an ISR and the main program are updated correctly, declare them as volatile.

---
---

## Prerequisite

1. [`Arduino IDE 1.8.13+`](https://www.arduino.cc/en/Main/Software)
2. [`ESP32 core 1.0.4+`](https://github.com/espressif/arduino-esp32/releases) for ESP32 boards

---

## Installation

### Use Arduino Library Manager

The best and easiest way is to use `Arduino Library Manager`. Search for `ESP32_ISR_Servo`, then select / install the latest version.
You can also use this link [![arduino-library-badge](https://www.ardu-badge.com/badge/ESP32_ISR_Servo.svg?)](https://www.ardu-badge.com/ESP32_ISR_Servo) for more detailed instructions.

### Manual Install

Another way to install is to:

1. Navigate to [ESP32_ISR_Servo](https://github.com/khoih-prog/ESP32_ISR_Servo) page.
2. Download the latest release `ESP32_ISR_Servo-master.zip`.
3. Extract the zip file to `ESP32_ISR_Servo-master` directory 
4. Copy whole `ESP32_ISR_Servo-master` folder to Arduino libraries' directory such as `~/Arduino/libraries/`.

### VS Code & PlatformIO:

1. Install [VS Code](https://code.visualstudio.com/)
2. Install [PlatformIO](https://platformio.org/platformio-ide)
3. Install **ESP32_ISR_Servo** library by using [Library Manager](https://docs.platformio.org/en/latest/librarymanager/). Search for ESP32_ISR_Servo in [Platform.io Author's Libraries](https://platformio.org/lib/search?query=author:%22Khoi%20Hoang%22)
4. Use included [platformio.ini](platformio/platformio.ini) file from examples to ensure that all dependent libraries will installed automatically. Please visit documentation for the other options and examples at [Project Configuration File](https://docs.platformio.org/page/projectconf.html)

---
---

### HOWTO Use analogRead() with ESP32 running WiFi and/or BlueTooth (BT/BLE)

Please have a look at [**ESP_WiFiManager Issue 39: Not able to read analog port when using the autoconnect example**](https://github.com/khoih-prog/ESP_WiFiManager/issues/39) to have more detailed description and solution of the issue.

#### 1.  ESP32 has 2 ADCs, named ADC1 and ADC2

#### 2. ESP32 ADCs functions

- ADC1 controls ADC function for pins **GPIO32-GPIO39**
- ADC2 controls ADC function for pins **GPIO0, 2, 4, 12-15, 25-27**

#### 3.. ESP32 WiFi uses ADC2 for WiFi functions

Look in file [**adc_common.c**](https://github.com/espressif/esp-idf/blob/master/components/driver/adc_common.c#L61)

> In ADC2, there're two locks used for different cases:
> 1. lock shared with app and Wi-Fi:
>    ESP32:
>         When Wi-Fi using the ADC2, we assume it will never stop, so app checks the lock and returns immediately if failed.
>    ESP32S2:
>         The controller's control over the ADC is determined by the arbiter. There is no need to control by lock.
> 
> 2. lock shared between tasks:
>    when several tasks sharing the ADC2, we want to guarantee
>    all the requests will be handled.
>    Since conversions are short (about 31us), app returns the lock very soon,
>    we use a spinlock to stand there waiting to do conversions one by one.
> 
> adc2_spinlock should be acquired first, then adc2_wifi_lock or rtc_spinlock.


- In order to use ADC2 for other functions, we have to **acquire complicated firmware locks and very difficult to do**
- So, it's not advisable to use ADC2 with WiFi/BlueTooth (BT/BLE).
- Use ADC1, and pins GPIO32-GPIO39
- If somehow it's a must to use those pins serviced by ADC2 (**GPIO0, 2, 4, 12, 13, 14, 15, 25, 26 and 27**), use the **fix mentioned at the end** of [**ESP_WiFiManager Issue 39: Not able to read analog port when using the autoconnect example**](https://github.com/khoih-prog/ESP_WiFiManager/issues/39) to work with ESP32 WiFi/BlueTooth (BT/BLE).

---
---

## More useful Information

  - **The ESP32 has two timer groups, each one with two general purpose hardware timers.**
  - All the timers are based on **64-bit counters and 16-bit prescalers.**
  - The timer counters can be configured to count up or down and support automatic reload and software reload.
  - They can also generate alarms when they reach a specific value, defined by the software. 
  - The value of the counter can be read by the software program.
  
---

## New from v1.0.1

1. Add functions `getPosition()` and `getPulseWidth()`
2. Optimize the code
3. Add more complicated example

---

Now these new **16 ISR-based Servo controllers** just use one ESP32 Hardware Timer. The number 16 is just arbitrarily chosen, and depending on application, you can increase that number to 32, 48, etc. without problem.

The accuracy is nearly perfect compared to software timers. The most important feature is they're ISR-based timers

Therefore, their executions are not blocked by bad-behaving functions / tasks. This important feature is absolutely necessary for mission-critical tasks. 

The [**MultipleServos**](examples/MultipleServos) example, which controls 6 servos independently, will demonstrate the nearly perfect accuracy.
Being ISR-based servo controllers, their executions are not blocked by bad-behaving functions / tasks, such as connecting to WiFi, Internet and Blynk services.

This non-being-blocked important feature is absolutely necessary for mission-critical tasks.

You'll see blynkTimer Software is blocked while system is connecting to WiFi / Internet / Blynk, as well as by blocking task in loop(), using delay() function as an example. The elapsed time then is very unaccurate

---

## Supported Boards

- ESP32

---

## HOWTO Usage

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

---
---

### Examples: 

 1. [ESP32_BlynkServoControl](examples/ESP32_BlynkServoControl)
 2. [ESP32_ISR_MultiServos](examples/ESP32_ISR_MultiServos)
 3. [ESP32_MultipleRandomServos](examples/ESP32_MultipleRandomServos) 
 4. [ESP32_MultipleServos](examples/ESP32_MultipleServos) 
 5. [ISR_MultiServos](examples/ISR_MultiServos)
 6. [MultipleRandomServos](examples/MultipleRandomServos)
 7. [MultipleServos](examples/MultipleServos)
 
---

### Example [ESP8266_BlynkServoControl](examples/ESP8266_BlynkServoControl)

```cpp
#ifndef ESP32
#error This code is designed to run on ESP32 platform, not Arduino nor ESP8266! Please check your Tools->Board setting.
#endif

#define BLYNK_PRINT Serial

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

#define USE_SPIFFS    true
//#define USE_SPIFFS    false

#define USE_BLYNK_WM    true            // https://github.com/khoih-prog/Blynk_WM
//#define USE_BLYNK_WM    false

//LIBRARIES INCLUDED
#include <WiFi.h>
#include <WiFiClient.h>

#if USE_BLYNK_WM
  #include <BlynkSimpleEsp32_WM.h>                    // https://github.com/khoih-prog/Blynk_WM
#else
  #include <BlynkSimpleEsp32.h>

  //BLYNK AUTHENTICATION TOKEN
  char auth[] = "******";
    
  // WIFI CREDENTIALS
  char ssid[] = "****";
  char pass[] = "****";
  
#endif

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
      Serial.print("B");        // B means connected to Blynk
    else
      Serial.print("H");        // H means connected to WiFi but no Blynk
  }
  else
    Serial.print("F");          // F means not connected to WiFi and Blynk
  
  if (num == 80) 
  {
    Serial.println();
    num = 1;
  }
  else if (num++ % 10 == 0) 
  {
    Serial.print(" ");
  }
}

void setup()
{
  // Debug console
  Serial.begin(115200);
  Serial.println("\nStarting");
  
  #if USE_BLYNK_WM
    Blynk.begin();
  #else
    Blynk.begin(auth, ssid, pass);
  #endif

  //Select ESP32 timer USE_ESP32_TIMER_NO
  ESP32_ISR_Servos.useTimer(USE_ESP32_TIMER_NO);
  
  servoIndex1 = ESP32_ISR_Servos.setupServo(servo1Pin, MIN_MICROS, MAX_MICROS);
  servoIndex2 = ESP32_ISR_Servos.setupServo(servo2Pin, MIN_MICROS, MAX_MICROS);
  servoIndex3 = ESP32_ISR_Servos.setupServo(servo3Pin, MIN_MICROS, MAX_MICROS);
  
  if (servoIndex1 != -1)
    Serial.println("Setup Servo1 OK");
  else
    Serial.println("Setup Servo1 failed");

  if (servoIndex2 != -1)
    Serial.println("Setup Servo2 OK");
  else
    Serial.println("Setup Servo2 failed");

  if (servoIndex3 != -1)
    Serial.println("Setup Servo3 OK");
  else
    Serial.println("Setup Servo3 failed");

  timer.setInterval(30000L, heartBeatPrint);
}

void loop()
{
  Blynk.run();
  timer.run();
}
```

---

## TO DO

1. Search for bug and improvement.


## DONE

1. Similar features for Arduino (UNO, Mega, etc...) and ESP8266
2. Add functions `getPosition()` and `getPulseWidth()`
3. Optimize the code
4. Add more complicated examples

---

#### Version v1.0.2

1. Add example using [Blynk](http://docs.blynk.cc/) to control servos. 
2. Change example names to avoid duplication.

#### Version v1.0.1

1. Basic 16 ISR-based servo controllers using 1 hardware timer for ESP32.

---
---

### Issues

Submit issues to: [ESP32_ISR_Servo issues](https://github.com/khoih-prog/ESP32_ISR_Servo/issues)

---

## Contributing

If you want to contribute to this project:
- Report bugs and errors
- Ask for enhancements
- Create issues and pull requests
- Tell other people about this library

---

### License

- The library is licensed under [MIT](https://github.com/khoih-prog/ESP32_ISR_Servo/blob/master/LICENSE)

---

## Copyright

Copyright 2019- Khoi Hoang
