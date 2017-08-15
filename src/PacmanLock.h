/* bik4plus-fw, a firmware for BIK4+ Bike Sharing Lock
 * PacmanLock.h
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

#ifndef PACMANLOCK_H_
#define PACMANLOCK_H_

#include "stm32f10x.h"

class PacmanLock
{
public:
	struct HardwareConfig
	{
		GPIO_TypeDef* GpioMotorA;
		uint16_t PinMotorA;
		GPIO_TypeDef* GpioMotorB;
		uint16_t PinMotorB;
		GPIO_TypeDef* GpioSwitchGateOpen;
		uint16_t PinSwitchGateOpen;
		GPIO_TypeDef* GpioSwitchFree;
		uint16_t PinSwitchFree;
		GPIO_TypeDef* GpioSwitchLocked;
		uint16_t PinSwitchLocked;
	};

public:
	PacmanLock(HardwareConfig& hw);
	virtual ~PacmanLock();

	void DriveMotor(int dir);

	void Unlock();
	void Lock();

	bool IsGateOpen();
	bool IsFree();
	bool IsLocked();

private:
	GPIO_TypeDef* m_GpioMotorA;
	uint16_t m_PinMotorA;
	GPIO_TypeDef* m_GpioMotorB;
	uint16_t m_PinMotorB;
	GPIO_TypeDef* m_GpioSwitchGateOpen;
	uint16_t m_PinSwitchGateOpen;
	GPIO_TypeDef* m_GpioSwitchFree;
	uint16_t m_PinSwitchFree;
	GPIO_TypeDef* m_GpioSwitchLocked;
	uint16_t m_PinSwitchLocked;
};

#endif /* PACMANLOCK_H_ */
