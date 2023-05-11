#include <xc.h>
#include "color.h"
#include "i2c.h"


/************************************
 *Function to initialise color click
************************************/
void color_click_init(void)
{   
    //setup colour sensor via i2c interface
    I2C_2_Master_Init();      //Initialise i2c Master

     //set device PON
	 color_writetoaddr(0x00, 0x01);
    __delay_ms(3); //need to wait 3ms for everthing to start up
    
    //turn on device ADC
	color_writetoaddr(0x00, 0x03);

    //set integration time
	color_writetoaddr(0x01, 0xD5);
    
    //turn on LEDs
    TRISGbits.TRISG0 = 0; 
    TRISEbits.TRISE7 = 0; 
    TRISAbits.TRISA3 = 0; 
    //turn on all 3 leds to get white light... why are these registers different from previous years?
    LATGbits.LATG0 = 1; //red led //G1 G0
    LATEbits.LATE7 = 1; //blue led //F7 E7
    LATAbits.LATA3 = 1; //green led //A4 A3
}

/************************************
 *Function to write to address
************************************/
void color_writetoaddr(char address, char value){
    I2C_2_Master_Start();         //Start condition
    I2C_2_Master_Write(0x52 | 0x00);     //7 bit device address + Write mode
    I2C_2_Master_Write(0x80 | address);    //command + register address
    I2C_2_Master_Write(value);    
    I2C_2_Master_Stop();          //Stop condition
}

/************************************
 *Function to read color red
************************************/
unsigned int color_read_Red(void)
{
	unsigned int tmp;
	I2C_2_Master_Start();         //Start condition
	I2C_2_Master_Write(0x52 | 0x00);     //7 bit address + Write mode
	I2C_2_Master_Write(0xA0 | 0x16);    //command (auto-increment protocol transaction) + start at RED low register
	I2C_2_Master_RepStart();			// start a repeated transmission
	I2C_2_Master_Write(0x52 | 0x01);     //7 bit address + Read (1) mode
	tmp=I2C_2_Master_Read(1);			//read the Red LSB
	tmp=tmp | (I2C_2_Master_Read(0)<<8); //read the Red MSB (don't acknowledge as this is the last read)
	I2C_2_Master_Stop();          //Stop condition
	return tmp;
}

/************************************
 *Function to read color green
************************************/
unsigned int color_read_Green(void)
{
	unsigned int tmp;
	I2C_2_Master_Start();         //Start condition
	I2C_2_Master_Write(0x52 | 0x00);     //7 bit address + Write mode
	I2C_2_Master_Write(0xA0 | 0x18);    //command (auto-increment protocol transaction) + start at RED low register
	I2C_2_Master_RepStart();			// start a repeated transmission
	I2C_2_Master_Write(0x52 | 0x01);     //7 bit address + Read (1) mode
	tmp=I2C_2_Master_Read(1);			//read the Red LSB
	tmp=tmp | (I2C_2_Master_Read(0)<<8); //read the Red MSB (don't acknowledge as this is the last read)
	I2C_2_Master_Stop();          //Stop condition
	return tmp;
}

/************************************
 *Function to read color blue
************************************/
unsigned int color_read_Blue(void)
{
	unsigned int tmp;
	I2C_2_Master_Start();         //Start condition
	I2C_2_Master_Write(0x52 | 0x00);     //7 bit address + Write mode
	I2C_2_Master_Write(0xA0 | 0x1A);    //command (auto-increment protocol transaction) + start at RED low register
	I2C_2_Master_RepStart();			// start a repeated transmission
	I2C_2_Master_Write(0x52 | 0x01);     //7 bit address + Read (1) mode
	tmp=I2C_2_Master_Read(1);			//read the Red LSB
	tmp=tmp | (I2C_2_Master_Read(0)<<8); //read the Red MSB (don't acknowledge as this is the last read)
	I2C_2_Master_Stop();          //Stop condition
	return tmp;
}

/************************************
 *Function to read color clear
************************************/
unsigned int color_read_Clear(void)
{
	unsigned int tmp;
	I2C_2_Master_Start();         //Start condition
	I2C_2_Master_Write(0x52 | 0x00);     //7 bit address + Write mode
	I2C_2_Master_Write(0xA0 | 0x14);    //command (auto-increment protocol transaction) + start at L low register
	I2C_2_Master_RepStart();			// start a repeated transmission
	I2C_2_Master_Write(0x52 | 0x01);     //7 bit address + Read (1) mode
	tmp=I2C_2_Master_Read(1);			//read the L LSB
	tmp=tmp | (I2C_2_Master_Read(0)<<8); //read the L MSB (don't acknowledge as this is the last read)
	I2C_2_Master_Stop();          //Stop condition
	return tmp;
}

/************************************
 * Function to store above color_read values into struct
************************************/
void color_read(struct RGBC_val *rgb)
{
    rgb->R = color_read_Red();
    rgb->G = color_read_Green();
    rgb->B = color_read_Blue();
    rgb->C = color_read_Clear();
}

/************************************
 *Function to calculate the relative rgb values and update it inside a given struct
************************************/
void update_rel_rgb(struct RGBC_val *rgbc_val, struct Rel_RGB *rel_rgb) {
    // Calculate the ratio of each color to the clear value
    float ratio_r = (float) rgbc_val->R / rgbc_val->C;
    float ratio_g = (float) rgbc_val->G / rgbc_val->C;
    float ratio_b = (float) rgbc_val->B / rgbc_val->C;

    // Normalize the ratios to 255 and store in the rel_rgb struct
    rel_rgb->relR = (unsigned int) (ratio_r * 100);
    rel_rgb->relG = (unsigned int) (ratio_g * 100);
    rel_rgb->relB = (unsigned int) (ratio_b * 100);
}

/************************************
 * Function that takes in relative RGB values and outputs associated colour code
************************************/
int color_check(struct Rel_RGB *rel_rgb) {
    if (rel_rgb->relR >= 81 && rel_rgb->relR <= 83 && rel_rgb->relG >= 10 && rel_rgb->relG <= 12 && rel_rgb->relB >= 13 && rel_rgb->relB <= 15) {
        return 1; //1 for Red
    }
    else if (rel_rgb->relR >= 36 && rel_rgb->relR <= 38 && rel_rgb->relG >= 43 && rel_rgb->relG <= 45 && rel_rgb->relB >= 20 && rel_rgb->relB <= 22) {
        return 2; //2 for Green
    }
    else if (rel_rgb->relR >= 32 && rel_rgb->relR <= 34 && rel_rgb->relG >= 33 && rel_rgb->relG <= 35 && rel_rgb->relB >= 32 && rel_rgb->relB <= 34) {
        return 3; //3 for Blue
    }
    else if (rel_rgb->relR >= 52 && rel_rgb->relR <= 54 && rel_rgb->relG >= 31 && rel_rgb->relG <= 33 && rel_rgb->relB >= 17 && rel_rgb->relB <= 19) {
        return 4; //4 for yellow
    }
    else if (rel_rgb->relR >= 49 && rel_rgb->relR <= 51 && rel_rgb->relG >= 29 && rel_rgb->relG <= 31 && rel_rgb->relB >= 22 && rel_rgb->relB <= 24) {
        return 5; //5 for pink
    }
    else if (rel_rgb->relR >= 60 && rel_rgb->relR <= 62 && rel_rgb->relG >= 23 && rel_rgb->relG <= 25 && rel_rgb->relB >= 17 && rel_rgb->relB <= 19) {
        return 6; //6 for orange
    }
    else if (rel_rgb->relR >= 32 && rel_rgb->relR <= 35 && rel_rgb->relG >= 40 && rel_rgb->relG <= 42 && rel_rgb->relB >= 27 && rel_rgb->relB <= 29) {
        return 7; //7 for light blue
    }
    else if (rel_rgb->relR >= 44 && rel_rgb->relR <= 46 && rel_rgb->relG >= 33 && rel_rgb->relG <= 35 && rel_rgb->relB >= 23 && rel_rgb->relB <= 25) {
        return 8; //8 for white
    }
    else if (rel_rgb->relR == 48 && rel_rgb->relG == 28 && rel_rgb->relB == 20) {
        return 9; //9 for black wall
    }
    else {
        return 0; // Function returns 0 if no known colour is sensed
    }
}

