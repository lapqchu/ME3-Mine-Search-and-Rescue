#include <xc.h>
#include "Interrupt.h"
#include "dc_motor.h"

/************************************
 * Function to turn on interrupts and set if priority is used
 * Note you also need to enable peripheral interrupts in the INTCON register to use CM1IE.
************************************/
void Interrupts_init(void)
{
	// turn on global interrupts, peripheral interrupts and the interrupt source 
	// It's a good idea to turn on global interrupts last, once all other interrupt configuration is done.
    PIE0bits.TMR0IE = 1; //enable interrupts from Timer0
    PIE2bits.C1IE = 1; // Enable interrupt from Comparator as a source (not used in this project)
    INTCONbits.PEIE = 1; // peripheral interrupts
    INTCONbits.GIE = 1; // global interrupts ON
}


void __interrupt() ISR()
{   

    if (PIR0bits.TMR0IF == 1) { // check interrupt timer flag is 1
        
        LATHbits.LATH3 = !LATHbits.LATH3; // Blink LEDH3 to indicate operation
        time_record[time_index]++; // Increment value in time_record
        TMR0H = 0b00001011; // reset initial of timer0
        TMR0L = 0b11011011;
        PIR0bits.TMR0IF = 0; // clear interrupt flag
    }
    
     
}
