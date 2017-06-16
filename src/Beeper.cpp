/*
 * Beeper.cpp
 */

#include "Beeper.h"
#include "Ticks.h"
#include "stm32f10x.h"

Beeper::Beeper(GPIO_TypeDef* pGpio, uint16_t pin):
	m_pGpio(pGpio),
	m_Pin(pin)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_Pin = m_Pin;
	GPIO_Init(m_pGpio, &GPIO_InitStruct);

	GPIO_ResetBits(m_pGpio, m_Pin);
}

Beeper::~Beeper()
{
}

void Beeper::BeepOnce(uint32_t durationMs)
{
	GPIO_SetBits(m_pGpio, m_Pin);
	Ticks::DelayMs(durationMs);
	GPIO_ResetBits(m_pGpio, m_Pin);
}

void Beeper::Beep(unsigned int count, uint32_t onMs, uint32_t offMs)
{
	for (unsigned int i = 0; i < count; i++) {
		BeepOnce(onMs);
		Ticks::DelayMs(offMs);
	}
}
