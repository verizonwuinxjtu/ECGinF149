#ifndef OLED_H_
#define OLED_H_
#include "global.h"

/*
 * I2C 管脚指令宏定义
 */
#define SCL_H			P1OUT |= BIT2
#define SCL_L			P1OUT &= ~BIT2

#define SDA_IN			P1DIR &= ~BIT3
#define SDA_OUT			P1DIR |= BIT3

#define SDA_H			P1OUT |= BIT3
#define SDA_L			P1OUT &= ~BIT3

/*
 * 软件模拟I2C基础函数
 */
void I2C_Start(void);
void I2C_Stop(void);
void I2C_Init(void);
void I2C_SendByte(uchar byte);

/*
 * OLED 基础函数定义
 */
void OLED_SendCommand(uchar cmd);
void OLED_SendData(uchar data);

void OLED_SetPos(uchar x, uchar y);

void OLED_DisplayOn(void);
void OLED_DisplayOff(void);

void OLED_Clear(void);
void OLED_Fill(void);

void OLED_ShowChar(uchar x, uchar y, uchar ch, uchar ch_size);
void OLED_ShowNum(uchar x, uchar y, uint num, uchar ch_size);
void OLED_ShowString(uchar x, uchar y, const uchar *str, uchar ch_size);
void OLED_Init(void);

#endif /* OLED_H_ */
