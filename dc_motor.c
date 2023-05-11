#include <xc.h>
#include "dc_motor.h"
#include "color.h"
#include "serial.h"
#include "Interrupt.h"

// function initialise T2 and CCP for DC motor control
void initDCmotorsPWM(unsigned int PWMperiod){
    //initialise your TRIS and LAT registers for PWM  
    LATEbits.LATE2 = 0;
    LATEbits.LATE4 = 0;
    LATCbits.LATC7 = 0;
    LATGbits.LATG6 = 0;
    
    TRISEbits.TRISE2 = 0;
    TRISEbits.TRISE4 = 0;
    TRISCbits.TRISC7 = 0;
    TRISGbits.TRISG6 = 0;
    
    //configure PPS to map CCP modules to pins
    RE2PPS=0x05; //CCP1 on RE2
    RE4PPS=0x06; //CCP2 on RE4
    RC7PPS=0x07; //CCP3 on RC7
    RG6PPS=0x08; //CCP4 on RG6

    // timer 2 config
    T2CONbits.CKPS=0b100; // 1:16 prescaler
    T2HLTbits.MODE=0b00000; // Free Running Mode, software gate only
    T2CLKCONbits.CS=0b0001; // Fosc/4

    // Tpwm*(Fosc/4)/prescaler - 1 = PTPER
    // 0.0001s*16MHz/16 -1 = 99
    T2PR=PWMperiod; //Period reg 10kHz base period
    T2CONbits.ON=1;
    
    //setup CCP modules to output PMW signals
    //initial duty cycles 
    CCPR1H=0; 
    CCPR2H=0; 
    CCPR3H=0; 
    CCPR4H=0; 
    
    //use tmr2 for all CCP modules used
    CCPTMRS0bits.C1TSEL=0;
    CCPTMRS0bits.C2TSEL=0;
    CCPTMRS0bits.C3TSEL=0;
    CCPTMRS0bits.C4TSEL=0;
    
    //configure each CCP
    CCP1CONbits.FMT=1; // left aligned duty cycle (we can just use high byte)
    CCP1CONbits.CCP1MODE=0b1100; //PWM mode  
    CCP1CONbits.EN=1; //turn on
    
    CCP2CONbits.FMT=1; // left aligned
    CCP2CONbits.CCP2MODE=0b1100; //PWM mode  
    CCP2CONbits.EN=1; //turn on
    
    CCP3CONbits.FMT=1; // left aligned
    CCP3CONbits.CCP3MODE=0b1100; //PWM mode  
    CCP3CONbits.EN=1; //turn on
    
    CCP4CONbits.FMT=1; // left aligned
    CCP4CONbits.CCP4MODE=0b1100; //PWM mode  
    CCP4CONbits.EN=1; //turn on
}

// function to set CCP PWM output from the values in the motor structure
void setMotorPWM(DC_motor *m)
{
    unsigned char posDuty, negDuty; //duty cycle values for different sides of the motor
    
    if(m->brakemode) {
        posDuty=m->PWMperiod - ((unsigned int)(m->power)*(m->PWMperiod))/100; //inverted PWM duty
        negDuty=m->PWMperiod; //other side of motor is high all the time
    }
    else {
        posDuty=0; //other side of motor is low all the time
        negDuty=((unsigned int)(m->power)*(m->PWMperiod))/100; // PWM duty
    }
    
    if (m->direction) {
        *(m->posDutyHighByte)=posDuty;  //assign values to the CCP duty cycle registers
        *(m->negDutyHighByte)=negDuty;       
    } else {
        *(m->posDutyHighByte)=negDuty;  //do it the other way around to change direction
        *(m->negDutyHighByte)=posDuty;
    }
}

//function to stop the robot
void stop(DC_motor *mL, DC_motor *mR)
{
    mL->power = 0;
    mR->power = 0;
    setMotorPWM(mL);
    setMotorPWM(mR);
}

//function to make the robot turn left 
void turnLeft(DC_motor *mL, DC_motor *mR)
{
    mL->direction = 1; 
    mR->direction = 0;

    mR->power = 40;
    mL->power = 40;
    setMotorPWM(mL);
    setMotorPWM(mR);
}

//function to make the robot turn right 
void turnRight(DC_motor *mL, DC_motor *mR)
{
    mL->direction = 0; 
    mR->direction = 1;

    mR->power = 40;
    mL->power = 40;
    setMotorPWM(mL);
    setMotorPWM(mR);
}

//function to make the robot go straight
void fullSpeedAhead(DC_motor *mL, DC_motor *mR)
{
    mL->power = 64; // Asymmetrical power due to differences in motor characteristics between left and right motor
    mR->power = 70;
    mL->direction = 0;      // Left wheels go forward
    mR->direction = 0;      // Right wheels go forward
    setMotorPWM(mL);       // Set PWM output for left motor
    setMotorPWM(mR);       // Set PWM output for right motor

}

void reverse (DC_motor *mL, DC_motor *mR)
{
    mL->power = 45;
    mR->power = 50;
    mL->direction = 1;      // Left wheels go backward
    mR->direction = 1;      // Right wheels go backward
    setMotorPWM(mL);       // Set PWM output for left motor
    setMotorPWM(mR);       // Set PWM output for right motor
}

/************************************
 * Below functions controls the motor based on the colour card instructions
 * Multiple copies of the same instruction are present to account for differences in required calibration when buggy is running UndoActions
************************************/
//turn right 90 degrees
//turn on power to turn right for a set period of time, how long depends on surface, so we calibrate using the delay function
void turnRight90(DC_motor *mL, DC_motor *mR)
{
    reverse (mL,mR);
    __delay_ms(600);
    turnRight (mL, mR);
    __delay_ms(620);
}

// Inverse turn right 90 degrees, this is used when buggy is running UndoActions. Different calibration was required to get turns correctly
void iturnRight90(DC_motor *mL, DC_motor *mR)
{
    reverse (mL,mR);
    __delay_ms(600);
    turnRight (mL, mR);
    __delay_ms(750);
}

//turn left 90
void turnLeft90(DC_motor *mL, DC_motor *mR)
{
    reverse (mL,mR);
    __delay_ms(600);
    turnLeft (mL, mR);
    __delay_ms(620);
}

void iturnLeft90(DC_motor *mL, DC_motor *mR)
{
    reverse (mL,mR);
    __delay_ms(600);
    turnLeft (mL, mR);
    __delay_ms(750);
}

void turn180 (DC_motor *mL, DC_motor *mR)
{
    reverse (mL,mR);
    __delay_ms(600);
    turnLeft (mL, mR);
    __delay_ms(1400);
}
// iiturn180 is used when buggy first reaches white card
void iiturn180 (DC_motor *mL, DC_motor *mR)
{
    reverse (mL,mR);
    __delay_ms(600);
    turnLeft (mL, mR);
    __delay_ms(1550);
}
// iturn180 is used in the for loop in UndoActions
void iturn180 (DC_motor *mL, DC_motor *mR)
{
    reverse (mL,mR);
    __delay_ms(600);
    turnLeft (mL, mR);
    __delay_ms(1325);
}

void RevAndRight (DC_motor *mL, DC_motor *mR)
{
    reverse (mL,mR);
    __delay_ms(2600);
    turnRight (mL, mR);
    __delay_ms(600);
}

void iRevAndRight (DC_motor *mL, DC_motor *mR)
{
    turnRight (mL, mR);
    __delay_ms(700);
    reverse (mL,mR);
    __delay_ms(2600);
}

void RevAndLeft (DC_motor *mL, DC_motor *mR)
{
    reverse (mL,mR);
    __delay_ms(2600);
    turnLeft (mL, mR);
    __delay_ms(600);
}

void iRevAndLeft (DC_motor *mL, DC_motor *mR)
{
    turnLeft (mL, mR);
    __delay_ms(700);
    reverse (mL,mR);
    __delay_ms(2600);
}


void turnRight135(DC_motor *mL, DC_motor *mR)
{
    reverse (mL,mR);
    __delay_ms(600);
    turnRight (mL, mR);
    __delay_ms(1000);
}

void iturnRight135(DC_motor *mL, DC_motor *mR)
{
    reverse (mL,mR);
    __delay_ms(600);
    turnRight (mL, mR);
    __delay_ms(1100);
}

void turnLeft135(DC_motor *mL, DC_motor *mR)
{
    reverse (mL,mR);
    __delay_ms(600);
    turnLeft (mL, mR);
    __delay_ms(1000);
}

void iturnLeft135(DC_motor *mL, DC_motor *mR)
{
    reverse (mL,mR);
    __delay_ms(600);
    turnLeft (mL, mR);
    __delay_ms(1100);
}

// Custom delay function to allow variables as inputs
void delay_ms(unsigned int ms) {
    while (ms>0) {
        __delay_ms(1);
        ms--;
    }
}

/************************************
 * This is the UndoActions function. Sweat and tears were spent getting this to work.
 * When it is called, the buggy will attempt to return to its original position when it was started
************************************/
void UndoActions(int* time_record, int* move_record, DC_motor* mL, DC_motor* mR) {
    PIE0bits.TMR0IE = 0; // Turn off timer interrupt to stop counting the time
    int i; // Index for the for loop
    iiturn180(mL, mR); // First turn 180 degrees
    
    // Go forward for the latest duration
    fullSpeedAhead(mL, mR);
    delay_ms((time_record[time_index] * 1000) - 500); // Duration subtracted by 500ms to account for delays in colour sensing
    
    // For loop to iterate through every duration in the time_record
    for (i = time_index; i >= 0; i--) {
        
        // These few lines print the current indexed duration and move through serial. It is used for debugging
        char buffer[100];
        sprintf(buffer, "i: %d, current_move: %d, current_time: %d, latest_time: %d\r\n", i, move_record[i-1], time_record[i-1], time_record[i]);
        sendStringSerial4(buffer);
        __delay_ms(50);
        
        
        // If i-1 >=0 condition is used here as time_index is always 1 index ahead of move_index
        if (i-1 >= 0) {
              

            if (move_record[i-1] == 1){
                iturnLeft90 (mL, mR); // Turn the latest read move in move_record
                fullSpeedAhead(mL, mR); // Move forward for the indexed duration
                delay_ms((time_record[i-1] * 1000) - 1000); // Duration was subtracted by 1000ms to account for various delays in colour sensing
            }
            else if (move_record[i-1] == 2){
                iturnRight90 (mL, mR);
                fullSpeedAhead(mL, mR);
                delay_ms((time_record[i-1] * 1000) - 1000); 
            }
            else if (move_record[i-1] == 3){
                iturn180 (mL, mR);
                fullSpeedAhead(mL, mR);
                delay_ms((time_record[i-1] * 1000) - 1000); 
            }
            else if (move_record[i-1] == 4){
                iRevAndLeft (mL, mR);
                fullSpeedAhead(mL, mR);
                delay_ms((time_record[i-1] * 1000) - 1000); 
            }
            else if (move_record[i-1] == 5){
                iRevAndRight (mL, mR);
                fullSpeedAhead(mL, mR);
                delay_ms((time_record[i-1] * 1000) - 1000); 
            }
            else if (move_record[i-1] == 6){
                iturnLeft135 (mL, mR);
                fullSpeedAhead(mL, mR);
                delay_ms((time_record[i-1] * 1000) - 1000); 
            }
            else if (move_record[i-1] == 7){
                iturnRight135 (mL, mR);
                fullSpeedAhead(mL, mR);
                delay_ms((time_record[i-1] * 1000) - 1000); 
            }
        }
    }
}