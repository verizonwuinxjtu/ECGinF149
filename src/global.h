#ifndef GLOBAL_H_
#define GLOBAL_H_
/*
 * ��Ҫͷ�ļ�����
 */
#include <msp430x14x.h>
/*
 * ��ʱ
 */
#define CPU_CLOCK       8000000
#define Delay_us(us)    __delay_cycles(CPU_CLOCK/1000000*(us))
#define Delay_ms(ms)    __delay_cycles(CPU_CLOCK/1000*(ms))

/*
 * �������ͼ�
 */
#define uchar		unsigned char
#define uint		unsigned int

#endif /* SRC_GLOBAL_H_ */
