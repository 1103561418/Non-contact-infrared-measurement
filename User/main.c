#include "stm32f10x.h"
#include "Delay.h"
#include "LED.h"
#include "Key.h"
#include "Buzzer.h"
#include "OLED.h"
#include "Timer.h"
#include "MLX9604.h"
#include "usart3.h"
#include "SR04.h"
#include "USART.h"
uint8_t rx_buffer[20];
uint8_t KeyNum;
u8 send1[2]={1,0}; // ������Ч����λ
u8 send2[2]={2,0}; // ������Ч����λ
u8 send3[2]={3,0}; // ������Ч����λ
u8 send4[2]={4,0}; // ������Ч����λ
float  tempture=0;
uint16_t MAX_TEMP=36;
int DIS=0;
int Distance_mm;
int main(void)	
	
{
	
	Timer_Init();		//��ʼ����ʱ��
	UART_Init();
	Key_Init();
	OLED_Init();
	float Temperature=10;
	Mlx96014_Configuration();
	Buzzer_Init();
	OLED_ShowString (1,3,"TEMPUTURE");
	uint8_t KeyNum=0;
	uart3_init(115200);
	HC_SR04_Init();		//��ʼ�����������ģ��
	while(1)
	{	
		
		KeyNum = Key_GetNum ();
        oled_new(); 
		
		 if (KeyNum == 1) {
            Delay_ms(200);  // ��ʱ200ms����ȥ����
            if (Key_GetNum() == 1) {  // �ٴ�ȷ�ϰ����Ƿ���Ȼ������
               u3_sendData(send1);
		
                while (Key_GetNum() == 1);  // �ȴ������ͷ�
							  KeyNum =0;
            }
			oled_new();
        }
		
		 if (KeyNum == 2) {
            Delay_ms(200);  // ��ʱ200ms����ȥ����
            if (Key_GetNum() == 2) {  // �ٴ�ȷ�ϰ����Ƿ���Ȼ������
               u3_sendData(send2);
		
                while (Key_GetNum() == 2);  // �ȴ������ͷ�
							  KeyNum =0;
            }
			oled_new();
		}
		 
		
		
		 if (KeyNum ==3)
		{
		u3_sendData(send3);
		while(1)
		{
		
	   UART_START();
		if(rx_buffer[0]==1)
		{
			OLED_ShowString(2,9,"NO");
		}
		else if(rx_buffer[1]==1)
		{
			OLED_ShowString(2,9,"ZWH");
		}
		else if(rx_buffer[1]==1)
		{
			OLED_ShowString(2,9,"KJY");
		}
			
		KeyNum = Key_GetNum ();
			
		 if(KeyNum ==8)
		{
		KeyNum =0;
		break;
		}
		
	 }
  }
		 
		  if (KeyNum ==4)
		{
		u3_sendData(send4);
		while(1)
		{
		    oled_new(); 
	   UART_START();
		if(rx_buffer[2]==1)
		{
			Buzzer_ON ();
		}
		else {Buzzer_OFF();}
		KeyNum = Key_GetNum ();
		 if(KeyNum ==8)
		{
		KeyNum =0;
		break;
		}
		
	 }
		oled_new();
  }
		
		
        // ���µ������ŵİ������� MAX_TEMP
        if (KeyNum == 6) {
            Delay_ms(200);  // ��ʱ200ms����ȥ����
            if (Key_GetNum() == 6) {  // �ٴ�ȷ�ϰ����Ƿ���Ȼ������
                ++MAX_TEMP;
                while (Key_GetNum() == 6);  // �ȴ������ͷ�
							  KeyNum =0;
            }
			oled_new();
        }
        // ���µ������ŵİ������� MAX_TEMP
        else if (KeyNum == 7) {
            Delay_ms(200);  // ��ʱ200ms����ȥ����
            if (Key_GetNum() == 7) {  // �ٴ�ȷ�ϰ����Ƿ���Ȼ������
                MAX_TEMP--;
                while (Key_GetNum() == 7);  // �ȴ������ͷ�
							  KeyNum=0;
            }
					oled_new();}		
		
		 
		 if (KeyNum ==5)
		{HC_SR04_Init();	
		while(1)
		{
			oled_new(); 
			Distance_mm=sonar_mm();
			tempture=SMBus_ReadTemp();
			
		if(tempture>MAX_TEMP)
		{
			Buzzer_ON ();
		}
		else {Buzzer_OFF();}
		KeyNum = Key_GetNum ();
		 if(KeyNum ==8)
		{
		KeyNum =0;
		break;
		}
		
	 }
  }
	  oled_new(); 
 }		

	
	
	}
