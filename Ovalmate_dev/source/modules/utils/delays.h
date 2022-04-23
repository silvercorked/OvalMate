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

#ifndef DELAYS_H
#define DELAYS_H

#include <stdint.h>

// Definitions
#define MCUCLOCKSPEEDHZ 150000000
#define ONEMSDELAYHZ	(MCUCLOCKSPEEDHZ / 1000)
#define ONEUSDELAYHZ	(MCUCLOCKSPEEDHZ / 1000000)
// End Definitions

// Prototypes
void delay20ms(void);
void delay1ms(void);
void delay500us(void);
// End Prototypes
#endif
