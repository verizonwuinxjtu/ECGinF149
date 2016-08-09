#ifndef GLOBAL_H_
#define GLOBAL_H_
/*
 * 必要头文件包含
 */
#include <msp430x14x.h>
/*
 * 延时
 */
#define CPU_CLOCK       8000000
#define Delay_us(us)    __delay_cycles(CPU_CLOCK/1000000*(us))
#define Delay_ms(ms)    __delay_cycles(CPU_CLOCK/1000*(ms))

/*
 * 变量类型简化
 */
#define uchar		unsigned char
#define uint		unsigned int

#endif /* SRC_GLOBAL_H_ */
