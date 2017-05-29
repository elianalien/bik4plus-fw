/* bik4plus-fw
 * main.cpp
 *
 * (c) 2017 Tuwuh Sarwoprasojo
 * All rights reserved.
 */

#include <stdio.h>
#include <stdlib.h>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wmissing-declarations"
#pragma GCC diagnostic ignored "-Wreturn-type"

int main(int argc, char* argv[])
{
	// Infinite loop
	while (1) {
		asm("nop");
	}
}

#pragma GCC diagnostic pop

