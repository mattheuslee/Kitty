[![Build Status](https://travis-ci.org/mattheuslee/KittyInterpreter.svg?branch=master)](https://travis-ci.org/mattheuslee/KittyInterpreter)
[![Codacy Badge](https://api.codacy.com/project/badge/Grade/f23a880e869647f89641f2d6a9e4a9ef)](https://www.codacy.com/app/mattheus.lee/KittyInterpreter?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=mattheuslee/KittyInterpreter&amp;utm_campaign=Badge_Grade)
[![Coverage Status](https://coveralls.io/repos/github/mattheuslee/KittyInterpreter/badge.svg?branch=master)](https://coveralls.io/github/mattheuslee/KittyInterpreter?branch=master)  
Kitty (v0.2.0 in progress)
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
* [ArduinoSTL](https://github.com/mike-matera/ArduinoSTL)  
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

Library Documentation:
----------------------
See the library documentation on [github pages](https://mattheuslee.github.io/KittyInterpreter/).

Creator and Maintainer:
-----------------------
Mattheus Lee [email me](mailto:mattheuslee@gmail.com)
