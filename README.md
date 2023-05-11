# Code Explanation - Mine navigation search and rescue

## Summary

This is a summary to explain what is the function of each of our code.

## main.c

The main.c file first calls out the initialisation of the motor, color clicker board, interrupt, timer. Serial communcation is also initialised for debugging purposes. Two structure variables, **RGBC_val** and **Rel_RGB**, are set up. **RGBC_val**  is used to store the RGB value detected, and their ratios to the colour "clear" is calculated and stored in **Rel_RGB**.  In the while(1) loop, the function **explore_maze** is called out and it is regulated by the variable **undo_complete**. When the buggy hit the colour "white", undo_complete would be set to 1 and the the buggy will return home and stop.

1. Initialisation 
    1. Motor
    2. Color Cliker Board
    3. Interrupt
    4. Timer
  
2. While loop
    1. Maze Exploration
    2. Return home and stop if white is detected
    3. Return home if car is lost




## dc_motor.c/h

The dc_motor.c file contains the initialisation code, **initDCmotorsPWM()** and **setMotorPWM()**, the functions to control the buggy's movements, **stop()**, **turnLeft()**, **turnRight()**, **fullSpeedAhead()**, **reverse()**, **turnRight90()**, **turnLeft90()**, **turn180()**, **RevAndRight()**, **RevAndLeft()**, **turnRight135()**, **turnLeft135()**, and the function that sends the buggy back to the starting point, **UndoActions()**. 

The **initDCmotorsPWM()** function initializes the PWM outputs and sets up the necessary registers. The **setMotorPWM()** function sets the PWM output for each motor based on its current power and direction. The **delay_ms()** function provides a simple delay mechanism by looping until a specified number of milliseconds have elapsed. It allows the use of variable delay times. 

Two lists that contains 30 elements, **time_record** and **move_record**, are used to store the duration between each action (in otherwords, the duration that the buggy is going full speed ahead), and the series of moves being executed.

In **UndoActions()**, the timer is first turned of by setting **PIE0bits.TMR0IE = 0**, which stop the code from counting the time during its journey to the starting point.  After that, the function begins by turning the robot around by calling the **turn180()** function, which turns the robot by 180 degrees. The robot is then instructed to move forward for the latest duration that it moved forward before hitting the wall by calling **fullSpeedAhead()** and **delay_ms()**. The function then loops through the time and move record arrays in reverse order, starting from the latest move and time recorded. For each recorded move, the function uses the corresponding movement function to move the robot backward in the opposite direction of the recorded move.

Colour | Case | Instruction | Reverse Instruction
---------|---------|---------|---------
Red | 1 | Turn Right 90 | Turn Left 90
Green | 2 | Turn Left 90 | Turn Right 90
Blue | 3 | Turn 180 | Turn 180
Yellow | 4 | Reverse 1 square and turn right 90 | (Reverse) turn Left 90 and reverse 1 square
Pink | 5 | Reverse 1 square and turn left 90 | (Reverse) turn Right 90 and reverse 1 square
Orange | 6 | Turn Right 135 | Turn Left 135
Light blue | 7 | Turn Left 135 | Turn Right 135
White | 8 | Return | N/A
Black | 9 | Maze wall colour | N/A

### Maze Exporation

            if (color_code == 1) {     //If colour is detected (red)
                time_index++; // Increment time counter array index
                PIE0bits.TMR0IE = 0; // Turn of timer interrupt so to stop counting time while buggy is turning
                turnRight90(mL,mR); // Turn based on color_code
                move_record[move_index] = 1; // Append color_code into move record array
                move_index++; // Increment move array index
                PIE0bits.TMR0IE = 1; // Turn timer interrupt back on
               
This logic is then looped through all the colours

### Move Functions
            void turnRight90(DC_motor *mL, DC_motor *mR)
            {
                reverse (mL,mR);   // reverse to avoid turning right at the wall
                __delay_ms(300);   // calibrate this delay to determine the reversing distance
                turnRight (mL, mR)  // execute the turn
                __delay_ms(400);   // calibrate this delay to determine the turning angle 
            }

The same knowledge applies to other actions 

## Timer.c/h

The Timer.c file contains the initialisation code for Timer0. The timer registers **TMR0H** and **TMR0L** are set to zero initially to ensure that the timer starts at zero. The timer is then enabled by setting the **T0CON0bits.T0EN** bit to 1.

The interrupt for Timer 0 is set up by enabling the **TMR0IE** bit in the **PIE0** register, which enables interrupts for Timer 0. The **PIR0bits.TMR0IF** bit is cleared to ensure that the interrupt flag is initially set to zero. The **IPR0bits.TMR0IP** bit is set to zero to indicate that the interrupt priority is low. The **INTCONbits.IPEN** bit is set to enable interrupt priority, and the **INTCONbits.PEIE** and **INTCONbits.GIE** bits are set to enable peripheral interrupts and global interrupts, respectively. This enables the interrupt service routine to be executed whenever Timer 0 overflows.

## Interrupt.c/h

The Interrupt.c file contains the initialisation and set up code for our interrupt service routine. This ISR ensures that Timer0 overflows every second, and increment the current element within the **time_record** list by 1 second. The purpose of this is to obtain the duration while the buggy is going full speed ahead between colour cards.

## color.c/h

The color.c file contains the  initialization, reading, and processing of data from a color sensor module. The **color_click_init()** function is used to set up the sensor by initializing the I2C master, configuring the device via I2C commands, and turning on the LED lights to produce white light. The **color_read()** function reads the color values from the sensor and stores them in a structure, while **update_rel_rgb()** function normalizes the color ratios to 255 (clear) and updates them in another structure. The **color_check()** function uses the normalized ratios to determine the color detected and returns a corresponding code. The function **color_inverse()** returns the inverse action of a given color code by incorporating a series of if statements.


Colour | Case | Instruction
---------|---------|---------
Red | 1 | Turn Right 90
Green | 2 | Turn Left 90
Blue | 3 | Turn 180
Yellow | 4 | Reverse 1 square and turn right 90
Pink | 5 | Reverse 1 square and turn left 90
Orange | 6 | Turn Right 135
Light blue | 7 | Turn Left 135 
White | 8 | Return Home
Black | 9 | Maze wall colour




## i2c.c/h

The i2c.c file sets up the commincation between the colour clicker board and the microcontroller.

## serial.c/h

The serial.c/h file sets up the commincation between Realterm and the microcontroller. It prints out variables such as **time_index** and **current time** for debugging purpose.
