/* bik4plus-fw
 * main.cpp
 *
 * (c) 2017 Tuwuh Sarwoprasojo
 * All rights reserved.
 */

#include <stdio.h>
#include <stdlib.h>

#include "util/Ticks.h"

#include "stm32f10x.h"
#include "stm32f10x_exti.h"
#include "stm32f10x_pwr.h"
#include "stm32f10x_rtc.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wmissing-declarations"
#pragma GCC diagnostic ignored "-Wreturn-type"

void initRtc()
{
}

void initGpio()
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);

	GPIO_InitTypeDef GPIO_InitStruct;

	// LED
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_12;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStruct);

	// MCO (alternate function for PA8)
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	RCC_MCOConfig(RCC_MCO_SYSCLK);

	// External event
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOB, &GPIO_InitStruct);

	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource11);

	EXTI_InitTypeDef EXTI_InitStruct;
	EXTI_InitStruct.EXTI_Line = EXTI_Line11;
	EXTI_InitStruct.EXTI_LineCmd = ENABLE;
	EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Event;
	EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_Init(&EXTI_InitStruct);
}

int main(int argc, char* argv[])
{
	initGpio();
	initRtc();
	Ticks::Init();

	// Infinite loop
	while (1) {
//		GPIO_WriteBit(GPIOB, GPIO_Pin_12, (BitAction) GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_11));
		GPIO_WriteBit(GPIOB, GPIO_Pin_12, Bit_SET);
		for (unsigned count = 0; count < 10; count++) {
			GPIO_WriteBit(GPIOB, GPIO_Pin_12,
					(BitAction) !GPIO_ReadOutputDataBit(GPIOB, GPIO_Pin_12));

			Ticks::DelayMs(100);
		}

//		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, DISABLE);
//		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, DISABLE);

		// Based on http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.faqs/ka15506.html
		// The sequence SEV WFE WFE is used to ensure sleep
		SCB->SCR &= ~SCB_SCR_SLEEPDEEP;
		__SEV();
		__WFE();
		__WFE();
//		PWR_EnterSTOPMode(PWR_Regulator_LowPower, PWR_STOPEntry_WFE);
//		PWR_EnterSTANDBYMode();
	}
}

#pragma GCC diagnostic pop

