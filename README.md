[![Build Status](https://travis-ci.org/mattheuslee/KittyInterpreter.svg?branch=master)](https://travis-ci.org/mattheuslee/KittyInterpreter)
[![Codacy grade](https://img.shields.io/codacy/grade/f23a880e869647f89641f2d6a9e4a9ef.svg)](https://www.codacy.com/app/mattheus.lee/KittyInterpreter?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=mattheuslee/KittyInterpreter&amp;utm_campaign=Badge_Grade)
[![Coverage Status](https://coveralls.io/repos/github/mattheuslee/KittyInterpreter/badge.svg?branch=master)](https://coveralls.io/github/mattheuslee/KittyInterpreter?branch=master)
[![GitHub license](https://img.shields.io/github/license/mattheuslee/KittyInterpreter.svg)](https://github.com/mattheuslee/KittyInterpreter/blob/master/LICENSE)
[![GitHub issues](https://img.shields.io/github/issues/mattheuslee/KittyInterpreter.svg)](https://github.com/mattheuslee/KittyInterpreter/issues)
[![GitHub top language](https://img.shields.io/github/languages/top/mattheuslee/KittyInterpreter.svg)](https://github.com/mattheuslee/KittyInterpreter)

[![GitHub (pre-)release](https://img.shields.io/github/release/mattheuslee/KittyInterpreter/all.svg)](https://github.com/mattheuslee/KittyInterpreter/releases)
<!--[![GitHub release](https://img.shields.io/github/release/mattheuslee/KittyInterpreter.svg)](https://github.com/mattheuslee/KittyInterpreter)-->
[![GitHub code size in bytes](https://img.shields.io/github/languages/code-size/mattheuslee/KittyInterpreter.svg)](https://github.com/mattheuslee/KittyInterpreter)
[![GitHub repo size in bytes](https://img.shields.io/github/repo-size/mattheuslee/KittyInterpreter.svg)](https://github.com/mattheuslee/KittyInterpreter)

[![Maintenance](https://img.shields.io/maintenance/yes/2018.svg)](https://github.com/mattheuslee/KittyInterpreter)
[![GitHub last commit](https://img.shields.io/github/last-commit/mattheuslee/KittyInterpreter.svg)](https://github.com/mattheuslee/KittyInterpreter)
<!--[![GitHub commits](https://img.shields.io/github/commits-since/mattheuslee/KittyInterpreter/0.1.0.svg)](https://github.com/mattheuslee/KittyInterpreter)
[![Github commits (since latest release)](https://img.shields.io/github/commits-since/mattheuslee/KittyInterpreter/latest.svg)](https://github.com/mattheuslee/KittyInterpreter)-->


Kitty
==========================
A high-level scripting language that runs entirely on an Arduino Mega.

Install and Requirements:
-------------------------
Copy this entire repository folder to your `Arduino/libraries` folder.  
Arduino IDE versions tested:  
* 1.8.2
* 1.8.5

Arduino models tested:
* Mega2560

Kitty will only work on an Arduino Mega as it requires a high amount of sketch and flash memory.  

Additional Arduino libraries required:  
* [ArduinoUnit](https://github.com/mmurdoch/arduinounit) for unit tests
* [ArduinoLog](https://github.com/thijse/Arduino-Log) for logging

## Running the live interpreter:
Open `live_interpreter/live_interpreter.ino` using the Arduino IDE.  
Upload the sketch to the Arduino Mega.  
Use the Arduino IDE serial monitor to enter commands into the interpreter.  
The baud rate should be set as `115200 baud`, and newline character as `Newline` in the corner of the serial monitor.  

Learning the Language:  
----------------------
See the language guide [here](https://github.com/mattheuslee/KittyInterpreter/blob/master/KittyLanguageGuide.md) for a guide to the language syntax and features.

Language Specification:
-----------------------
For a more in-depth look at the language specifications, see [here](TODO: INSERT LINK).

Library Documentation:
----------------------
See the library documentation on [github pages](https://mattheuslee.github.io/KittyInterpreter/).

Creator and Maintainer:
-----------------------
Mattheus Lee [email me](mailto:mattheuslee@gmail.com)
