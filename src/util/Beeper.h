/* bik4plus-fw, a firmware for BIK4+ Bike Sharing Lock
 * Beeper.h
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

#ifndef BEEPER_H_
#define BEEPER_H_

#include "stm32f10x.h"

class Beeper
{
public:
	Beeper(GPIO_TypeDef* gpio, uint16_t pin);
	virtual ~Beeper();

	void BeepOnce(uint32_t durationMs = 100);
	void Beep(unsigned int count, uint32_t onMs, uint32_t offMs);

private:
	GPIO_TypeDef* m_pGpio;
	uint16_t m_Pin;
};

#endif /* BEEPER_H_ */
