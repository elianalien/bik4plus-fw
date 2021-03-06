/* bik4plus-fw, a firmware for BIK4+ Bike Sharing Lock
 * Ticks.cpp
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

#include "Ticks.h"

void Ticks::Init()
{
	if (!(CoreDebug->DEMCR & CoreDebug_DEMCR_TRCENA_Msk)) {
		CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
	}
	DWT->CYCCNT = 0;
	DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
}

uint32_t Ticks::Get()
{
	return DWT->CYCCNT;
}

bool Ticks::HasElapsed(int32_t ticks)
{
	return (((int32_t) (Get()) - ticks) >= 0);
}

void Ticks::DelayUs(uint32_t us)
{
	int32_t tp = Get() + UsToTicks(us);
	while (!HasElapsed(tp));
}

void Ticks::DelayMs(uint32_t ms)
{
	int32_t tp = Get() + MsToTicks(ms);
	while (!HasElapsed(tp));
}

uint32_t Ticks::UsToTicks(uint32_t us)
{
	return us * (SystemCoreClock / 1000000);
}

uint32_t Ticks::MsToTicks(uint32_t ms)
{
	return ms * (SystemCoreClock / 1000);
}
