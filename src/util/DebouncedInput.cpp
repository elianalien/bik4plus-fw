/* bik4plus-fw, a firmware for BIK4+ Bike Sharing Lock
 * DebouncedInput.cpp
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
