#ifndef __USART3_H__
#define __USART3_H__

#include "stm32f10x.h"

void uart3_init(u32 bound); // 串口3初始化
void u3_sendData(u8 *str);  // 串口3发送数据  当作为从机时可以不使用

extern u8 data1; // 三个有效数据位
extern u8 data2;
extern u8 data3;
extern u8 data4;
#endif
