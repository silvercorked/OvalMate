/**
 * Title: OvalMate_delays_module_DevBoard
 * Version: 0.0.1
 * Filename: delays.h
 * Authors: Alex Wissing
 * Purpose of Program: This file contains several functions for delaying the microcontroller for set periods of time.
 * How Program is Run on Target System:
 * Date Started: 4/11/2022
 * Update History:
 *	- 4/11/2022
 *		Created delays files
 */

#include "delays.h"

/**
 * Delay for 20 milliseconds. This is exactly 1 period length of the servo motors only frequency.
 *
 * @params	- None
 * @return	- None
 */
void delay20ms() {
	uint32_t twentyMs = 20 * ONEMSDELAYHZ;
	for (uint32_t i = 0U; i < twentyMs; i++)	// 3 000 000 should be 20ms delay
		__asm("NOP"); /* delay */
}

/**
 * Delay for 1 millisecond. This is exactly 1 period length of the slowest speed of the stepper motors.
 *
 * @params	- None
 * @return	- None
 */
void delay1ms(void) {
	for (uint32_t i = 0U; i < ONEMSDELAYHZ; i++) // clock is 150MHz, div by 1000 -> 1 ms
		__asm("NOP"); /* delay */
}

/**
 * Delay for 500 microseconds. This is half of 1 millisecond and a useful delay for debouncing switches.
 *
 * @params  - None
 * @return  - None
 */
void delay500us() {
	uint32_t half = ONEMSDELAYHZ / 2;
	for (uint32_t i = 0U; i < half; i++)
		__asm("NOP"); /* delay */
}

/**
 * Delay for some given milliseconds.
 *
 * @params  - uint32_t ms	: number of milliseconds to delay.
 * @return  - None
 */
void delayms(uint32_t ms) {
	uint64_t clocks = ONEMSDELAYHZ * ms;
	for (uint64_t i = 0U; i < clocks; i++)
		__asm("NOP");
}
