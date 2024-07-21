#include "stm32f10x.h"
#include "usart3.h"     
#include "Key.h"
#include "oled.h"
#include "Buzzer.h"
#include "stdio.h"
#include "Delay.h"
#include "usart3.h"
extern uint8_t KeyNum;
extern u8 send1[3]; // 三个有效数据位
extern u8 send2[3]; // 三个有效数据位
extern u8 send3[3]; // 三个有效数据位
extern u8 send4[3]; // 三个有效数据位
void Face_recognition(void)
{ 
    Key_Init();
    Buzzer_Init();
    
    uart3_init(115200);
    
    while(1)
{    
    
      
       
}
}
