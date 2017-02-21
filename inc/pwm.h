#ifndef __WZL_PWM_H__
#define __WZL_PWM_H__
	#include "stm32f0xx.h"
	
	#define ENDSWITCH	0

	#define PWM1_ON(); TIM14->CR1 |= TIM_CR1_CEN;
	#define PWM1_OFF(); TIM14->CR1 &= (uint16_t)(~((uint16_t)TIM_CR1_CEN));
	#define PWM2_ON(); TIM16->CR1 |= TIM_CR1_CEN;
	#define PWM2_OFF(); TIM16->CR1 &= (uint16_t)(~((uint16_t)TIM_CR1_CEN));
	#define PWM3_ON(); TIM17->CR1 |= TIM_CR1_CEN;
	#define PWM3_OFF(); TIM17->CR1 &= (uint16_t)(~((uint16_t)TIM_CR1_CEN));

	//#define PWM1_ON()		TIM_CCxCmd(TIM14,TIM_Channel_1,TIM_CCx_Enable)
	//#define PWM1_OFF()	TIM_CCxCmd(TIM14,TIM_Channel_1,TIM_CCx_Disable)
	//#define PWM2_ON()		TIM_CCxCmd(TIM16,TIM_Channel_1,TIM_CCx_Enable)
	//#define PWM2_OFF()	TIM_CCxCmd(TIM16,TIM_Channel_1,TIM_CCx_Disable)
	//#define PWM3_ON()		TIM_CCxCmd(TIM17,TIM_Channel_1,TIM_CCx_Enable)
	//#define PWM3_OFF()	TIM_CCxCmd(TIM17,TIM_Channel_1,TIM_CCx_Disable)

	extern __IO uint32_t PWM1_busy;
	extern __IO uint32_t PWM2_busy;
	extern __IO uint32_t PWM3_busy;
	extern __IO uint32_t PWM1_pulse;
	extern __IO uint32_t PWM2_pulse;
	extern __IO uint32_t PWM3_pulse;
	void TIM14_IRQHandler(void);
	void TIM16_IRQHandler(void);
	void TIM17_IRQHandler(void);
	void PWM1_Configuration(void);
	void PWM2_Configuration(void);
	void PWM3_Configuration(void);
	void PWM1_setPulse(int32_t value);
	void PWM2_setPulse(int32_t value);
	void PWM3_setPulse(int32_t value);
	void PWM_Init(void);
	void PWM_setFreq(TIM_TypeDef* TIMx, uint32_t freq);
	void PWM_Freq(uint8_t idx, uint32_t freq);
	void PWM_setPulse(uint8_t idx, int32_t value);
	void PWMn_setPulse(int32_t value1, int32_t value2, int32_t value3);
#endif
