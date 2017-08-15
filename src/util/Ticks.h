/* bik4plus-fw, a firmware for BIK4+ Bike Sharing Lock
 * Ticks.h
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

#ifndef TICKS_H_
#define TICKS_H_

#include <stdlib.h>
#include "stm32f10x.h"

extern uint32_t SystemCoreClock;

class Ticks
{
public:
	static void Init();
	static uint32_t Get();
	static bool HasElapsed(int32_t ticks);
	static void DelayUs(uint32_t us);
	static void DelayMs(uint32_t ms);
	static uint32_t UsToTicks(uint32_t us);
	static uint32_t MsToTicks(uint32_t ms);
};
#endif /* TICKS_H_ */
