/*
 * OmniLock.h
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
