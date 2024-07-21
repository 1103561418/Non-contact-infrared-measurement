#include "stm32f10x.h"
void UART_Init(void) {
    USART_InitTypeDef USART_InitStructure; // �������ڳ�ʼ��USART�Ľṹ��
    GPIO_InitTypeDef GPIO_InitStructure; // �������ڳ�ʼ��GPIO�Ľṹ��
    NVIC_InitTypeDef NVIC_InitStructure; // �������ڳ�ʼ���жϿ������Ľṹ��

    // 1. ʹ��ʱ��
    // ����GPIOA��USART1��ʱ��
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE);

    // 2. ����USART1 Tx (PA.09) �� Rx (PA.10)��GPIO
    // ����PA9��ΪUSART1��Tx�����ͣ�����
     // ����GPIO�ٶ�

    // ����PA10��ΪUSART1��Rx�����գ�����
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; // PA.10
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; // ����Ϊ��������
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure); // Ӧ������

    // 3. ����USART1
    // ����USART1�Ĳ����������ʡ�����λ��ֹͣλ��У��λ��
    USART_InitStructure.USART_BaudRate = 115200; // ������9600
    USART_InitStructure.USART_WordLength = USART_WordLength_8b; // ����λ8λ
    USART_InitStructure.USART_StopBits = USART_StopBits_1; // 1��ֹͣλ
    USART_InitStructure.USART_Parity = USART_Parity_No; // ��У��λ
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; // ��Ӳ��������
    USART_InitStructure.USART_Mode = USART_Mode_Rx; // ���ý��պͷ���
    USART_Init(USART1, &USART_InitStructure); // Ӧ��USART����

    // 4. ����USART1�ж�
    // ����USART1�жϵ����ȼ���ʹ��
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn; // USART1�ж�
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2; // ��ռ���ȼ�
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2; // �����ȼ�
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; // ʹ���ж�ͨ��
    NVIC_Init(&NVIC_InitStructure); // Ӧ���ж�����

    // 5. ʹ��USART1�����ж�
    // ʹ��USART1�Ľ����жϣ����յ�����ʱ������
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);

    // 6. ʹ��USART1
    // ����USART1ģ��
    USART_Cmd(USART1, ENABLE);
}

void UART_STOP(void){
	    // 1. ���� USART1 ���պͷ���
    USART_Cmd(USART1, DISABLE);

    // 2. �ر� USART1 ���ж�
    USART_ITConfig(USART1, USART_IT_RXNE, DISABLE);
    NVIC_DisableIRQ(USART1_IRQn);

    // 3. �� USART1 �Ĵ�����λ��Ĭ��ֵ
    USART_DeInit(USART1);  // �����ڱ�׼�����
    // ��ʹ�� HAL_UART_DeInit(&huart1);  // �����ʹ�õ��� HAL ��

    // 4. (��ѡ) ���� USART1 ��ص� GPIO ����
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; // ����Ϊ��������
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // 5. (��ѡ) �ر� USART1 ��ʱ��
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, DISABLE);
}

void UART_START(void){
	UART_Init();
}	

extern uint8_t rx_buffer[20];
static int rx_index = 0;
static int header_received = 0;  // ����һ��״̬��־��ȷ���Ƿ���յ�����ȷ��֡ͷ

void USART1_IRQHandler(void) {


    if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) {
        uint8_t data = USART_ReceiveData(USART1);

        // ���֡ͷ
        if (rx_index == 0 && data == 0x2C) {
            // ��һ���ֽ���0x2C����ʱ����¼
            rx_buffer[rx_index++] = data;
        } else if (rx_index == 1 && data == 0x12) {
            // ȷ�ϵڶ����ֽ���0x12����ʾ֡ͷȷ�ϳɹ�
            rx_buffer[rx_index++] = data;
            header_received = 1;  // ����֡ͷ�ѽ��ձ�־
        } else if (header_received) {
            // ����Ѿ����յ�֡ͷ����ʼ��������
            if (data == 0x5B) {
                // ������յ�֡β
                rx_buffer[rx_index] = data;  // �洢֡β
               // ProcessData(rx_buffer);  // ��������
                rx_index = 0;  // ����������׼����һ���ݰ�
                header_received = 0;  // ����֡ͷ���ձ�־
            } else {
                // �����洢����
                if (rx_index < sizeof(rx_buffer)) {
                    rx_buffer[rx_index++] = data;
                } else {
                    // �����������������ý���������֡ͷ��־
                    rx_index = 0;
                    header_received = 0;
                }
            }
        } else {
            // ���û����ȷ����֡ͷ����������
            rx_index = 0;
        }
    }
}



	




