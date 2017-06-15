/*
 * DebouncedInput.cpp
 */

#include "DebouncedInput.h"
#include "Ticks.h"

DebouncedInput::DebouncedInput(uint32_t debounceTimeUs)
{
	m_NextTimestamp = 0;
	m_DebounceTimeTicks = Ticks::UsToTicks(debounceTimeUs);
	m_Active = false;
}

DebouncedInput::~DebouncedInput()
{
}

bool DebouncedInput::Debounce(bool input)
{
	if (input != m_Active && Ticks::HasElapsed(m_NextTimestamp)) {
		m_Active = input;
	} else if (input == m_Active) {
		m_NextTimestamp = Ticks::Get() + m_DebounceTimeTicks;
	}

	return m_Active;
}
