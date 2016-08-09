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
 * �ܽ�ע��
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
//ʱ��ѡ��
void Clock_Select(uchar CLKSEL)
{
	if(CLKSEL==1)
		P4OUT |= BIT2;
	else	P4OUT &= ~BIT2;
}

//оƬ����
void Chip_Reset(void)
{
	P4OUT |= BIT1;				// Set High
	Delay_ms(1);
	P4OUT &= ~BIT1;				// Set to low
	Delay_ms(1);
	P4OUT |= BIT1;				// Set High
	Delay_ms(7);
}
//Ƭѡʹ�ܺ���
void Set_Chip_Enable(void)
{
	P3OUT &= ~BIT4;
}
//Ƭѡ���ʹ�ܺ���
void Clear_Chip_Enable(void)
{
	Delay_us(20);
	P3OUT |= BIT4;
}

//Ӳ��ADCת������ʹ�ܺ���
void Enable_Start(void)
{
	P4OUT |= BIT0;
	Delay_ms(10);

}
//Ӳ��ADCת���������ʹ�ܺ���
void Disable_Start(void)
{
	P4OUT &= ~BIT0;
	Delay_ms(7);
}

/*ADS1191 ����������*/
//���������

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
//����ADS1191
void Wake_Up_ADS1191(void){
	Send_Command_Data (WAKEUP);
}
//˯��ADS1191
void Put_ADS1191_In_Sleep(void){
	Send_Command_Data (STANDBY);
}
//�������ADS1191
void Soft_Reset_Chip(void){
	Send_Command_Data(RESET);
}
////�����ʼ
//void Soft_Start_ReStart_ADS1191(void)
//{
//	Send_Command_Data(START);
//	Clear_Chip_Eable();
//}
//����START����
void Start_Data_Conv_Command(void){
	Send_Command_Data(START);
}
//����SDATAC
void Stop_Read_Data_Continuous(void){
	Send_Command_Data(SDATAC);
}
//����RDATAC
void Start_Read_Data_Continuous(void){
	Send_Command_Data(RDATAC);
}
//
void Read_Data (void)
{
	Send_Command_Data(RDATA);                   // Send 0x12 to the ADS1x9x
}
//���ֹͣת��
void Soft_Stop_Chip(void){
	Send_Command_Data(STOP);
}
// Ӳ��ֹͣ
void Hard_Stop_Chip (void)
  {
    P4OUT &= ~BIT0;		// Set Start pin to Low
    Delay_ms(14);
  }
/*���Ĵ������� */
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
//�����мĴ�������
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

//д�Ĵ�������
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
    }	//�Բ��ɸ���λ����У��

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
// ��ʼ���Ĵ���
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
//��ʼ��I/O���ж�
void Init_DRDY_Interrupt(void)
{
	P2IES |= BIT5;
	P2IFG &= ~BIT5;
	P2IE &= ~BIT5;
}
//ʹ��I/O�ж�
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
//��ʼ��ADS1191
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
