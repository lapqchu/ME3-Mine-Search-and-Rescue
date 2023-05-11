// CONFIG1L
#pragma config FEXTOSC = HS     // External Oscillator mode Selection bits (HS (crystal oscillator) above 8 MHz; PFM set to high power)
#pragma config RSTOSC = EXTOSC_4PLL// Power-up default value for COSC bits (EXTOSC with 4x PLL, with EXTOSC operating per FEXTOSC bits)

// CONFIG3L
#pragma config WDTCPS = WDTCPS_31// WDT Period Select bits (Divider ratio 1:65536; software control of WDTPS)
#pragma config WDTE = OFF        // WDT operating mode (WDT enabled regardless of sleep)

#include <xc.h>
#include <stdio.h>
#include "color.h"
#include "i2c.h"
#include "dc_motor.h"
#include "serial.h"
#include "Timer.h"
#include "Interrupt.h"

#define _XTAL_FREQ 64000000 //note intrinsic _delay function is 62.5ns at 64,000,000Hz  


void main(void) {
    
/************************************
 *Initialising the color sensor, USART module, timer0 module and interrupt service routines
************************************/
    color_click_init();
    initUSART4();
    Interrupts_init();
    Timer0_init();
    
// Enabling LED H3, this LED blinks every second as a signal that the buggy is in operation
    TRISHbits.TRISH3 = 0;
    LATHbits.LATH3 = 0;
    
// Motor initialisation, setting the PWM, and defining the structures
    initDCmotorsPWM(99);
    DC_motor motorL, motorR;  
    DC_motor * mL = &motorL;
    DC_motor * mR = &motorR; 
    motorL.power=0; 						//zero power to start
    motorL.direction=1; 					//set default motor direction
    motorL.brakemode=1;						// brake mode (slow decay)
    motorL.posDutyHighByte=(unsigned char *)(&CCPR1H);  //store address of CCP1 duty high byte
    motorL.negDutyHighByte=(unsigned char *)(&CCPR2H);  //store address of CCP2 duty high byte
    motorL.PWMperiod=99; 			//store PWMperiod for motor (value of T2PR in this case)
    
    motorR.power=0; 						//zero power to start
    motorR.direction=1; 					//set default motor direction
    motorR.brakemode=1;						// brake mode (slow decay)
    motorR.posDutyHighByte=(unsigned char *)(&CCPR3H);  //store address of CCP1 duty high byte
    motorR.negDutyHighByte=(unsigned char *)(&CCPR4H);  //store address of CCP2 duty high byte
    motorR.PWMperiod=99; 			//store PWMperiod for motor (value of T2PR in this case)
    

    struct RGBC_val val; // Calling the RGBC struct as val
    struct Rel_RGB rel_rgb; // Calling the Normalised RGB struct as rel_rgb
    
    
    while (1){
        if (undo_complete == 0){ // This if statement checks that the UndoAction function is not completed yet
            
            
            /************************************
             * Reads colour sensor values and calculates the normalised RGB values and checks what colour is being read
            ************************************/
            color_read(&val);   // Read color values and store in RGBC_val struct
            update_rel_rgb(&val, &rel_rgb);  // Calculate normalised RGB values and store it in Rel_RGB struct
            int color_code = color_check(&rel_rgb); // Variable color_code stores the current color that is sensed by putting value of rel_rgb through color_check function


            /************************************
             * Print normalised RGB values to realterm for testing and calibration
            ************************************/
//            char buffer[100];  
//            sprintf(buffer, "RelR: %d, RelG: %d, RelB: %d, R: %d, G: %d, B: %d, C: %d, color_code: %d, latest_time: %d\r\n", rel_rgb.relR, rel_rgb.relG, rel_rgb.relB, val.R, val.G, val.B, val.C, color_code);
//            sendStringSerial4(buffer);
//            __delay_ms(50);


             //Communication through serial to debug and understand what's going on with my arrays
            char buffer[100];  
            sprintf(buffer, "time_index: %d, move_index: %d, color_code: %d, latest_time: %d, latest_move: %d\r\n",time_index, move_index, color_code, time_record[time_index], move_record[move_index - 1]);
            sendStringSerial4(buffer);
            __delay_ms(50);

            /************************************
             * Code now checks the current value in color_code, engages motor functions based on color read, stores turns and duration of forward movement
            ************************************/
            if (color_code == 1) { // Checks color_code value
                time_index++; // Increment time counter array index
                PIE0bits.TMR0IE = 0; // Turn of timer interrupt so to stop counting time while buggy is turning
                turnRight90(mL,mR); // Turn based on color_code
                move_record[move_index] = 1; // Append color_code into move record array
                move_index++; // Increment move array index
                PIE0bits.TMR0IE = 1; // Turn timer interrupt back on
            }
            else if (color_code == 2) {
                time_index++;
                PIE0bits.TMR0IE = 0;
                turnLeft90(mL, mR);
                move_record[move_index] = color_code;
                move_index++;
                PIE0bits.TMR0IE = 1;
            }
            else if (color_code == 3) {
                time_index++;
                PIE0bits.TMR0IE = 0;
                turn180(mL, mR);
                move_record[move_index] = color_code;
                move_index++;
                PIE0bits.TMR0IE = 1;
            }
            else if (color_code == 4) {
                time_index++;
                PIE0bits.TMR0IE = 0;
                RevAndRight(mL, mR);
                move_record[move_index] = color_code;
                move_index++;
                PIE0bits.TMR0IE = 1;
            }
            else if (color_code == 5) {
                time_index++;
                PIE0bits.TMR0IE = 0;
                RevAndLeft(mL, mR);
                move_record[move_index] = color_code;
                move_index++;
                PIE0bits.TMR0IE = 1;
            }
            else if (color_code == 6) {
                time_index++;
                PIE0bits.TMR0IE = 0;
                turnRight135(mL, mR);
                move_record[move_index] = color_code;
                move_index++;
                PIE0bits.TMR0IE = 1;
            }
            else if (color_code == 7) {
                time_index++;
                PIE0bits.TMR0IE = 0;
                turnLeft135(mL, mR);
                move_record[move_index] = color_code;
                move_index++;
                PIE0bits.TMR0IE = 1;
            }
            else if (color_code == 0) {
                fullSpeedAhead(mL, mR);
            }
            else if (color_code == 8 || color_code == 9) { // If color_code read is white or black wall, return home by engaging UndoActions Note: add || color_code == 9, to include black (lost function))
                UndoActions(time_record, move_record, mL, mR);
                __delay_ms(500); 
                undo_complete++; // Increment undo_complete variable so stop motor
                }
        }
        else { // If undo_complete != 0, it means UndoActions has been completed and buggy is back home! Stop the motor.
            stop (mL, mR);
        }

    }

}