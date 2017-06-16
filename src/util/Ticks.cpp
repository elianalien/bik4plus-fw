/*
 * Ticks.cpp
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
