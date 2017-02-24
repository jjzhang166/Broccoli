#include "Broccoli.h"
#include "stm32f0xx.h"
#include "usart1.h"
#include "sx1278.h"
static __IO uint32_t TimingDelay = 0;

uint32_t Delay_Init(int freq)
{
	uint32_t ret;
	RCC_ClocksTypeDef RCC_ClocksStatus;
	RCC_GetClocksFreq(&RCC_ClocksStatus);
	ret = SysTick_Config( RCC_ClocksStatus.HCLK_Frequency / freq);
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK);
	return ret;
}

void SysTick_Handler(void)
{
  if (TimingDelay != 0x00)
  {
    TimingDelay--;
  }
}

void Delay(__IO uint32_t nTime)
{
  TimingDelay = nTime;
  while(TimingDelay != 0);
}

void GPIO_Configure(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_AHBPeriphResetCmd(RCC_AHBPeriph_GPIOA | RCC_AHBPeriph_GPIOB, DISABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA | RCC_AHBPeriph_GPIOB, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}

/**
  * @brief  Configure PA2 in interrupt mode
  * @param  None
  * @retval None
  */
void EXTI0_Config(void)
{
	EXTI_InitTypeDef   EXTI_InitStructure;
	GPIO_InitTypeDef   GPIO_InitStructure;
	NVIC_InitTypeDef   NVIC_InitStructure;
	/* Enable GPIOA clock */
	//RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);

	/* Configure PA0 pin as input floating */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* Enable SYSCFG clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
	/* Connect EXTI0 Line to PA0 pin */
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource0);

	/* Configure EXTI0 line */
	EXTI_InitStructure.EXTI_Line = EXTI_Line0;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);

	/* Enable and set EXTI0 Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPriority = 0x00;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

void SPI1_Interface_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	SPI_InitTypeDef SPI_InitStructure;

	RCC_APB2PeriphResetCmd (RCC_APB2Periph_SPI1, DISABLE);
	RCC_APB2PeriphClockCmd (RCC_APB2Periph_SPI1, ENABLE);

	//RCC_AHBPeriphResetCmd(RCC_AHBPeriph_GPIOA, DISABLE);
	//RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);

	/* Configure SD_SPI pins: SCK */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* Configure SD_SPI pins: MISO */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* Configure SD_SPI pins: MOSI */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* Configure SD_SPI_CS_PIN pin: SD Card CS pin */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* Connect PXx to SD_SPI_SCK */
	GPIO_PinAFConfig(GPIOA, GPIO_Pin_5, GPIO_AF_0);

	/* Connect PXx to SD_SPI_MISO */
	GPIO_PinAFConfig(GPIOA, GPIO_Pin_6, GPIO_AF_0);

	/* Connect PXx to SD_SPI_MOSI */
	GPIO_PinAFConfig(GPIOA, GPIO_Pin_7, GPIO_AF_0);

	/* SPI configuration -------------------------------------------------------*/
	SPI_I2S_DeInit(SPI1);

	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;

	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 7;
	SPI_Init(SPI1, &SPI_InitStructure);

	SPI_RxFIFOThresholdConfig(SPI1, SPI_RxFIFOThreshold_QF);

	SPI_Cmd(SPI1, ENABLE); /* SD_SPI enable */
	GPIO_WriteBit(GPIOA, GPIO_Pin_4, Bit_SET);
}

void SX1278Reset(void)
{
	GPIO_ResetBits(GPIOA, GPIO_Pin_3);
	Delay(10);
	GPIO_SetBits(GPIOA, GPIO_Pin_3);
	Delay(100);
}

unsigned char HW_SPI_InOut(unsigned char out)
{
	/* Wait until the transmit buffer is empty */
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET)
	{
	}
	/* Send the byte */
	SPI_SendData8(SPI1, out);

	/* Wait until a data is received */
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET)
	{
	}

	/* Return the shifted data */
	return SPI_ReceiveData8(SPI1);
}

void HW_SPI_WR_DX(unsigned char *wbuf,unsigned char *rbuf, int len)
{
	int i;
	GPIO_ResetBits(GPIOA, GPIO_Pin_4);
	for(i=0;i<len;i++)
	{
		rbuf[i] = HW_SPI_InOut(wbuf[i]);
	}
	GPIO_SetBits(GPIOA, GPIO_Pin_4);
}

void HW_SPI_RW_HF(unsigned char *wbuf,int wlen, unsigned char *rbuf, int rlen)
{
	int i;
	GPIO_ResetBits(GPIOA, GPIO_Pin_4);
	for(i=0;i<wlen;i++)
	{
		HW_SPI_InOut(wbuf[i]);
	}
	for(i=0;i<rlen;i++)
	{
		rbuf[i] = HW_SPI_InOut(0xFF);
	}
	GPIO_SetBits(GPIOA, GPIO_Pin_4);
}

/**
  * @brief  This function handles External line 0 to 1 interrupt request.
  * @param  None
  * @retval None
  */
void EXTI0_1_IRQHandler(void)
{
  if(EXTI_GetITStatus(EXTI_Line0) != RESET)
  {
	  SX1278_Interupt();
    /* Clear the EXTI line 0 pending bit */
    EXTI_ClearITPendingBit(EXTI_Line0);
  }
}

void SystemWaitTime(void)
{
	while((SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk) == 0);
}

void GetDeviceAddress (DEVICE_ADDRESS *addr)
{
	uint8_t *p = (uint8_t *)0x1FFFF7AC;
	memcpy(addr, p ,sizeof(DEVICE_ADDRESS));
}

//void SaveHostAddress (DEVICE_ADDRESS *addr)
//{
//	//TODO
//}
//
//void LoadHostAddress (DEVICE_ADDRESS *addr)
//{
//	memcpy(addr->addr,"\x01\x00\x24\x00\x0A\x57\x32\x58",8);
//}
//
//void CleanHostAddress (void)
//{
//	//TODO
//}

//void SX1278_RxOpen(void)
//{
//	GPIO_SetBits(GPIOB, GPIO_Pin_1);
//}
//
//void SX1278_TxOpen(void)
//{
//	GPIO_ResetBits(GPIOB, GPIO_Pin_1);
//}

void SX1278_EnOpen(void)
{
	GPIO_ResetBits(GPIOA, GPIO_Pin_4);
}

void SX1278_EnClose(void)
{
	GPIO_SetBits(GPIOA, GPIO_Pin_4);
}

unsigned char SX1278_ByteWriteReadfunc(unsigned char out)
{
	return HW_SPI_InOut(out);
}
