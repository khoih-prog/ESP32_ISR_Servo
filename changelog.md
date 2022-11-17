# ESP32_ISR_Servo Library

[![arduino-library-badge](https://www.ardu-badge.com/badge/ESP32_ISR_Servo.svg?)](https://www.ardu-badge.com/ESP32_ISR_Servo)
[![GitHub release](https://img.shields.io/github/release/khoih-prog/ESP32_ISR_Servo.svg)](https://github.com/khoih-prog/ESP32_ISR_Servo/releases)
[![GitHub](https://img.shields.io/github/license/mashape/apistatus.svg)](https://github.com/khoih-prog/ESP32_ISR_Servo/blob/master/LICENSE)
[![contributions welcome](https://img.shields.io/badge/contributions-welcome-brightgreen.svg?style=flat)](#Contributing)
[![GitHub issues](https://img.shields.io/github/issues/khoih-prog/ESP32_ISR_Servo.svg)](http://github.com/khoih-prog/ESP32_ISR_Servo/issues)


<a href="https://www.buymeacoffee.com/khoihprog6" title="Donate to my libraries using BuyMeACoffee"><img src="https://cdn.buymeacoffee.com/buttons/v2/default-yellow.png" alt="Donate to my libraries using BuyMeACoffee" style="height: 50px !important;width: 181px !important;" ></a>
<a href="https://www.buymeacoffee.com/khoihprog6" title="Donate to my libraries using BuyMeACoffee"><img src="https://img.shields.io/badge/buy%20me%20a%20coffee-donate-orange.svg?logo=buy-me-a-coffee&logoColor=FFDD00" style="height: 20px !important;width: 200px !important;" ></a>
<a href="https://profile-counter.glitch.me/khoih-prog/count.svg" title="Total khoih-prog Visitor count"><img src="https://profile-counter.glitch.me/khoih-prog/count.svg" style="height: 30px;width: 200px;"></a>
<a href="https://profile-counter.glitch.me/khoih-prog-ESP32_ISR_Servo/count.svg" title="ESP32_ISR_Servo Visitor count"><img src="https://profile-counter.glitch.me/khoih-prog-ESP32_ISR_Servo/count.svg" style="height: 30px;width: 200px;"></a>

---
---

## Table of Contents

* [Changelog](#changelog)
  * [Releases v1.5.0](#releases-v150)
  * [Releases v1.4.0](#releases-v140)
  * [Releases v1.3.0](#releases-v130)
  * [Releases v1.2.1](#releases-v121)
  * [Releases v1.2.0](#releases-v120)
  * [Releases v1.1.0](#releases-v110)
  * [Releases v1.0.2](#releases-v102)
  * [Releases v1.0.1](#releases-v101)
 
---
---

## Changelog

### Releases v1.5.0

1. Fix doubled time for `ESP32_C3, ESP32_S2 and ESP32_S3`. Check [Error in the value defined by TIMER0_INTERVAL_MS #28](https://github.com/khoih-prog/ESP32_ISR_Servo/issues/28)
2. Modify examples to avoid using `LED_BUILTIN` / `GPIO2` and `GPIO2` as they can cause crash in some boards, such as `ESP32_C3`
3. Use `allman astyle` and add `utils`

### Releases v1.4.0

1. Suppress errors and warnings for new ESP32 core v2.0.4+

### Releases v1.3.1

1. Add support to new Adafruit boards such as QTPY_ESP32S2, FEATHER_ESP32S3_NOPSRAM and QTPY_ESP32S3_NOPSRAM

### Releases v1.3.0

1. Fix breaking issue caused by **ESP32 core v2.0.1+** by increasing `TIMER_INTERVAL_MICRO` to `12uS` from `10uS`. Tested OK with ESP32 core v2.0.3 now

### Releases v1.2.1

1. Fix bug

### Releases v1.2.0

1. Add support to new `ESP32-S3` (ESP32S3_DEV, ESP32_S3_BOX, UM TINYS3, UM PROS3, UM FEATHERS3, etc.)
2. Add support to new `ESP32-S2` (ESP32S2_DEV, etc.)
3. Add support to new `ESP32-C3` (ESP32C3_DEV, etc.)
4. Convert to h-only library.
5. Optimize library code by using `reference-passing` instead of `value-passing`
6. Improve accuracy by using `float`, instead of `uint32_t` for `position` in degrees
7. Add example [multiFileProject](examples/multiFileProject) to demo for multiple-file project

### Releases v1.1.0

1. Fix bug. See [Fixed count >= min comparison for servo enable](https://github.com/khoih-prog/ESP32_ISR_Servo/pull/1)
2. Clean-up all compiler warnings possible.
3. Add Table of Contents
4. Add Version String
5. Fix and Optimize old examples

#### Releases v1.0.2

1. Add example using [Blynk](http://docs.blynk.cc/) to control servos. 
2. Change example names to avoid duplication.

#### Releases v1.0.1

1. Basic 16 ISR-based servo controllers using 1 hardware timer for ESP32.


