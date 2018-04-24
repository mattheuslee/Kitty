# Kitty Language Guide (v0.2.0)
Kitty is a basic high-level scripting language that is suitable for beginners getting into programming.  
It allows users to easily control Arduino functions through simple commands with immediate results, and using the most compact code as possible.

## Introduction
The prompt `>>>` indicates that the interpreter is ready for you to begin entering commands.  
If there is output from any of the commands, it is displayed immediately after each of the commands.  

## A. Names  
Names can contain the characters `a-z`, `A-Z`, and `_` (underscores), in any order.  
The only restriction is that names cannot begin with an uppercase letter, as such names as used specially for commands.  

## B. Creation/Assignment  
There are various types of things that you can work with in Kitty:  
* Number
* LED
* Servo

Whenever you create anything, you need to give it a name.  

__Numbers__  
Numbers store the value 0 by default, if you don't specify a starting value.  
To create a number called `answer`:  
> `>>> CreateNumber answer`  

To create a number called `answer` that stores 42:  
> `>>> CreateNumber answer 42`  

__LED__  
LEDs have 2 pins: the shorter pin goes to the Arduino GND(ground) pin, and the longer pin goes to an Arduino pin of your choice, through a 220 ohm resistor.  
Pin 13 on the Arduino already has an inbuilt LED, which we'll use for this example.  
To create an LED called `light` using pin 13:  
> `>>> CreateLED light 13`  

You might notice that after this LED is created, it lights up to 50%. This is the default starting brightness.  
To create an LED called `light` using pin 13 that starts at 25% brightness instead:
> `>>> CreateLED light 13 25` 

__Servo__  
Servos have 3 wires: the red wire goes to the Arduino 5V pin, the black wire goes to the Arduino GND pin, and the orange wire goes to an Arduino pin of your choice.  
To create a servo called `sweeper` using pin 10:  
> `>>> CreateServo sweeper 10`  

You might notice that after this Servo is created, it moves to the middle position, or 90 degrees. This is the default starting position.  
To create a servo called `sweeper` using pin 10 that starts at 45 degrees instead:
> `>>> CreateServo sweeper 10 45`  

You can only use the same name once. For example, after the following commands are run, `thing` will refer to the servo, and not the LED:  
> `>>> CreateLED thing 13`  
> `>>> CreateServo thing 10`  

If at this point you still want to work with the LED at pin 13, then you'll need to give it another name, for example:
> `>>> CreateLED light_thing 13`  

## C. Displaying  
To display information about something, just type in its name, and its information will be displayed.  
> `>>> CreateNumber answer 42`  
> `>>> answer`  
> `answer: Number storing 42`  

> `>>> CreateLED light 13`  
> `>>> light`  
> `light: LED using pin 13 (50%)`  

> `>>> CreateServo sweeper 10`  
> `>>> sweeper`  
> `sweeper: Servo using pin 10 (90 degrees)`

You can also display all numbers, LEDs or servos using the `AllNumbers`, `AllLEDs` and `AllServos` commands. You can also display everything using the `All` command.

> `>>> CreateNumber answer_a 42`  
> `>>> CreateNumber answer_b 43`  
> `>>> CreateNumber answer_c 44`  
> `>>> AllNumbers`  
> `answer_a: Number storing 42`  
> `answer_b: Number storing 43`  
> `answer_c: Number storing 44`  

> `>>> CreateNumber answer_a 42`  
> `>>> CreateNumber answer_b 43`  
> `>>> CreateLED light_a 13`  
> `>>> CreateLED light_b 14`  
> `>>> All`  
> `answer_a: Number storing 42`  
> `answer_b: Number storing 43`  
> `light_a: LED using pin 13 (50%)`  
> `light_b: LED using pin 14 (50%)`  

## D. Modification  
For numbers, LEDs and servos, there are two commands that you can use to modify them:  
* `MoveBy`
* `SetTo`

__`MoveBy`__  
The `MoveBy` command executes a relative movement.  
To increase a number value called `answer` by 10:  
> `>>> answer MoveBy 10`

The modification commands work with negative numbers as well.  
To decrease a number value called `answer` by 10:  
> `>>> answer MoveBy -10`

The `MoveBy` command works to increase and decrease an LED brightness as well.  
To increase the brightness of an LED called `light` by 10%:  
> `>>> light MoveBy 10`

The `MoveBy` command also works to increase and decrease a servo angle.  
To decrease the angle of a servo called `sweeper` by 10 degrees:  
> `>>> sweeper MoveBy -10`

__`SetTo`__  
The `SetTo` command executes a movement to an absolute value.  
To set a number value called `answer` to 101:  
> `>>> answer SetTo 101`

To set the brightness of an LED called `light` to 75%:
> `>>> light SetTo 75`

To set the angle of a servo called `sweeper` to 45 degrees:
> `>>> sweeper SetTo 45`

There are also versions of `MoveBy` and `SetTo` that only last for a specific amount of time, with an additional `For`.  
To increase the brightness of an LED called `light` by 50% for 1 second, and then back to its original brightness after that:  
> `>>> light MoveBy 50 For 1s`

To set the angle of a servo called `sweeper` to 135 degrees for 1 second, and then back to its original angle after that:
> `>>> sweeper SetTo 135 For 1s`

## E. Time Delays
The `Wait` command allows for time delays.  
To cause the program to wait for 2 seconds before continuing:  
> `>>> Wait 2s`  

To cause a program to wait for 0.25 seconds before continuing:
> `>>> Wait 0.25s`  

To cause a program to wait for 250 milliseconds(0.25 seconds) before continuing:
> `>>> Wait 250ms`  

## F. Command Groups  
You can command groups which contain one or more command, which will make it more convenient to work with more commands.  
To start creating a command group called `blink`:  
> `>>> CreateGroup blink`  
> `(blink)>>> `  

You will notice that the prompt changes to `(blink)>>> `, which means that you are currently entering commands that will be part of the `blink` group.  
Once you're done entering commands for the group, the `EndGroup` command closes up the group and saves it.  
> `(blink)>>> light SetTo 100 For 1s`  
> `(blink)>>> EndGroup`  
> `Command Group blink closed.`  
> `>>> blink`  
> `blink: Command Group containing the command(s):`  
> `light SetTo 100 For 1s`

Once you have created the command group `blink`, you can run its commands using the `RunGroup` command:  
> `>>> RunGroup blink`  

Running a command group once is exactly the same as if you were to execute all the commands in that group, one by one.  
We have previously seen the command to run a command group once. If you want to continuously run `blink`, you can do so with an additional `Forever`:  
> `>>> RunGroup blink Forever`

There are several ways for you to control how to run a command group.  
If you want to run `blink` once, and only if a certain `<condition>` is true:  
> `>>> RunGroup blink If <condition>`  

If you want to continuously run `blink` as long as a certain `<condition>` continues to be true:  
> `>>> RunGroup blink While <condition>`  

If you want to continuously run `blink` until a certain `<condition>` becomes true, and then stop:  
> `>>> RunGroup blink Until <condition>`  

## G. Conditions  
We have seen a certain `<condition>` that we can specify for some `RunGroup` commands.  
All conditions are always evaluated to result in either `True` or `False`.

__Comparing Values__  
Values can be compared to check if they are:  
* equal (`=`)
  * `10 = 10` is `True`
  * `11 = 10` is `False`
* less than (`<`)
  * `5 < 10` is `True`
  * `10 < 5` is `False`
* greater than (`>`)
  * `10 > 5` is `True`
  * `5 > 10` is `False`
* less than or equal to (`<=`)
  * `5 <= 10` is `True`
  * `10 <= 10` is `True`
* greater than or equal to (`>=`)
  * `10 >= 5` is `True`
  * `10 >= 10` is `True`

Besides using raw numbers, the values in Numbers, LEDs and Servos can also be used in comparisons. For example:  
`>>> CreateNumber answer 42`  
`>>> CreateLED light 13 25`  
* `answer = 42` is `True`
* `light = 25` is `True`
* `light = 50` is `False`

__Combining Conditions__  
There are a few ways to combine conditions together:  
* `And` - if both conditions are `True`, then the combination is `True`
  * `True And True` is `True`
  * `False And False` is `False`
* `Or` - if either condition, or both conditions are `True`, then the combination is `True`
  * `True Or False` is `True`
  * `False Or False` is `False`
* `Xor` - if only one condition is `True`, then the combination is `True`
  * `True Xor False` is `True`
  * `True Xor True` is `False`

When combining multiple conditions together, you should use brackets `( )` to enclose conditions that you want to evaluate together.  
If no brackets are present, the conditions will be evaluated from left to right, which might produce different results than expected:  
* `(True Or False) Or (False Xor True)` is `True`
* `True Or False Or False Xor True` is `False`
