#include "stm32f10x.h"                  // Device header
#include "Delay.h"

/**
  * ��    ����������ʼ��
  * ��    ������
  * �� �� ֵ����
  */
void Key_Init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOA, ENABLE);  // ����GPIOB��GPIOC��ʱ��

    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    
    // ��ʼ��GPIOB��PB0��PB1Ϊ��������
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1|GPIO_Pin_4 ;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    
    // ��ʼ��GPIOC��PC8��PC9Ϊ��������
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_15|GPIO_Pin_4 | GPIO_Pin_5|GPIO_Pin_6;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
}

/**
  * ��    ����������ȡ����
  * ��    ������
  * �� �� ֵ�����°����ļ���ֵ����Χ��0~2������0����û�а�������
  * ע������˺���������ʽ��������������ס����ʱ�������Ῠס��ֱ����������
  */
extern uint8_t KeyNum ;
uint8_t Key_GetNum(void)
{

	if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_0) == 0)
	{
		Delay_ms(20);
		while (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_0) == 0)
			;
		Delay_ms(20);
		KeyNum = 1;
	}
	else if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_1) == 0)
	{
		Delay_ms(20);
		while (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_1) == 0)
			;
		Delay_ms(20);
		KeyNum = 2;
	}
	else if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_4) == 0)
	{
		Delay_ms(20);
		while (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_4) == 0)
			;
		Delay_ms(20);
		KeyNum = 3;
	}
	else if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_5) == 0)
	{
		Delay_ms(20);
		while (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_5) == 0)
			;
		Delay_ms(20);
		KeyNum = 4;
	}
else if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_4) == 0)
	{
		Delay_ms(20);
		while (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_4) == 0)
			;
		Delay_ms(20);
		KeyNum = 5;
	}
else if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_6) == 0)
	{
		Delay_ms(20);
		while (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_6) == 0)
			;
		Delay_ms(20);
		KeyNum = 6;
	}
else if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_12) == 0)
	{
		Delay_ms(20);
		while (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_12) == 0)
			;
		Delay_ms(20);
		KeyNum = 7;
	}
else if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_15) == 0)
	{
		Delay_ms(20);
		while (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_15) == 0)
			;
		Delay_ms(20);
		KeyNum = 8;
	}

	return KeyNum;
}
