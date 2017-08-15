# Kitty Language Guide
Kitty is a basic high-level scripting language that is suitable for total beginners getting into programming.  
It allows users to easily control Arduino functions through simple commands with immediate results.

## 0. Preface
The command examples given here are prefixed with `>>>`.  
The expected output from the commands are displayed after the commands without a prefix.

## 1. Names
Names in Kitty can only contain the characters `a-z`, `A-Z`, and underscores `_`

## 2. Creating and storing numbers and devices
Creation and storing is done using the `is` keyword.
### 2.1 Creating and storing numbers
To store `42` in `answer`:
> `>>>answer is 42`  

To display information about any stored number or device, type in just it's name:
> `>>>answer`  
> `answer: 42`

To store `-1` in `negative`:
> `>>>negative is -1`  
> `>>>negative`  
> `negative: -1`

Copies of numbers can also be made.  
To make a copy of `answer` from earlier into `other_answer`:
> `>>>other_answer is answer`  
> `>>>other_answer`  
> `other_answer: 42`

### 2.2 Creating and storing devices

#### 2.2.1 Input devices

Sonar input devices require a specified trigger pin and an echo pin.  
To store a sonar named `front_sonar` that uses Arduino pin 11 as the trigger pin and Arduino pin 12 as the echo pin:
> `>>>front_sonar is sonar using 11, 12`  
> `>>>front_sonar`  
> `front_sonar: sonar using trigger pin 11 and echo pin 12`

To store the pin numbers first and then use them later:
> `>>>trigger_pin is 11`  
> `>>>echo_pin is 12`  
> `>>>front_sonar is sonar using trigger_pin, echo_pin`  
> `>>>front_sonar`  
> `front_sonar: sonar using trigger pin 11 and echo pin 12`

#### 2.2.2 Output devices  
LED output devices require a specified power pin.
To store an LED named `light` that uses Arduino pin 10 as the pin:
> `>>>light is led using 10`  
> `>>>light`  
> `light: led using pin 10 at level 50%`

LEDs start out at 50% brightness.  
To store the pin number first and then use it later:
> `>>>led_pin is 10`  
> `>>>light is led using led_pin`  
> `>>>light`  
> `light: led using pin 10 at level 50%`

Servo output devices require a specified control pin.
To store a servo named `control` that uses arduino pin 9 as the control pin:
> `>>>control is servo using 9`  
> `>>>control`  
> `control: servo using pin 9 at value 90 degrees`

Servos start out in the 90 degree position.  
To store the pin number first and then use it later:
> `>>>control_pin is 9`  
> `>>>control is servo using control_pin`  
> `>>>control`  
> `control: servo using pin 9 at value 90 degrees`

## 3. Modifying numbers  
To change a currently stored number, you can simply assign a new number to it.  
To store `32` in `answer`, and later change it to `42`:
> `>>>answer is 32`  
> `>>>answer is 42`  
> `>>>answer`  
> `answer: 42`

To increment `answer` by 10 after initially setting it to `32`:
> `>>>answer is 32`  
> `>>>increment answer by 10`  
> `>>>answer`  
> `answer: 42`

To decrement `answer` by 10 after initially setting it to `52`:
> `>>>answer is 52`  
> `>>>decrement answer by 10`  
> `>>>answer`  
> `answer: 42`

To increment `i` by 1 (default increment):
> `>>>i is 0`  
> `>>>increment i`  
> `>>>i`  
> `i: 1`

To decrement `i` by 1 (default decrement):
> `>>>i is 5`  
> `>>>increment i`  
> `>>>i`  
> `i: 4`
