#include <Broccoli.h>
#include "sx1278.h"
#include "stm32f0xx.h"
#include "usart1.h"

//#define COORDINATOR
//#define ROUTER
//#define FREE_ROUTER
#define ENDDEVICE

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
#ifdef FREE_ROUTER
	Broccoli_INIT(BROCCOLI_FREE_ROUTER);
#endif
#ifdef ENDDEVICE
	Broccoli_INIT(BROCCOLI_ENDDEVICE);
	Broccoli_UpLink("Hello Router!",13);
#endif
	while (1) {
#ifdef ROUTER
		Delay(1000);
		Broccoli_UpLink("Hello Coordinator!",18);
#endif
#ifdef ENDDEVICE
		//SleepNextWakeUp(1000);
		Delay(1000);
		Broccoli_UpLink("Hello Router!",13);
		//Radio_SleepMode();
#endif
		Broccoli_MainProcess();
	}
}

void putAddress(DEVICE_ADDRESS *addr) {
	uint8_t i;
	for (i = 0; i < sizeof(DEVICE_ADDRESS); i++) {
		if (i)
			putch(' ');
		putHEX(addr->addr[i]);
	}
}

void Broccoli_Receive(DEVICE_ADDRESS *addr1, DEVICE_ADDRESS *addr2,
		uint8_t *data, uint16_t length) {
	uint16_t i;
#ifdef COORDINATOR
	Delay(20);
	Broccoli_DownLink(addr1,"Hello Router!",13);
#endif
#ifdef FREE_ROUTER
	Delay(10);
	Broccoli_DownLink(addr1, "Hello Enddevice!", 16);
#endif
	if (addr1 != NULL) {
		putstr("From:(");
		putAddress(addr1);
		putstr("), ");
	}
	if (addr2 != NULL) {
		putstr("pass:(");
		putAddress(addr2);
		putstr("), ");
	}
	for (i = 0; i < length; i++)
		putch(data[i]);
	putstr("\n\r");
}
