#include <msp430x14x.h>
#include "src/BoardConfig.h"
#include "src/oled.h"
#include "src/ads1291.h"
#include "src/ecg_processing.h"

uchar BluetoothFlag = 0;
const uchar str[] = "HR: 000 bpm\0";

uchar REC_DATA[6];             //从ADS读出的数据
uchar Data_Index;

uchar TX_DATA[2];
/* for test */

int showdata[256];
uchar index = 0;

/* * * * * */

long baseLine;
uchar baseLineSetFlag = 1;

long RAW_DATA;

//时钟初始化
void BCS_Init(void)
{
  BCSCTL1 &= ~XT2OFF;
  do
  {
    IFG1 &= ~OFIFG; // 清除振荡器失效标志
    Delay_us(20);
  }
  while ((IFG1 & OFIFG) != 0); // 判断XT2是否起振
  BCSCTL2 = SELM_2+SELS; //选择MCLK、SMCLK为XT2
}

//初始化串口1 For 蓝牙（UART模式）
void UART_Init(void)
{
  ME2 |= UTXE1 + URXE1;                     // Enable USART1 TXD/RXD
  UCTL1 |= CHAR;                            // 8-bit character
  UTCTL1 |= SSEL0;                          // UCLK = ACLK  （串口发送寄存器选择辅助时钟）
  UBR01 = 0x03;                             // 32k/9600 - 3.41  （UBR即波特率控制寄存器）
  UBR11 = 0x00;                             //
  UMCTL1 = 0x4A;                            // Modulation 波率调制
  UCTL1 &= ~SWRST;                        // Initialize USART state machine（初始化异步串行通信USART）
  IE2 |= URXIE1;                           // Enable USART1 RX interrupt（使USART1能中断）
}

//初始化串口0 For ADS1291（SPI模式）
void SPI_Init(void)
{
  U0CTL |= CHAR + SYNC + MM +SWRST;
  U0TCTL |= SSEL1 + STC;
  U0BR0 = 0x08;                            // SPICLK = SMCLK/2
  U0BR1 = 0x00;
  U0MCTL = 0x00;
  ME1 |= USPIE0;
  U0CTL &= ~SWRST;
}
//初始化GPIO
void GPIO_Init(void)
{
  // For ADS1291
  P4DIR |= BIT0 + BIT1 + BIT2;
  
  P3DIR |= BIT1 + BIT3 + BIT4;
  P3SEL |= BIT1 + BIT2 + BIT3;
  P4OUT = 0;
  P3OUT = 0;
  
  // For Bluetooth
  //P3DIR |= BIT6;
  P3SEL |= BIT6 + BIT7;

  // For OLED
  P1DIR |= BIT2 + BIT3;
  P1OUT = 0xFF;
}

//**********************************************************************
//--------------------------------读取芯片数据函数-----------------------
void Read_Chip_Data(void)
{
	Enable_DRDY_Interrupt();//给单片机
	Start_Data_Conv_Command();
}
void Read_Chip_Data_Continuous(void)
{
	unsigned int delay;
	Enable_DRDY_Interrupt();
	Set_Chip_Enable();
	Soft_Stop_Chip();
		for(delay=0;delay<35000;delay++);
	Start_Read_Data_Continuous();
	Start_Data_Conv_Command();
}

void Read_Byte(void)
{
	U0TXBUF = 0x00;//to check
	while (!(IFG1 & UTXIFG0));
	Delay_us(10);
	REC_DATA[Data_Index]=U0RXBUF;
	if((REC_DATA[0]&0xF0)==0xC0)
	{
		Data_Index++;
	}
	else Data_Index=0;

}
void main(void) {
    WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer
    BoardConfig(0xf8);
    BCS_Init();
    GPIO_Init();
    UART_Init();
    SPI_Init();
    ADS1291_Init();
    //InitFlag = 1;
    //OLED_DisplayOn();
    OLED_Clear();
    OLED_ShowString(0,0,str,16);
    InitFlag = 1;
    _EINT();
    Read_Chip_Data();
    while(1){
        LPM2;
        Filter_50Hz();
		HeartRateCal();
		showdata[index] = Dataout;
		index++;
		if(BluetoothFlag == 1)
		{
			TX_DATA[0] = Dataout >> 8;
			TX_DATA[1] = Dataout & 0x00ff;
			U1TXBUF = TX_DATA[1];
			while (!(IFG2 & UTXIFG1));
			Delay_us(10);
			U1TXBUF = TX_DATA[0];
			while (!(IFG2 & UTXIFG1));
			Delay_us(10);
		}
    }
}
#pragma vector=PORT2_VECTOR
__interrupt void PORT_2(void)
{
	LPM2_EXIT;
	P2IFG &= ~BIT5;

	Set_Chip_Enable();
	Read_Data();
	Data_Index = 0;
	Read_Byte();
	Read_Byte();
	Read_Byte();
	Read_Byte();
	Read_Byte();
	Read_Byte();

	RAW_DATA = (signed long)REC_DATA[3];
	RAW_DATA = RAW_DATA << 8;
	RAW_DATA += REC_DATA[4];
	RAW_DATA = RAW_DATA << 8;
	RAW_DATA += REC_DATA[5];

	if(1 == baseLineSetFlag)
	{
	  baseLine = RAW_DATA;
	  baseLineSetFlag = 0;
	}

	Datain = (RAW_DATA - baseLine) >> 2;


}

#pragma vector=USART1RX_VECTOR
__interrupt void usart1_rx (void)
{
  uchar command;
  command = U1RXBUF;
  if (command == 'u')
  {
    BluetoothFlag = 1;
  }
  else if(command == 's')
  {
    BluetoothFlag = 0;
  }
}
