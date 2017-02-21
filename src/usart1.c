#include "USART1.h"

char halfHEX(char v)
{
	if((v>=0)&&(v<=9)) return v+'0';
	if((v>=0x0a)&&(v<=0x0f)) return v-0x0a+'A';
	return ' ';
}
/* USART初始化 */
void USART1_Init(int speed)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStruct;
	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);  //使能GPIOA的时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);//使能USART的时钟
	/* USART1的端口配置 */
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_1);//配置PA9成第二功能引脚	TX
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_1);//配置PA10成第二功能引脚  RX	

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	

	/* USART1的基本配置 */
	USART_InitStructure.USART_BaudRate = speed;              //波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

	USART_Init(USART1, &USART_InitStructure);		
	USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);           //使能接收中断
	USART_Cmd(USART1, ENABLE);                             //使能USART1
	
	/* USART1的NVIC中断配置 */
	NVIC_InitStruct.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelPriority = 0x02;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStruct);
				
}

//=============================================================================
//文件名称：
//功能概要：USART1中断函数
//参数说明：无
//函数返回：无
//=============================================================================
void USART1_IRQHandler(void)
{
	if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
	{
		//putchFIFO_RX(USART_ReceiveData(USART1));//wbus_receive(USART_ReceiveData(USART1));
		USART_ClearITPendingBit(USART1,  USART_IT_RXNE);
	}		
}

/**
  * @brief  Retargets the C library printf function to the USART.
  * @param  None
  * @retval None
  */
PUTCHAR_PROTOTYPE
{
  /* Place your implementation of fputc here */
  /* e.g. write a character to the USART */
  USART_SendData(USART1, (uint8_t) ch);

  /* Loop until the end of transmission */
  while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET)
  {}

  return ch;
}

void putch(char ch)
{
	USART_SendData(USART1, (uint8_t) ch);
	while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET)
	{}
}

void putstr(char *str)
{
	while(*str!=0)
	{
		putch(*str++);
	}
}

void putHEX(char ch)
{
	putch(halfHEX((ch>>4)&0x0F));
	putch(halfHEX(ch&0x0F));
}

void putvalue(int32_t value) {
	if (value < 0) {
		putch('-');
		value = -value;
	}
	if (value >= 1000000000)
		putch(((value / 1000000000) % 10) + '0');
	if (value >= 100000000)
		putch(((value / 100000000) % 10) + '0');
	if (value >= 10000000)
		putch(((value / 10000000) % 10) + '0');
	if (value >= 1000000)
		putch(((value / 1000000) % 10) + '0');
	if (value >= 100000)
		putch(((value / 100000) % 10) + '0');
	if (value >= 10000)
		putch(((value / 10000) % 10) + '0');
	if (value >= 1000)
		putch(((value / 1000) % 10) + '0');
	if (value >= 100)
		putch(((value / 100) % 10) + '0');
	if (value >= 10)
		putch(((value / 10) % 10) + '0');
	putch((value % 10) + '0');
}
