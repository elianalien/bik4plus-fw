/*
 * Ticks.h
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
	static bool HasElapsed(int32_t tp);
	static void DelayUs(uint32_t us);
	static void DelayMs(uint32_t ms);
	static uint32_t UsToTicks(uint32_t us);
	static uint32_t MsToTicks(uint32_t ms);
};
#endif /* TICKS_H_ */
