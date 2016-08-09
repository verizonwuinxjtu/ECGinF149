#include "oled.h"
#include "oled_font.h"
void I2C_Start(void)
{
	SCL_H;
	SDA_H;
	SDA_L;
	SCL_L;
}
void I2C_Stop(void)
{
	SCL_L;
	SDA_L;
	SCL_H;
	SDA_H;
}

void I2C_SendByte(uchar byte)
{
	uchar i = 0;
	for(i=0;i<8;i++)
	{
          SCL_L;
          if((byte<<i)&0x80)
          {
        	  SDA_H;
          }
          else
          {
        	  SDA_L;
          }
          SCL_H;
	}
	SCL_L;
	SCL_H;
}

void OLED_SendCommand(uchar cmd)
{
	I2C_Start();
	I2C_SendByte(0x78);
	I2C_SendByte(0x00);
	I2C_SendByte(cmd);
	I2C_Stop();
}

void OLED_SendData(uchar data)
{
	I2C_Start();
	I2C_SendByte(0x78);
	I2C_SendByte(0x40);
	I2C_SendByte(data);
	I2C_Stop();
}

void OLED_SetPos(uchar x, uchar y)
{
	OLED_SendCommand(0xb0 + y);
	OLED_SendCommand(((x & 0xf0) >> 4) | 0x10);
	OLED_SendCommand(x & 0x0f);
}

void OLED_DisplayOn(void)
{
	OLED_SendCommand(0x8D);
	OLED_SendCommand(0x14);
	OLED_SendCommand(0xAF);
}
void OLED_DisplayOff(void)
{
	OLED_SendCommand(0x8D);
	OLED_SendCommand(0x14);
	OLED_SendCommand(0xAE);
}
void OLED_Clear(void)
{
	uchar i,j;
	for(i = 0; i < 4; i++)
	{
		OLED_SendCommand(0xb0+i);
		OLED_SendCommand(0x00);
		OLED_SendCommand(0x12);
		for(j = 0; j < 128; j++)
		{
			OLED_SendData(0x00);
		}
	}
}
void OLED_Fill(void)
{
	uchar j;
	OLED_SendCommand(0xb0);
	OLED_SendCommand(0x00);
	OLED_SendCommand(0x12);
	for(j = 32; j < 128; j++)
	{
		OLED_SendData(1);
	}
}
void OLED_ShowChar(uchar x, uchar y, uchar ch, uchar ch_size)
{
	uchar i;
	ch -= ' ';
	if(16 == ch_size)
	{
		OLED_SetPos(x,y);
		for(i = 0; i < 8; i++)
		{
			OLED_SendData(F8X16[ch*16+i]);

		}
		OLED_SetPos(x,(y+1)%2);
		for(i = 0; i < 8; i++)
		{
			OLED_SendData(F8X16[ch*16+8+i]);
		}
	}
	else
	{
		OLED_SetPos(x,y);
		for(i=0;i<6;i++)
		{
			OLED_SendData(F6x8[ch*6+i]);
		}
	}
}

void OLED_ShowNum(uchar x, uchar y, uint num, uchar ch_size)
{
	uchar temp;
	temp = num / 100;
	if(temp<10)
	{
		OLED_ShowChar(x,y,temp+'0',ch_size);
	}
	else
	{
		OLED_ShowChar(x,y,'?',ch_size);
	}
	temp = (num % 100) / 10;
	OLED_ShowChar(x+ch_size/2,y,temp+'0',ch_size);
	temp = num % 10;
	OLED_ShowChar(x+ch_size,y,temp+'0',ch_size);
}

void OLED_ShowString(uchar x, uchar y, const uchar *str, uchar ch_size)
{
	uchar i = 0;
	while(str[i]!='\0')
	{
		OLED_ShowChar(x,y,str[i],ch_size);
		x += ch_size/2;
		i++;
	}
}

void OLED_Init(void)
{
	Delay_ms(100);
	OLED_SendCommand(0xAE);//--display off
	OLED_SendCommand(0x00);//---set low column address
	OLED_SendCommand(0x10);//---set high column address
	OLED_SendCommand(0x40);//--set start line address
	OLED_SendCommand(0xB0);//--set page address
	OLED_SendCommand(0x81); // contract control
	OLED_SendCommand(0xFF);//--128
	OLED_SendCommand(0xA1);//set segment remap
	OLED_SendCommand(0xA6);//--normal / reverse
	OLED_SendCommand(0xA8);//--set multiplex ratio(1 to 64)
	OLED_SendCommand(0x0F);//--1/16 duty
	OLED_SendCommand(0xC8);//Com scan direction
	OLED_SendCommand(0xD3);//-set display offset
	OLED_SendCommand(0x00);//
	OLED_SendCommand(0xD5);//set osc division
	OLED_SendCommand(0x80);//
	OLED_SendCommand(0xD9);//set pre-charge period
	OLED_SendCommand(0x22);//
	OLED_SendCommand(0xDA);//set COM pins
	OLED_SendCommand(0x02);//
	OLED_SendCommand(0xDB);//set vcomh
	OLED_SendCommand(0x40);//
	OLED_SendCommand(0x8D);//set charge pump enable
	OLED_SendCommand(0x14);//
	OLED_SendCommand(0xAF);//--turn on oled panel
}
