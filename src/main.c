#include <Broccoli.h>
#include "sx1278.h"
#include "stm32f0xx.h"
#include "usart1.h"

void GPIO_Configure(void);
uint32_t Delay_Init(int freq);
void SPI1_Interface_Init(void);
void EXTI0_Config(void);
void SX1278Reset(void);

int main(void) {
	SystemInit();
	Delay_Init(1000);
	GPIO_Configure();
	USART1_Init(115200);
	SPI1_Interface_Init();
	SX1278Reset();
	SX1278LORA_INT();
	EXTI0_Config();
#ifdef COORDINATOR
	Broccoli_INIT(BROCCOLI_COORDINATOR);
#endif
#ifdef ROUTER
	Broccoli_INIT(BROCCOLI_ROUTER);
	Broccoli_UpLink("Hello Coordinator!",18);
#endif
	while (1) {
		#ifdef ROUTER
		Broccoli_SendData_PtoHost("Hello Coordinator!",254-17);
		#endif
		Broccoli_MainProcess();
		Delay(50);
	}
}
