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
}

int main(int argc, char* argv[])
{
	initGpio();
	initRtc();
	Ticks::Init();

	// Infinite loop
	while (1) {
		for (unsigned count = 0; count < 10; count++) {
			GPIO_WriteBit(GPIOB, GPIO_Pin_12,
					(BitAction) !GPIO_ReadOutputDataBit(GPIOB, GPIO_Pin_12));

			Ticks::DelayMs(250);
		}

//		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, DISABLE);
//		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, DISABLE);

//		__WFE();
//		PWR_EnterSTOPMode(PWR_Regulator_LowPower, PWR_STOPEntry_WFE);
//		PWR_EnterSTANDBYMode();
	}
}

#pragma GCC diagnostic pop

