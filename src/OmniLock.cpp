/*
 * OmniLock.cpp
 */

#include "OmniLock.h"
#include "stm32f10x.h"

OmniLock::OmniLock(HardwareConfig& hw):
m_InputStep(300), m_InputClosed(300)
{
	m_GpioMotorA = hw.GpioMotorA;
	m_PinMotorA = hw.PinMotorA;
	m_GpioMotorB = hw.GpioMotorB;
	m_PinMotorB = hw.PinMotorB;
	m_GpioSwitchStep = hw.GpioSwitchStep;
	m_PinSwitchStep = hw.PinSwitchStep;
	m_GpioSwitchClosed = hw.GpioSwitchClosed;
	m_PinSwitchClosed = hw.PinSwitchClosed;

	GPIO_InitTypeDef GPIO_InitStruct;

	// Switches
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_Pin = m_PinSwitchStep;
	GPIO_Init(m_GpioSwitchStep, &GPIO_InitStruct);
	GPIO_InitStruct.GPIO_Pin = m_PinSwitchClosed;
	GPIO_Init(m_GpioSwitchClosed, &GPIO_InitStruct);

	// Motor
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_Pin = m_PinMotorA;
	GPIO_Init(m_GpioMotorA, &GPIO_InitStruct);
	GPIO_InitStruct.GPIO_Pin = m_PinMotorB;
	GPIO_Init(m_GpioMotorB, &GPIO_InitStruct);

	DriveMotor(0);
}

OmniLock::~OmniLock()
{
}

void OmniLock::DriveMotor(int dir)
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

void OmniLock::Unlock()
{
}

void OmniLock::Lock()
{
}

void OmniLock::MoveOneStep()
{
	DriveMotor(1);
	while (m_InputStep.Debounce(IsOnStep()));
	while (!m_InputStep.Debounce(IsOnStep()));
	DriveMotor(0);
}

bool OmniLock::IsGateClosed()
{
	return !(GPIO_ReadInputDataBit(m_GpioSwitchClosed, m_PinSwitchClosed));
}

bool OmniLock::IsOnStep()
{
	return !(GPIO_ReadInputDataBit(m_GpioSwitchStep, m_PinSwitchStep));
}
