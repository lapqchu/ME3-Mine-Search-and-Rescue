#ifndef _color_H
#define _color_H

#include <xc.h>

#define _XTAL_FREQ 64000000 //note intrinsic _delay function is 62.5ns at 64,000,000Hz  


/********************************************//**
 *  Function to initialise the colour click module using I2C
 ***********************************************/
void color_click_init(void);

/********************************************//**
 *  Function to write to the colour click module
 *  address is the register within the colour click to write to
 *	value is the value that will be written to that address
 ***********************************************/
void color_writetoaddr(char address, char value);

/********************************************//**
 *  Function to read the red, green, blue and clear channel
 *	Returns a 16 bit ADC value representing colour intensity
 ***********************************************/
unsigned int color_read_Red(void);
unsigned int color_read_Green(void);
unsigned int color_read_Blue(void);
unsigned int color_read_Clear(void);


//defining a struct to hold RGC values and intensity (L)
struct RGBC_val {
	unsigned int R;
	unsigned int G;
	unsigned int B;
    unsigned int C;
};

struct Rel_RGB {
	unsigned int relR;
	unsigned int relG;
	unsigned int relB;
};

//function to store above color_read values into struct
void color_read(struct RGBC_val *rgb);

void update_rel_rgb(struct RGBC_val *rgbc_val, struct Rel_RGB *rel_rgb);

int color_check(struct Rel_RGB *rel_rgb);

int color_inverse (int color_code);

#endif