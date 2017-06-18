/*
 * PacmanLock.cpp
 */

#include "DebouncedInput.h"
#include "PacmanLock.h"
#include "Ticks.h"
#include "stm32f10x.h"

PacmanLock::PacmanLock(HardwareConfig& hw)
{
	m_GpioMotorA           = hw.GpioMotorA;
	m_PinMotorA            = hw.PinMotorA;
	m_GpioMotorB           = hw.GpioMotorB;
	m_PinMotorB            = hw.PinMotorB;
	m_GpioSwitchGateOpen   = hw.GpioSwitchGateOpen;
	m_PinSwitchGateOpen    = hw.PinSwitchGateOpen;
	m_GpioSwitchFree       = hw.GpioSwitchFree;
	m_PinSwitchFree        = hw.PinSwitchFree;
	m_GpioSwitchLocked     = hw.GpioSwitchLocked;
	m_PinSwitchLocked      = hw.PinSwitchLocked;

	GPIO_InitTypeDef GPIO_InitStruct;

	// Switches
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_Pin = m_PinSwitchGateOpen;
	GPIO_Init(m_GpioSwitchGateOpen, &GPIO_InitStruct);
	GPIO_InitStruct.GPIO_Pin = m_PinSwitchFree;
	GPIO_Init(m_GpioSwitchFree, &GPIO_InitStruct);
	GPIO_InitStruct.GPIO_Pin = m_PinSwitchLocked;
	GPIO_Init(m_GpioSwitchLocked, &GPIO_InitStruct);

	// Motor
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_Pin = m_PinMotorA;
	GPIO_Init(m_GpioMotorA, &GPIO_InitStruct);
	GPIO_InitStruct.GPIO_Pin = m_PinMotorB;
	GPIO_Init(m_GpioMotorB, &GPIO_InitStruct);
}

PacmanLock::~PacmanLock()
{
}

void PacmanLock::DriveMotor(int dir)
{
	if (dir < 0) {
		GPIO_SetBits(m_GpioMotorA, m_PinMotorA);
		GPIO_ResetBits(m_GpioMotorB, m_PinMotorB);
	} else if (dir > 0) {
		GPIO_ResetBits(m_GpioMotorA, m_PinMotorA);
		GPIO_SetBits(m_GpioMotorB, m_PinMotorB);
	} else {
		GPIO_ResetBits(m_GpioMotorA, m_PinMotorA);
		GPIO_ResetBits(m_GpioMotorB, m_PinMotorB);
	}
}

bool PacmanLock::IsGateOpen()
{
	return !(GPIO_ReadInputDataBit(m_GpioSwitchGateOpen, m_PinSwitchGateOpen));
}

bool PacmanLock::IsFree()
{
	return !(GPIO_ReadInputDataBit(m_GpioSwitchFree, m_PinSwitchFree));
}

void PacmanLock::Unlock()
{
	DriveMotor(-1);
	while (!IsFree());
	DriveMotor(0);
}

void PacmanLock::Lock()
{
	if (IsGateOpen()) return;
	DriveMotor(-1);
	while (!IsLocked());
	DriveMotor(0);
}

bool PacmanLock::IsLocked()
{
	return !(GPIO_ReadInputDataBit(m_GpioSwitchLocked, m_PinSwitchLocked));
}
