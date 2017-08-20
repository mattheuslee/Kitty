[![Codacy Badge](https://api.codacy.com/project/badge/Grade/f23a880e869647f89641f2d6a9e4a9ef)](https://www.codacy.com/app/mattheus.lee/KittyInterpreter?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=mattheuslee/KittyInterpreter&amp;utm_campaign=Badge_Grade)
# KittyInterpreter
An embedded interpreter for a high-level scripting language for Arduino

## Install and Requirements:
Copy this entire repository folder to your `Arduino/libraries` folder.  
This repository has been tested on Arduino IDE 1.8.2.  
The sketches in this repository will only work on an Arduino Mega - The amount of sketch and flash memory on the Arduino Uno is not enough for this library.

Additional Arduino libraries required:
* [StandardCplusplus](https://github.com/maniacbug/StandardCplusplus)
* [NewPing](https://github.com/PaulStoffregen/NewPing)
* [Regexp](https://github.com/nickgammon/Regexp)  

These libraries can be found in the `additional_required_libraries` folder.  
In order to install them just copy the contents of the `additional_required_libraries` folder to your `Arduino/libraries` folder alongside `kitty`.

## Running the live interpreter:
Open `kitty/live_interpreter/live_interpreter.ino` using the Arduino IDE.  
Upload the sketch to the Arduino Mega.  
Use the Arduino IDE serial monitor to enter commands into the interpreter.  
The baud rate should be set as `115200 baud`, and newline character as `Newline` in the corner of the serial monitor.

## Running the preloaded interpreter:
Open `kitty/preloaded_interpreter/preloaded_interpreter.ino` using the Arduino IDE.  
Ensure that the variable `commands` in the `setup` loop contains all the commands that you want to execute, in the correct order.  
Upload the sketch to the Arduino Mega and it'll automatically begin running the preloaded commands.

## Learning the language:
See [`KittyLanguageGuide.md`](https://github.com/mattheuslee/KittyInterpreter/blob/master/KittyLanguageGuide.md) for a guide to the language syntax.

## Creator and Maintainer:
Mattheus Lee [email me](mailto:mattheuslee@gmail.com)
