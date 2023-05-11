#include <xc.h>
#include "Timer.h"
//#include "LEDarray.h"
/************************************
 * Function to set up timer 0
************************************/
void Timer0_init(void)
{
    T0CON1bits.T0CS=0b010; // Fosc/4
    T0CON1bits.T0ASYNC=1; // see datasheet errata - needed to ensure correct operation when Fosc/4 used as clock source
    T0CON1bits.T0CKPS=0b1000; //for real 1 minute: change this to 0b1110;
    T0CON0bits.T016BIT=1;	//Set 8bit mode or 16bit mode
	
    // it's a good idea to initialise the timer registers so we know we are at 0
    // it's a good idea to initialise the timer registers so we know we are at 0
    TMR0H=0b00001011;            //for real 1 minute: change this to 0b00011011;
    TMR0L=0b11011011;            //for real 1 minute: change this to 0b00011101
    T0CON0bits.T0EN=1;	//start the timer
   
       // enable interrupt
    // in here we choose the PIE0 register, peripheral interrupt sources 
    // since it PIE0, we also need to set PIR as PIR0, which is the peripheral interrupt flag 
    PIE0bits.TMR0IE = 1; // TMR0IE corresponds to timer 0 peripheral interrupt sources, as specified in notes/data sheet
    PIR0bits.TMR0IF = 0; // clear interrupt flag, TMR0IF corresponds to timer 0 
    IPR0bits.TMR0IP = 0; // IPR is the priority register, but idk why this is set as 0 (low priority)
    INTCONbits.IPEN = 1; // enable interrupt priority
    INTCONbits.PEIE = 1; // peripheral interrupt
    INTCONbits.GIE = 1; // global interrupt
}

