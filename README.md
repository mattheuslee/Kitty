# KittyInterpreter
An embedded interpreter for a high-level scripting language for Arduino

## Install and Requirements:
Copy this entire repository folder to your `Arduino/libraries` folder.  
This repository has been tested on Arduino 1.8.2.

Additional Arduino libraries required:
* [StandardCplusplus](https://github.com/maniacbug/StandardCplusplus)
* [NewPing](https://github.com/PaulStoffregen/NewPing)
* [Regexp](https://github.com/nickgammon/Regexp)

## Running the live interpreter:
Open `kitty/live_interpreter/live_interpreter.ino` using the Arduino IDE  
Upload the sketch to an Arduino board. The Arduino Mega is recommended - for larger programs an Arduino Uno may not contain sufficient program or memory space.
Use the Arduino IDE serial monitor to enter commands into the interpreter. The baud rate should be set as `115200 baud`, and newline character as `Newline` in the corner of the serial monitor.

## Help:
See [`KittyLanguageGuide.md`](https://github.com/mattheuslee/KittyInterpreter/blob/master/KittyLanguageGuide.md) for help with the language syntax.

## Creator and Maintainer:
Mattheus Lee [email me](mailto:mattheuslee@gmail.com)
