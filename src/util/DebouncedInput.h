/* bik4plus-fw, a firmware for BIK4+ Bike Sharing Lock
 * DebouncedInput.h
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

#ifndef DEBOUNCEDINPUT_H_
#define DEBOUNCEDINPUT_H_

#include <stdint.h>

class DebouncedInput
{
public:
	DebouncedInput(uint32_t debounceTimeUs);
	virtual ~DebouncedInput();

	bool Debounce(bool input);

private:
	bool m_Active;
	uint32_t m_DebounceTimeTicks;
	uint32_t m_NextTimestamp;
};

#endif /* DEBOUNCEDINPUT_H_ */
