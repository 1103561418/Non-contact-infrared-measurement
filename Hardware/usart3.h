#ifndef __USART3_H__
#define __USART3_H__

#include "stm32f10x.h"

void uart3_init(u32 bound); // ����3��ʼ��
void u3_sendData(u8 *str);  // ����3��������  ����Ϊ�ӻ�ʱ���Բ�ʹ��

extern u8 data1; // ������Ч����λ
extern u8 data2;
extern u8 data3;
extern u8 data4;
#endif
