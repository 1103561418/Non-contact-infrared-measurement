#include "stm32f10x.h"
void UART_Init(void) {
    USART_InitTypeDef USART_InitStructure; // 定义用于初始化USART的结构体
    GPIO_InitTypeDef GPIO_InitStructure; // 定义用于初始化GPIO的结构体
    NVIC_InitTypeDef NVIC_InitStructure; // 定义用于初始化中断控制器的结构体

    // 1. 使能时钟
    // 启用GPIOA和USART1的时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE);

    // 2. 配置USART1 Tx (PA.09) 和 Rx (PA.10)的GPIO
    // 设置PA9作为USART1的Tx（发送）引脚
     // 设置GPIO速度

    // 设置PA10作为USART1的Rx（接收）引脚
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; // PA.10
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; // 设置为浮空输入
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure); // 应用设置

    // 3. 配置USART1
    // 设置USART1的参数：波特率、数据位、停止位、校验位等
    USART_InitStructure.USART_BaudRate = 115200; // 波特率9600
    USART_InitStructure.USART_WordLength = USART_WordLength_8b; // 数据位8位
    USART_InitStructure.USART_StopBits = USART_StopBits_1; // 1个停止位
    USART_InitStructure.USART_Parity = USART_Parity_No; // 无校验位
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; // 无硬件流控制
    USART_InitStructure.USART_Mode = USART_Mode_Rx; // 启用接收和发送
    USART_Init(USART1, &USART_InitStructure); // 应用USART设置

    // 4. 配置USART1中断
    // 设置USART1中断的优先级并使能
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn; // USART1中断
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2; // 抢占优先级
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2; // 子优先级
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; // 使能中断通道
    NVIC_Init(&NVIC_InitStructure); // 应用中断设置

    // 5. 使能USART1接收中断
    // 使能USART1的接收中断（接收到数据时触发）
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);

    // 6. 使能USART1
    // 启用USART1模块
    USART_Cmd(USART1, ENABLE);
}

void UART_STOP(void){
	    // 1. 禁用 USART1 接收和发送
    USART_Cmd(USART1, DISABLE);

    // 2. 关闭 USART1 的中断
    USART_ITConfig(USART1, USART_IT_RXNE, DISABLE);
    NVIC_DisableIRQ(USART1_IRQn);

    // 3. 将 USART1 寄存器复位到默认值
    USART_DeInit(USART1);  // 适用于标准外设库
    // 或使用 HAL_UART_DeInit(&huart1);  // 如果您使用的是 HAL 库

    // 4. (可选) 禁用 USART1 相关的 GPIO 引脚
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; // 设置为浮空输入
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // 5. (可选) 关闭 USART1 的时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, DISABLE);
}

void UART_START(void){
	UART_Init();
}	

extern uint8_t rx_buffer[20];
static int rx_index = 0;
static int header_received = 0;  // 增加一个状态标志来确认是否接收到了正确的帧头

void USART1_IRQHandler(void) {


    if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) {
        uint8_t data = USART_ReceiveData(USART1);

        // 检查帧头
        if (rx_index == 0 && data == 0x2C) {
            // 第一个字节是0x2C，暂时仅记录
            rx_buffer[rx_index++] = data;
        } else if (rx_index == 1 && data == 0x12) {
            // 确认第二个字节是0x12，表示帧头确认成功
            rx_buffer[rx_index++] = data;
            header_received = 1;  // 设置帧头已接收标志
        } else if (header_received) {
            // 如果已经接收到帧头，开始接收数据
            if (data == 0x5B) {
                // 如果接收到帧尾
                rx_buffer[rx_index] = data;  // 存储帧尾
               // ProcessData(rx_buffer);  // 处理数据
                rx_index = 0;  // 重置索引以准备下一数据包
                header_received = 0;  // 重置帧头接收标志
            } else {
                // 继续存储数据
                if (rx_index < sizeof(rx_buffer)) {
                    rx_buffer[rx_index++] = data;
                } else {
                    // 如果数组溢出，则重置接收索引和帧头标志
                    rx_index = 0;
                    header_received = 0;
                }
            }
        } else {
            // 如果没有正确接收帧头，重置索引
            rx_index = 0;
        }
    }
}



	




