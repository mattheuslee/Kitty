[![Build Status](https://travis-ci.org/mattheuslee/KittyInterpreter.svg?branch=master)](https://travis-ci.org/mattheuslee/KittyInterpreter)
[![Codacy Badge](https://api.codacy.com/project/badge/Grade/f23a880e869647f89641f2d6a9e4a9ef)](https://www.codacy.com/app/mattheus.lee/KittyInterpreter?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=mattheuslee/KittyInterpreter&amp;utm_campaign=Badge_Grade)
# Kitty (v0.2.0 in progress)
A high-level scripting language that runs entirely on an Arduino Mega.

## Install and Requirements:
Copy this entire repository folder to your `Arduino/libraries` folder.  
This library has been tested on Arduino IDE 1.8.2.  
Kitty will only work on an Arduino Mega as it requires a high amount of sketch and flash memory.  

Additional Arduino libraries required:  
* [ArduinoSTL](https://github.com/mike-matera/ArduinoSTL)

## Running the live interpreter:
Open `live_interpreter/live_interpreter.ino` using the Arduino IDE.  
Upload the sketch to the Arduino Mega.  
Use the Arduino IDE serial monitor to enter commands into the interpreter.  
The baud rate should be set as `115200 baud`, and newline character as `Newline` in the corner of the serial monitor.  

## Learning the language:  
See the language guide on [github pages](https://mattheuslee.github.io/KittyInterpreter/) for a guide to the language syntax and features.

## Releases:
v0.1.0 - Initial release (26 August 2017)

## Creator and Maintainer:
Mattheus Lee [email me](mailto:mattheuslee@gmail.com)
