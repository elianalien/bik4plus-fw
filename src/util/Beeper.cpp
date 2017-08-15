/* bik4plus-fw, a firmware for BIK4+ Bike Sharing Lock
 * Beeper.cpp
 *
 * Copyright (C) 2017 Tuwuh Sarwoprasojo
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
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
