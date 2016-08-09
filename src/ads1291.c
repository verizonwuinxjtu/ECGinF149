#include "ads1291.h"

uchar SPI_Tx_buf[3];
uchar Reg_State[12];
const uchar Reg_Setting[12]={
		0x50,		//Device ID (READONLY)
		0x02,		//CONFIG1 02(Default)
		0xA0,		//CONFIG2 80(Default)
		0x10,		//LOFF 10(Default)
		0x60,		//CH1SET (PGA gain = 6) 00(Default)
		0x81,		//CH2SET 00(Default)
		0x00,		//RLD_SENS 00(Default)
		0x00,		//LOFF_SENS 00(Default)
		0x1F,		//LOFF_STAT 00(Default)
		0x02,		//MISC1 02(Default&Necessary)
		0x00,		//MISC2 00(Default)
		0x0C 		//GPIO 0C
};
/*
 * 管脚注明
 * P4.2 CLKSEL
 * P4.1 /RESET
 * P4.0 START
 * P3.7 TXD
 * P3.6 RXD
 * P3.4 CS
 * P3.3 SCLK
 * P3.2 SOMI
 * P3.1 SIMO
 * P2.5 DRDY
 */


/*ADS1191 Basic Function*/
//时钟选择
void Clock_Select(uchar CLKSEL)
{
	if(CLKSEL==1)
		P4OUT |= BIT2;
	else	P4OUT &= ~BIT2;
}

//芯片重置
void Chip_Reset(void)
{
	P4OUT |= BIT1;				// Set High
	Delay_ms(1);
	P4OUT &= ~BIT1;				// Set to low
	Delay_ms(1);
	P4OUT |= BIT1;				// Set High
	Delay_ms(7);
}
//片选使能函数
void Set_Chip_Enable(void)
{
	P3OUT &= ~BIT4;
}
//片选清除使能函数
void Clear_Chip_Enable(void)
{
	Delay_us(20);
	P3OUT |= BIT4;
}

//硬件ADC转换操作使能函数
void Enable_Start(void)
{
	P4OUT |= BIT0;
	Delay_ms(10);

}
//硬件ADC转换操作清除使能函数
void Disable_Start(void)
{
	P4OUT &= ~BIT0;
	Delay_ms(7);
}

/*ADS1191 控制字设置*/
//发送命令函数

void Send_Command_Data(uchar data)
{
	Set_Chip_Enable();
	Delay_us(1);
	Clear_Chip_Enable();
	Set_Chip_Enable();

	U0TXBUF = data;

	while (!(IFG1 & UTXIFG0));
        Delay_us(10);
	U0RXBUF;
	Delay_us(30);
}
//唤醒ADS1191
void Wake_Up_ADS1191(void){
	Send_Command_Data (WAKEUP);
}
//睡眠ADS1191
void Put_ADS1191_In_Sleep(void){
	Send_Command_Data (STANDBY);
}
//软件重置ADS1191
void Soft_Reset_Chip(void){
	Send_Command_Data(RESET);
}
////软件开始
//void Soft_Start_ReStart_ADS1191(void)
//{
//	Send_Command_Data(START);
//	Clear_Chip_Eable();
//}
//发送START命令
void Start_Data_Conv_Command(void){
	Send_Command_Data(START);
}
//发送SDATAC
void Stop_Read_Data_Continuous(void){
	Send_Command_Data(SDATAC);
}
//发送RDATAC
void Start_Read_Data_Continuous(void){
	Send_Command_Data(RDATAC);
}
//
void Read_Data (void)
{
	Send_Command_Data(RDATA);                   // Send 0x12 to the ADS1x9x
}
//软件停止转换
void Soft_Stop_Chip(void){
	Send_Command_Data(STOP);
}
// 硬件停止
void Hard_Stop_Chip (void)
  {
    P4OUT &= ~BIT0;		// Set Start pin to Low
    Delay_ms(14);
  }
/*读寄存器函数 */
uchar Reg_Read(uchar ADDRESS)
{
	uchar retVal=0;

	SPI_Tx_buf[0] = ADDRESS | RREG;
	SPI_Tx_buf[1] = 0;
	Set_Chip_Enable();

	U0TXBUF = SPI_Tx_buf[0];                  // Send the first data to the TX Buffer
	while (!(IFG1 & UTXIFG0));				// USCI_B0 TX buffer ready?
        Delay_us(10);
	U0TXBUF = SPI_Tx_buf[1];                  // Send the first data to the TX Buffer
	while (!(IFG1 & UTXIFG0));				// USCI_B0 TX buffer ready?
        Delay_us(10);
	U0RXBUF;						         	// Read RX buff
	U0TXBUF = 0x00;                           // Send the first data to the TX Buffer
	while (!(IFG1 & UTXIFG0));				// USCI_B0 TX buffer ready?
        Delay_us(10);
	retVal = U0RXBUF;							// Read RX buff

	Clear_Chip_Enable();
	return retVal;
}
//读所有寄存器函数
void Read_All_Reg(void)
{
	uchar reg_i;
	Set_Chip_Enable();
	Delay_us(40);
	Clear_Chip_Enable();

	for(reg_i=0;reg_i<12;reg_i++)
	{
		Reg_State[reg_i]=Reg_Read(reg_i);
	}

}

//写寄存器函数
void Reg_Write(uchar ADDRESS, uchar DATA)
{
  
    switch(ADDRESS)
    {
            case 0:	DATA = DATA & 0xE3;
                            DATA = DATA | 0x10;	break;
            case 1:	DATA = DATA & 0x87;	break;
            case 2:	DATA = DATA & 0xFB;
                            DATA |= 0x80;	break;
            case 3:	DATA = DATA & 0xFD;
                            DATA |= 0x10;	break;
            case 7:	DATA = DATA & 0x3F;	break;
            case 8:	DATA = DATA & 0xAF;	break;
            case 9:	DATA |= 0x02;	break;
            case 10:	DATA = DATA & 0x87;
                                    DATA |= 0x01;	break;
            case 11:	DATA = DATA & 0x0F;	break;

            default:	break;		
    }	//对不可更改位进行校正

    SPI_Tx_buf[0] = ADDRESS | WREG;
    SPI_Tx_buf[1] = 0;						// Write Single byte
    SPI_Tx_buf[2] = DATA;					// Write Single byte
    Set_Chip_Enable();
    Delay_us(20);

    U0TXBUF = SPI_Tx_buf[0];              // Send the first data to the TX Buffer
    while (!(IFG1 & UTXIFG0));			// USCI_B0 TX buffer ready?
    Delay_us(10);
    U0RXBUF;							// Read Rx buf
    U0TXBUF = SPI_Tx_buf[1];              // Send the first data to the TX Buffer
    while (!(IFG1 & UTXIFG0));			// USCI_B0 TX buffer ready?
    Delay_us(10);
    U0RXBUF;
    U0TXBUF = SPI_Tx_buf[2];              // Send the first data to the TX Buffer
    while (!(IFG1 & UTXIFG0));			// USCI_B0 TX buffer ready?
    Delay_us(10);
    U0RXBUF;
    Clear_Chip_Enable();

}
// 初始化寄存器
void Reg_Init(void)
{
	uchar Reg_Init_i;
	Set_Chip_Enable();
	Delay_us(20);
	Clear_Chip_Enable();


	for ( Reg_Init_i = 1; Reg_Init_i < 12; Reg_Init_i++)
	{
		Reg_Write(Reg_Init_i,Reg_Setting[Reg_Init_i]);
	}
}
//初始化I/O口中断
void Init_DRDY_Interrupt(void)
{
	P2IES |= BIT5;
	P2IFG &= ~BIT5;
	P2IE &= ~BIT5;
}
//使能I/O中断
void Enable_DRDY_Interrupt(void)
{
	P2IFG &= ~BIT5;
	P2IE |= BIT5;
}
//
void Disable_DRDY_Interrupt(void)
{
	P2IFG &= ~BIT5;
	P2IE &= ~BIT5;
}
//初始化ADS1191
void ADS1291_Init(void)
{
	Chip_Reset();
	Delay_ms(24);
        
	Clock_Select(1);
	Delay_ms(12);

	Disable_Start();
	Enable_Start();

	Hard_Stop_Chip();

	Start_Data_Conv_Command();
	Soft_Stop_Chip();
	Delay_ms(8);

	Stop_Read_Data_Continuous();
	Delay_ms(28);

	Read_All_Reg();
	Reg_Init();
	Read_All_Reg();
        
    Init_DRDY_Interrupt();
	Enable_DRDY_Interrupt();
}
