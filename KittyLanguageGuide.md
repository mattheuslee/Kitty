# Kitty Language Guide (v0.2.0)
Kitty is a basic high-level scripting language that is suitable for beginners getting into programming.  
It allows users to easily control Arduino functions through simple commands with immediate results.

## Introduction
The prompt `>>>` indicates that the interpreter is ready for us to begin entering commands.  
If there is output from any of the commands, it is displayed immediately after each of the commands.  

## Names  
Names can contain the characters `a-z`, `A-Z`, `0-9` and `_` (underscores), in any order.  
The only restriction is that names cannot begin with an uppercase letter or number.  
Everything created in Kitty must be given its own name.

## Numbers  
The most basic thing we can work with in Kitty is numbers.  
The `IsNumber` command creates numbers. There are a few versions of this command:  
To create a number called `answer` that contains the value 0(which is the default), use:
> `>>> answer IsNumber`  
or  
> `>>> answer IsNumber()`

To create a number called `answer` that contains the value 42 instead:  
> `>>> answer IsNumber(42)` 

## External Devices  
Kitty allows us to use external devices like LEDs and servos.   
Once we've connected the pins of those external devices to the Arduino, we just need to create their corresponding devices within Kitty in order to control them.  

__LEDs__  
LEDs have 2 pins: the shorter pin goes to the Arduino GND(ground) pin, and the longer pin goes to an Arduino pin of our choice, through a 220 ohm resistor.  
Pin 13 on the Arduino already has an inbuilt LED, which we'll use for this example.  
The `IsLED` command creates LEDs. There are a few versions of this command:  
To create an LED called `light` that uses pin 13 and starts at 50% brightness(which is the default:  
> `>>> light IsLED(13)`  

To create an LED called `light` that uses pin 13 and starts at 25% brightness instead:
> `>>> light IsLED(13, 25)`  

__Servos__  
Servos have 3 wires: the red wire goes to the Arduino 5V pin, the black wire goes to the Arduino GND pin, and the orange wire goes to an Arduino pin of our choice. 
The `IsServo` command creates servos. There are a few versions of this command:  
To create a servo called `sweeper` that uses pin 10 and starts at 90 degrees(which is the default):  
> `>>> sweeper IsServo(10)`  

To create a servo called `sweeper` using pin 10 that starts at 45 degrees instead:
> `>>> sweeper IsServo(10, 45)`  

## Name Clashes

The same name can only be used once. For example, after the following commands are run, `thing` will refer to the servo, and not the LED:  
> `>>> thing IsLED(13)`  
> `>>> thing IsServo(10)`  

If at this point we still want to work with the LED at pin 13, then we'll need to give it another name, for example:  
> `>>> CreateLED light_thing 13`  

## Displaying  
To display information about something, just type in its name, and its information will be displayed.  
> `>>> answer IsNumber(42)`  
> `>>> answer`  
> `answer: Number storing 42`  

> `>>> light IsLED(13)`  
> `>>> light`  
> `light: LED using pin 13 (50%)`  

> `>>> sweeper IsServo(10)`  
> `>>> sweeper`  
> `sweeper: Servo using pin 10 (90 degrees)`

We can also display all numbers, LEDs or servos using the `AllNumbers`, `AllLEDs` and `AllServos` commands. We can also display everything that we've created so far using the `All` command.

> `>>> answer_a IsNumber(42)`  
> `>>> answer_b IsNumber(43)`  
> `>>> AllNumbers`  
> `answer_a: Number storing 42`  
> `answer_b: Number storing 43`  

> `>>> answer_a IsNumber(42)`  
> `>>> answer_b IsNumber(43)`   
> `>>> light_a IsLED(13)`  
> `>>> light_b IsLED(14)`  
> `>>> All`  
> `answer_a: Number storing 42`  
> `answer_b: Number storing 43`  
> `light_a: LED using pin 13 (50%)`  
> `light_b: LED using pin 14 (50%)`  

## Modification  
For numbers, LEDs and servos, there are two commands that we can use to modify them:  
* `MoveBy`
* `SetTo`

__`MoveBy`__  
The `MoveBy` command executes a relative movement.  
To increase a number value called `answer` by 10:  
> `>>> answer MoveBy(10)`

The modification commands work with negative numbers as well.  
To decrease a number value called `answer` by 10:  
> `>>> answer MoveBy(-10)`

The `MoveBy` command works to increase and decrease an LED brightness as well.  
To increase the brightness of an LED called `light` by 10%:  
> `>>> light MoveBy(10)`

The `MoveBy` command also works to increase and decrease a servo angle.  
To decrease the angle of a servo called `sweeper` by 10 degrees:  
> `>>> sweeper MoveBy(-10)`

__`SetTo`__  
The `SetTo` command executes a movement to an absolute value.  
To set a number value called `answer` to 101:  
> `>>> answer SetTo(101)`

To set the brightness of an LED called `light` to 75%:
> `>>> light SetTo(75)`

To set the angle of a servo called `sweeper` to 45 degrees:
> `>>> sweeper SetTo(45)`

There are also versions of `MoveBy` and `SetTo` that only last for a specific amount of time, called `MoveByFor` and `SetToFor`.  
To increase the brightness of an LED called `light` by 50% for 1 second, and then back to its original brightness after that:  
> `>>> light MoveByFor(50, 1s)`

To set the angle of a servo called `sweeper` to 135 degrees for 1 second, and then back to its original angle after that:
> `>>> sweeper SetToFor(135, 1s)`

## Time Delays
The `Wait` command allows for time delays.  
To cause the program to wait for 2 seconds before continuing:  
> `>>> Wait(2s)`  

To cause a program to wait for 0.25 seconds before continuing:
> `>>> Wait(0.25s)`  

To cause a program to wait for 250 milliseconds(0.25 seconds) before continuing:
> `>>> Wait(250ms)`  

## Command Groups  
We can create command groups which contain one or more command.  
To start creating a command group called `blink`:  
> `>>> blink IsGroup(`  
> `(blink)>>> `  

Notice that the prompt changes to `(blink)>>> `, which means that we are currently entering commands that will be part of the `blink` group.  
Also, notice that we only include `(` after the `IsGroup` command. Once we're done entering commands for the group, entering `)` closes the group and saves it:  
> `(blink)>>> light SetTo 100 For 1s`  
> `(blink)>>> )`  
> `>>> blink`  
> `blink: Command Group containing the command(s):`  
> `1. light SetTo 100 For 1s`

When creating a command group, each command within the group must be on its own line.

Once we have created the command group `blink`, we can run its commands using the `Run` command:  
> `>>> blink Run`  

Running a command group once is exactly the same as if we were to execute all the commands in that group, one by one.  

## Expressions
+, -, *, /, %, ^, etc...

## Conditions  
Conditions are what allows us to select between several choices, depending on whether our specified conditions are met or not.  
The most basic conditions are `True` and `False`, and all other conditions will always be evaluated to result in either `True` or `False`.  

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
`>>> answer IsNumber(42)`  
`>>> light IsLED(13, 42)`  
* `answer = 42` is `True`
* `light = 42` is `True`
* `light = 50` is `False`
* `answer = light` is `True`

__Combining Conditions__  
There are a few ways to combine conditions together:  
* `And` - if both conditions are `True`, then the combination is `True`
  * `True And True` is `True`
  * `False And False` is `False`
* `Or` - if either condition, or both conditions are `True`, then the combination is `True`
  * `True Or False` is `True`
  * `False Or False` is `False`
* `Xor` - if either one condition, but not both, is `True`, then the combination is `True`
  * `True Xor False` is `True`
  * `True Xor True` is `False`
* `Not` - turns `True` into `False` and `False` into `True`  
  * `Not True` is `False`
  * `Not False` is `True`

When combining multiple conditions together, we should use brackets `( )` to enclose conditions that we want to evaluate together.  
If no brackets are present, the conditions will be evaluated from left to right, which might produce different results than expected:  
* `(True Or False) Or (False Xor True)` is `True`
* `True Or False Or False Xor True` is `False`

__`If` Command__   
The `If` command runs its commands once, only if the condition is evaluated to be `True`.  
We want to create an LED called `light` using the pin number stored in `pin`, if that number is 20 or less.
> `>>> pin IsNumber(15)`  
> `>>> light IsLED(13)`  
> `>>> If(pin <= 20) (`  
> `>>> light IsLED(pin)`  
> `>>> )`  
> `>>> light`  
> `light: LED using pin 15 (50%)`  

When using the `If` command, the condition must be on the same line as the `If`, and each of the commands contained within must be each on their own line.  
`(` and `)` are used here in a similar way as in command groups, to indicate the start and end of the groups which make up the `If` command.

Here's an example where the condition is `False`:  
> `>>> pin IsNumber(25)`  
> `>>> light IsLED(13)`  
> `>>> If(pin <= 20) (`  
> `>>> light IsLED(pin)`  
> `>>> )`  
> `>>> light`  
> `light: LED using pin 13 (50%)`  

We can see here that since the condition `pin <= 20` is `False`, none of the commands within the `If` command are run, and so `light` is still referring to the LED using pin 13.

__`Else` Command__   
Sometimes when we have an `If` command that runs several commands if a condition is `True`, we also want to be able to run some other commands if that condition is `False` instead.  

We want to create an LED called `light` using the pin number stored in `pin`, if that number is 20 or less. If it is more than 20, then we want to create an LED called `light` using pin 13 instead:  
> `>>> pin IsNumber(15)`  
> `>>> If(pin <= 20) (`  
> `>>> light IsLED(pin)`  
> `>>> )`  
> `>>> Else(`  
> `>>> light IsLED(13)`  
> `>>> )`  
> `>>> light`  
> `light: LED using pin 15 (50%)`  

And here's the alternative scenario:  
> `>>> pin IsNumber(25)`  
> `>>> If(pin <= 20) (`  
> `>>> light IsLED(pin)`  
> `>>> )`  
> `>>> Else(`  
> `>>> light IsLED(13)`  
> `>>> )`
> `>>> light`  
> `light: LED using pin 13 (50%)`  

If there is an `Else` command, it must be in the line immediately after the `)` of the `If` command.

__`While` Command__   
The `While` command keeps running its commands as long as the condition is still evaluated to be `True`.  
We want to count from 1 to 5, using a number stored in `num`:  
> `>>> num IsNumber(1)`  
> `>>> While(num <= 5) (`  
> `>>> num`  
> `>>> num MoveBy(1)`  
> `>>> )`  
> `num: Number storing 1`  
> `num: Number storing 2`  
> `num: Number storing 3`  
> `num: Number storing 4`  
> `num: Number storing 5`  

We can see here that the value stored in `num` will keep increasing one step at a time, until it reaches 6. When it is 6, then `num <= 5` is `False`, and the `While` command stops repeating its commands.  
