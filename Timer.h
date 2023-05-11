#ifndef _Timer_H
#define _Timer_H

#include <xc.h>

#define _XTAL_FREQ 64000000



void Timer0_init(void);
unsigned int get16bitTMR0val(void);
void __interrupt(low_priority) LowISR(); //add in this addtional function

#endif
