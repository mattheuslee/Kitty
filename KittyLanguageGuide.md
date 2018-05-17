# Kitty Language Guide (v0.2.0)
Kitty is a basic high-level scripting language that is suitable for beginners getting into programming.  
It allows users to easily control Arduino functions through simple commands with immediate results.

## Introduction
The prompt `>>>` indicates that the interpreter is ready for us to begin entering commands.  
If there is output from any of the commands, it is displayed immediately after each of the commands.  

## Names  
Names can contain the characters `a-z`, and `_` (underscores), in any order.  
Everything created in Kitty must be given its own name.

## Numbers  
The most basic thing we can work with in Kitty is numbers.  
The `IsNumber` command creates numbers. There are a few versions of this command:  
To create a number called `answer` that contains the value 0(which is the default):
```
>>> answer IsNumber()
```

To create a number called `answer` that contains the value 42 instead:  
```
>>> answer IsNumber(42)
```

## External Devices  
Kitty allows us to use external devices like LEDs and servos.   
Once we've connected the pins of those external devices to the Arduino, we just need to create their corresponding devices within Kitty in order to control them.  

__LEDs__  
LEDs have 2 pins: the shorter pin goes to the Arduino GND(ground) pin, and the longer pin goes to an Arduino pin of our choice, through a 220 ohm resistor.  
Pin 13 on the Arduino already has an inbuilt LED, which we'll use for this example.  
The `IsLED` command creates LEDs. There are a few versions of this command:  
To create an LED called `light` that uses pin 13 and starts at 50% brightness(which is the default):  
```
>>> light IsLED(13)
```

To create an LED called `light` that uses pin 13 and starts at 25% brightness instead:
```
>>> light IsLED(13, 25)
``` 

## Name Clashes

The same name can only be used once. For example, after the following commands are run, `thing` will refer to the number, and not the LED:  
```
>>> thing IsLED(13)
>>> thing IsNumber(10)
```

If at this point we still want to work with the LED at pin 13, then we'll need to give it another name, for example:  
```
>>> light_thing IsLED(13)
```

## Displaying  
To display information about something, just type in its name, and its information will be displayed.  
```
>>> answer IsNumber(42)
>>> answer
answer: number storing 42
```

```
>>> light IsLED(13)
>>> light
light: LED using pin 13 (50%)
```

We can also display words directly to the screen, using either `'` or `"` to enclose the words:
```
>>> 'hello!'
hello!
>>> "hello!"
hello!
```

We can also combine the two into the more complex `Print` command. The `Print` command allows us to construct a sentence to print out of smaller parts, which can be words or values taken from numbers or LEDs.  
To print the sentence "The value of answer is \<value\>", where \<value\> is the current number stored in `answer`:  
```
>>> answer IsNumber(42)
>>> Print('The value of answer is ', answer)
The value of answer is 42
```
Notice how we can just use `answer` in the command and it automatically replaces it with its value. Numbers put into the `Print` command are replaced with their number value, and LEDs put into the `print` command are replaced with their brightness percentage.

## Modification  
For numbers and LEDs, there are two commands that we can use to modify them:  
* `MoveBy`
* `SetTo`

__`MoveBy`__  
The `MoveBy` command executes a relative movement.  
To increase a number value called `answer` by 10:  
```
>>> answer MoveBy(10)
```

The modification commands work with negative numbers as well.  
To decrease a number value called `answer` by 10:  
```
>>> answer MoveBy(-10)
```

The `MoveBy` command works to increase and decrease an LED brightness as well.  
To increase the brightness of an LED called `light` by 10%:  
```
>>> light MoveBy(10)
```

There is also a version of `MoveBy` that only lasts for a specific amount of time - `MoveByFor`.  
To increase the brightness of an LED called `light` by 50% for 1 second(1000 milliseconds), and then back to its original brightness after that time:  
```
>>> light MoveByFor(50, 1000)
```

Note that since the amount of time to wait is in milliseconds, you can have wait times that are less than 1 second, like half a second(500 milliseconds).

__`SetTo`__  
The `SetTo` command executes a movement to an absolute value.  
To set a number value called `answer` to 101:  
```
>>> answer SetTo(101)
```

To set the brightness of an LED called `light` to 75%:
```
>>> light SetTo(75)
```

There are also versions of `MoveBy` and `SetTo` that only last for a specific amount of time, called `MoveByFor` and `SetToFor`.  
To increase the brightness of an LED called `light` by 50% for 1 second (1000 milliseconds), and then back to its original brightness after that:  
```
>>> light MoveByFor(50, 1000)
```

To set the brightness of an LED called `light` to 100% for 1 second (1000 milliseconds), and then back to its original brightness after that:  
```
>>> light SetToFor(100, 1000)
```

## Time Delays
The `Wait` command allows for time delays.  
To wait for 2 seconds (2000 milliseconds) before continuing:  
```
>>> Wait(2000)
```

To wait for 0.25 seconds (250 milliseconds) before continuing:
```
>>> Wait(250)
```

## Command Groups  
Sometimes we don't want to keep typing the same commands throughout our program. Command groups allow us to group multiple commands together under a single group name.   
To start creating a command group called `blink`:  
```
>>> blink IsGroup (
(blink) >>> 
```

Notice that the prompt changes to `(blink) >>> `, which means that we are currently entering commands that will be part of the `blink` group. As long as the prompt still shows `(blink) >>>`, all of the commands that we enter will go directly to be stored in the group, and will not be immediately executed.  
Also, notice that we only include `(` after the `IsGroup` command. Once we're done entering commands for the group, entering `)` closes the group and saves it, and the prompt goes back to the original `>>>`:  
```
(blink) >>> light MoveByFor(100, 1000)
(blink) >>> )
>>> blink
blink: group containing the command(s)
     light MoveByFor(100, 1000)
```

When creating a command group, each command within the group must be on its own line.  

Once we have created the command group `blink`, we can run its commands using the `RunGroup` command:  
```
>>> blink RunGroup()
```

Running a command group once is exactly the same as if we were to execute all the commands in that group, one by one. So doing this:  
```
>>> blink RunGroup()
```
Has the same effect as doing this:
```
>>> light MoveByFor(100, 1000)
```

By default, if there are no numbers between the brackets of the `RunGroup` command, the group is run once.  
If we want to run the group multiple times, we can provide the number of times the group should run.  
To run the command group `blink` 10 times:  
```
>>> blink RunGroup(10)
```
We can also run a command group continuously(forever):
```
>>> blink RunGroup(-1)
```

## Expressions
| Symbol      | Meaning                                             | Example       |  
|:-----------:|:----------------------------------------------------|:-------------:|  
| +           | Add two values                                      | 1 + 2 gives 3 |  
| -           | Subtract one value from another                     | 3 - 2 gives 1 |  
| *           | Multiply one value with another                     | 3 * 2 gives 6 |  
| /           | Divide one value by another, giving the quotient    | 6 / 2 gives 3 |  
|             |                                                     | 5 / 2 gives 2 |
| %           | Divide one value by another, giving the remainder   | 6 % 2 gives 0 |  
|             |                                                     | 5 % 2 gives 1 |
| %           | The first value to the power of the second value,   | 2 ^ 3 gives 8 |  
|             | or the first value multiplied by itself for a       | 3 ^ 1 gives 3 |  
|             | number of times equal to the second value.          | 3 ^ 2 gives 9 |  

## Conditions  
Conditions are what allows us to select between several choices, depending on whether our specified conditions are met or not.  
The most basic conditions are true and false, and all other conditions will always be evaluated to result in either true or false.  

__Comparing Values__  
Values can be compared to check if they are:  
* equal (`=`)
  * `10 = 10` is true
  * `11 = 10` is false
* less than (`<`)
  * `5 < 10` is true
  * `10 < 5` is false
* greater than (`>`)
  * `10 > 5` is true
  * `5 > 10` is false
* less than or equal to (`<=`)
  * `5 <= 10` is true
  * `10 <= 10` is true
* greater than or equal to (`>=`)
  * `10 >= 5` is true
  * `10 >= 10` is true

Besides using raw numbers, the values in Numbers, LEDs and Servos can also be used in comparisons. For example:  
```
>>> answer IsNumber(42)
>>> light IsLED(13, 42)
```
* `answer = 42` is true
* `light = 42` is true
* `light = 50` is false
* `answer = light` is true

In addition, the value of a number or LED can be used directly as a condition. If used this way, any value other than 0 is true, and only the value of 0 is false.  We will be using `1` and `0` in the next section to show this.

__Combining Conditions__  
There are a few ways to combine conditions together:  
* & (and) - if both conditions are true, then the combination is true
  * `1 & 1` is true
  * `1 & 0` is false
* | (or) - if either condition, or both conditions are true, then the combination is true
  * `1 | 0` is true
  * `0 | 0` is false
* ! (xor) - if either one condition, but not both, is true, then the combination is true
  * `1 ! 0` is true
  * `1 ! 1` is false
* ~ (not) - turns true into false and false into true  
  * `~ 1` is false
  * `~ 0` is true

When combining multiple conditions together, we should use brackets `( )` to enclose conditions that we want to evaluate together.  
If no brackets are present, the conditions will be evaluated from left to right, which might produce different results than expected, even though the only difference may be the brackets:  
* `(1 | 0) | (0 ! 1)` is true
* `1 | 0 | 0 ! 1` is false  

__`If` and `Else` Commands__   
We've previously seen the `IsGroup` command that can create command groups for us to run later.  
The `If` and `Else` commands also create command groups, with some big differences:  
* These command groups are run immediately when they're closed, unlike `IsGroup` command groups.
* These groups have no name, since they are meant to run immediately.
* These groups only run if certain conditions are met.

The `If` command runs its commands once, only if its condition is evaluated to be true.  
```
>>> answer IsNumber(42)
>>> If (answer = 42) (
(IF) >>> answer MoveBy(10)
>>> )
>>> answer
answer: number storing 52
```

When using the `If` command, the condition must be on the same line as the `If`, and each of the commands contained within must be each on their own line. You will notice that other aspects are quite similar to the `IsGroup` command, like the use of `(` and `)`.  

Here's an example where the condition is false, and thus the commands inside the `If` are not run:  
```
>>> answer IsNumber(42)
>>> If (answer < 42) (
(IF) >>> answer MoveBy(10)
(IF) >>> )
>>> answer
answer: number storing 42
```

Sometimes when we have an `If` command that runs several commands if a condition is true, we also want to be able to run some other commands if that condition is false instead, giving us two possible paths for our program.  

```
>>> answer IsNumber(42)
>>> If (answer < 42) (
(IF) >>> answer MoveBy(10)
(IF) >>> )
>>> Else (
(ELSE) >>> answer IsNumber(0)
(ELSE) >>> )
>>> answer
answer: number storing 0
```

The commands in an `Else` are only run if the `Else` comes immediately after the closing `)` line of an `If` command, and that the condition for that `If` command was false.

__Basic Programs__   
This program prints out `num`, for values 1 to 10.  
```
>>> print_num IsGroup (
(print_num) >>> num
(print_num) >>> num MoveBy(1)
(print_num) >>> )
>>> num IsNumber(1)
>>> print_num RunGroup(10)
num: number storing 1
num: number storing 2
num: number storing 3
num: number storing 4
num: number storing 5
num: number storing 6
num: number storing 7
num: number storing 8
num: number storing 9
num: number storing 10
```

This program runs through the same values 1 to 10, but every time num is even it prints `'even'`, and every time num is odd it prints `'odd'`.  
```
>>> odd_even IsGroup (
(odd_even) >>> num
(odd_even) >>> If (num % 2 = 0) (
(odd_even) >>> 'even'
(odd_even) >>> )
(odd_even) >>> Else (
(odd_even) >>> 'odd'
(odd_even) >>> )
(odd_even) >>> num MoveBy(1)
(odd_even) >>> )
>>> num IsNumber(1)
>>> odd_even RunGroup(10)
num: number storing 1
odd
num: number storing 2
even
num: number storing 3
odd
num: number storing 4
even
num: number storing 5
odd
num: number storing 6
even
num: number storing 7
odd
num: number storing 8
even
num: number storing 9
odd
num: number storing 10
even
```
