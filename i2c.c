#include <xc.h>
#include "i2c.h"

void I2C_2_Master_Init(void)
{
  //i2c config  
  SSP2CON1bits.SSPM= 0b1000;    // i2c master mode
  SSP2CON1bits.SSPEN = 1;       //enable i2c
  SSP2ADD = (_XTAL_FREQ/(4*_I2C_CLOCK))-1; //Baud rate divider bits (in master mode)
  
  //pin configuration for i2c  
  TRISDbits.TRISD5 = 1;                   //Disable output driver
  TRISDbits.TRISD6 = 1;                   //Disable output driver
  ANSELDbits.ANSELD5=0;
  ANSELDbits.ANSELD6=0;
  SSP2DATPPS=0x1D;      //pin RD5
  SSP2CLKPPS=0x1E;      //pin RD6
  RD5PPS=0x1C;      // data output
  RD6PPS=0x1B;      //clock output
}

void I2C_2_Master_Idle(void)
{
  //This function waits until the I2C bus is idle by checking the status bits in the SSP2STAT and SSP2CON2 registers. 
  //When both bits are clear (i.e., the bus is idle), the function returns.
    
  while ((SSP2STAT & 0x04) || (SSP2CON2 & 0x1F)); // wait until bus is idle
}

void I2C_2_Master_Start(void)
{
  //This function initiates a start condition on the I2C bus by setting the SEN bit in the SSP2CON2 register to 1. 
  //Before doing so, it first checks if the bus is idle by calling the I2C_2_Master_Idle() function.  
    
  I2C_2_Master_Idle();    
  SSP2CON2bits.SEN = 1;             //Initiate start condition
}

void I2C_2_Master_RepStart(void)
{
  //This function initiates a repeated start condition on the I2C bus by setting the RSEN bit in the SSP2CON2 register to 1. 
  //Before doing so, it first checks if the bus is idle by calling the I2C_2_Master_Idle() function.
    
  I2C_2_Master_Idle();
  SSP2CON2bits.RSEN = 1;           //Initiate repeated start condition, use when having multiple slave devices 
}

void I2C_2_Master_Stop()
{
  //This function initiates a stop condition on the I2C bus by setting the PEN bit in the SSP2CON2 register to 1. 
  //Before doing so, it first checks if the bus is idle by calling the I2C_2_Master_Idle() function.
  
  I2C_2_Master_Idle();
  SSP2CON2bits.PEN = 1;           //Initiate stop condition
}

void I2C_2_Master_Write(unsigned char data_byte)
{
  //This function writes a single byte of data to the I2C bus by storing it in the SSP2BUF register. 
  //Before doing so, it first checks if the bus is idle by calling the I2C_2_Master_Idle() function
    
  I2C_2_Master_Idle();
  SSP2BUF = data_byte;         //Write data to SSPBUF
}

unsigned char I2C_2_Master_Read(unsigned char ack)
{
  //The function takes an argument "ack", which is a boolean value indicating whether or not to send an ACK (acknowledge) bit after reading the data. 
  //If "ack" is true, the function sends an ACK bit to the slave device to indicate that more data will be read. 
  //If "ack" is false, the function sends a NACK (not acknowledge) bit to the slave device to indicate that this is the last data to be read.
  unsigned char tmp;
  I2C_2_Master_Idle();
  SSP2CON2bits.RCEN = 1;        // put the module into receive mode
  I2C_2_Master_Idle();
  tmp = SSP2BUF;                //Read data from SS2PBUF
  I2C_2_Master_Idle();
  SSP2CON2bits.ACKDT = !ack;     // 0 turns on acknowledge data bit
  SSP2CON2bits.ACKEN = 1;        //start acknowledge sequence
  return tmp;
}
