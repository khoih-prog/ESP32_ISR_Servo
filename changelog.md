# ESP32_ISR_Servo Library

[![arduino-library-badge](https://www.ardu-badge.com/badge/ESP32_ISR_Servo.svg?)](https://www.ardu-badge.com/ESP32_ISR_Servo)
[![GitHub release](https://img.shields.io/github/release/khoih-prog/ESP32_ISR_Servo.svg)](https://github.com/khoih-prog/ESP32_ISR_Servo/releases)
[![GitHub](https://img.shields.io/github/license/mashape/apistatus.svg)](https://github.com/khoih-prog/ESP32_ISR_Servo/blob/master/LICENSE)
[![contributions welcome](https://img.shields.io/badge/contributions-welcome-brightgreen.svg?style=flat)](#Contributing)
[![GitHub issues](https://img.shields.io/github/issues/khoih-prog/ESP32_ISR_Servo.svg)](http://github.com/khoih-prog/ESP32_ISR_Servo/issues)

---
---

## Table of Contents

* [Changelog](#changelog)
  * [Releases v1.2.1](#releases-v121)
  * [Releases v1.2.0](#releases-v120)
  * [Releases v1.1.0](#releases-v110)
  * [Releases v1.0.2](#releases-v102)
  * [Releases v1.0.1](#releases-v101)
 
---
---

## Changelog

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


