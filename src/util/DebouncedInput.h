/*
 * DebouncedInput.h
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
