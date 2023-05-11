#ifndef _Interrupt_H
#define _Interrupt_H

#include <xc.h>

#define _XTAL_FREQ 64000000
char SecondTrigger = 0;   //Define Second Trigger
void Interrupts_init(void);
void __interrupt() ISR();

#endif