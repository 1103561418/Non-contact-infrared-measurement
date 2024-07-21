#include "usart3.h"

// ????   ???????   ?????       ??????

/**
 * @brief 	??3???
 * @param 	?
 * @retval  ?
 */
void uart3_init(u32 bound)
{
    // GPIO????
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE); // ??USART3,GPIOA??

    // USART3_TX   GPIOB10      	PB10
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; // PA2
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; // ??????
    GPIO_Init(GPIOB, &GPIO_InitStructure);          // ???GPIOB10

    // USART3_RX	GPIOB11   		PB11
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;            // PB11
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; // ????
    GPIO_Init(GPIOB, &GPIO_InitStructure);                // ???GPIOA3

    // USART3 NVIC ??
    NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; // ?????3
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;        // ????3
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;           // IRQ????
    NVIC_Init(&NVIC_InitStructure);                           // ??????????VIC???

    // USART ?????
    USART_InitStructure.USART_BaudRate = bound;                                     // ?????
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;                     // ???8?????
    USART_InitStructure.USART_StopBits = USART_StopBits_1;                          // ?????
    USART_InitStructure.USART_Parity = USART_Parity_No;                             // ??????
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; // ????????
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;                 // ????

    USART_Init(USART3, &USART_InitStructure);      // ?????1
    USART_ITConfig(USART3, USART_IT_RXNE, ENABLE); // ????????
    USART_Cmd(USART3, ENABLE);                     // ????1
}

/**
 * @brief 	??3?????
 * @param 	?
 * @retval  ?
 */
void u3_sendData(u8 *str) // ???????  b3 b3 data1 data2 data3 data4,b5
{

    u8 i = 0;
    for (i = 0; i < 2; i++) // b3 b3
    {
        USART_SendData(USART3, 0xb3);
        while (USART_GetFlagStatus(USART3, USART_FLAG_TC) != SET)
            ;
    }

    for (i = 0; i < 2; i++) // ???????
    {
        USART_SendData(USART3, str[i]);
        while (USART_GetFlagStatus(USART3, USART_FLAG_TC) != SET)
            ;
    }

    USART_SendData(USART3, 0x5b); // 0x5b
    while (USART_GetFlagStatus(USART3, USART_FLAG_TC) != SET)
        ;
}

/**
 * @brief 	??3????
 * @param 	?
 * @retval  ?
 */
void USART3_IRQHandler(void) // ??1??????
{
    uint8_t ReceiveData;
    if (USART_GetITStatus(USART3, USART_IT_RXNE) != RESET) // ????(?????????0x0d 0x0a??)
    {
        ReceiveData = USART_ReceiveData(USART3); // ?????

        // USART_SendData(USART3, ReceiveData); ??????
        // if (ReceiveData == 58) // ????0x3A?  ??????    ???????????????
        // {
        //     MY_GPIO_Init(GPIOC, GPIO_Pin_13, GPIO_Mode_Out_PP);
        // }

        USART_ClearITPendingBit(USART3, USART_IT_RXNE);
    }
}
