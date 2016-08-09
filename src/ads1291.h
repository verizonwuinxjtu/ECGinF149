#ifndef ADS1291_H_
#define ADS1291_H_

#include "global.h"
#include <msp430x14x.h>
/****************************************************************/
/* Delay Function Definition */
/****************************************************************/

#define CPU_CLOCK       8000000
#define Delay_us(us)    __delay_cycles(CPU_CLOCK/1000000*(us))
#define Delay_ms(ms)    __delay_cycles(CPU_CLOCK/1000*(ms))

/****************************************************************/
/* Global functions*/
/****************************************************************/
 void Clock_Select(uchar CLKSEL);
 void ADS1191_Init(void);					//Initial ADS1X9X
 void Set_Chip_Enable(void);				//Set CS to Low
 void Clear_Chip_Enable(void);			//Set CS to High
 void Chip_Reset(void);					//Set POVERDOWN/RESET pin to Low
 void Disable_Start(void);				//Set START pin to Low
 void Enable_Start(void);					//Set START pin to High
 void Send_Command_Data(uchar data);	//Function to send command
 void Wake_Up_ADS1191 (void);				//WAKEUP
 void Put_ADS1191_In_Sleep (void);		//STANDBY
 void Soft_Reset_Chip (void);				//RESET
// void Soft_Start_ReStart_ADS1191 (void);	//START and Stop SPI communication
 void Start_Data_Conv_Command (void);		//START
 void Stop_Read_Data_Continuous (void);	//SDATAC
 void Start_Read_Data_Continuous (void);	//RDATAC
 void Soft_Stop_Chip (void);				//STOP
 void Read_Data (void);					//RDATA
 void ADS1291_Init(void);
 void Init_DRDY_Interrupt(void);
 void Enable_DRDY_Interrupt(void);
 void Disable_DRDY_Interrupt(void);
 void Reg_Init(void);
 

 
 /****************************************************************/
/* ADS1x9x COMMAND DESCRIPTION and definations */
/****************************************************************/
 // System Commands
 #define WAKEUP		0x02		//Wake-up from standby mode
 #define STANDBY	0x04		//Enter standby mode
 #define RESET		0x06		//Reset the device
 #define START		0x08		//Start/restart (synchronize) conversions
 #define STOP		0x0A		//Stop conversion

// Data Read Commands
 #define RDATAC		0x10		//Enable Read Data Continuous mode.
 								//This mode is the default mode at power-up.
 #define SDATAC		0x11		//Stop Read Data Continuously mode
 #define RDATA		0x12		//Read data by command; supports multiple read back.

 // Register Read Commands
 #define RREG		0x20		//Read n nnnn registers starting at address r rrrr
 								//first byte 001r rrrr (2xh)(2) - second byte 000n nnnn(2)
 #define WREG		0x40		//Write n nnnn registers starting at address r rrrr
								//first byte 010r rrrr (2xh)(2) - second byte 000n nnnn(2)


#endif
