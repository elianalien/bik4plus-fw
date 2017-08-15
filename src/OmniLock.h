/* bik4plus-fw, a firmware for BIK4+ Bike Sharing Lock
 * OmniLock.h
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

#ifndef OMNILOCK_H_
#define OMNILOCK_H_

#include "stm32f10x.h"
#include "DebouncedInput.h"

class OmniLock
{
public:
	struct HardwareConfig
	{
		GPIO_TypeDef* GpioMotorA;
		uint16_t PinMotorA;
		GPIO_TypeDef* GpioMotorB;
		uint16_t PinMotorB;
		GPIO_TypeDef* GpioSwitchStep;
		uint16_t PinSwitchStep;
		GPIO_TypeDef* GpioSwitchClosed;
		uint16_t PinSwitchClosed;
	};

public:
	OmniLock(HardwareConfig& hw);
	virtual ~OmniLock();

	void DriveMotor(int dir);

	void Initialize();

	void Unlock();
	void Lock();
	bool IsLocked();
	bool IsOnStep();
	bool IsGateClosed();

private:
	void moveOneStep(bool fwd);

private:
	GPIO_TypeDef* m_GpioMotorA;
	uint16_t m_PinMotorA;

	GPIO_TypeDef* m_GpioMotorB;
	uint16_t m_PinMotorB;

	GPIO_TypeDef* m_GpioSwitchStep;
	uint16_t m_PinSwitchStep;
	DebouncedInput m_InputStep;

	GPIO_TypeDef* m_GpioSwitchClosed;
	uint16_t m_PinSwitchClosed;
	DebouncedInput m_InputClosed;

	enum {
		Free,
		Locked
	} m_State;
};

#endif /* OMNILOCK_H_ */
