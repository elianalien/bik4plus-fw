/*
 * Beeper.h
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
