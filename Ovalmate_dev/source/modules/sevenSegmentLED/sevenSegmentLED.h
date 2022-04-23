/**
 * Title: OvalMate_sevenSegmentLED_module_DevBoard
 * Version: 0.0.1
 * Filename: LPC55S16_Project_main_dev.c
 * Authors: Alex Wissing
 * Purpose of Program: Manage 7-segment LEDs
 * How Program is Run on Target System:
 * Date Started: 2/28/2022
 * Update History:
 *	- 2/28/2022
 *		Moved this into separate "modules"
 *	- 3/7/2022:
 *		Added definition safeguards around major declarations to avoid duplicate declarations and imports
 *	- 3/14/2022:
 *		modified seven seg function names to better represent the modular approach
 *		Removed extern variables, as user shouldn't need those.
 *		Setup macro functions for common functional outputs. Added displayErrorCode function for error states
 */

#ifndef SEVENSEGMENT_H
#define SEVENSEGMENT_H

// Includes
#include "fsl_gpio.h"
#include <stdbool.h>
// End Includes

// Structs
#ifndef PININFORMATION_S
#define PININFORMATION_S
typedef struct {
	uint8_t port;
	uint8_t pin;
} pinInformation_s;
#endif
// End Structs

// Macro Functions
#define SEVENSEG_setLegs_0() \
	SEVENSEG_setLegs(true, true, true, true, true, true, false)
#define SEVENSEG_setLegs_1() \
	SEVENSEG_setLegs(false, true, true, false, false, false, false)
#define SEVENSEG_setLegs_2() \
	SEVENSEG_setLegs(true, true, false, true, true, false, true)
#define SEVENSEG_setLegs_3() \
	SEVENSEG_setLegs(true, true, true, true, false, false, true)
#define SEVENSEG_setLegs_4() \
	SEVENSEG_setLegs(true, true, true, false, false, true, true)
#define SEVENSEG_setLegs_5() \
	SEVENSEG_setLegs(true, false, true, true, false, true, true)
#define SEVENSEG_setLegs_6() \
	SEVENSEG_setLegs(true, false, true, true, true, true, true)
#define SEVENSEG_setLegs_7() \
	SEVENSEG_setLegs(true, true, true, false, false, false, false)
#define SEVENSEG_setLegs_8() \
	SEVENSEG_setLegs(true, true, true, true, true, true, true)
#define SEVENSEG_setLegs_9() \
	SEVENSEG_setLegs(true, true, true, true, false, true, true)
#define SEVENSEG_setLegs_A() \
	SEVENSEG_setLegs(true, true, true, false, true, true, false)
// looks like 8
#define SEVENSEG_setLegs_B() \
	SEVENSEG_setLegs(true, true, true, true, true, true, true)
#define SEVENSEG_setLegs_C() \
	SEVENSEG_setLegs(true, false, false, true, true, true, false)
// looks like 0
#define SEVENSEG_setLegs_D() \
	SEVENSEG_setLegs(true, true, true, true, true, true, false)
#define SEVENSEG_setLegs_E() \
	SEVENSEG_setLegs(true, false, false, true, true, true, true)
#define SEVENSEG_setLegs_F() \
	SEVENSEG_setLegs(true, false, false, false, true, true, true)
// End Macro Functions

// Prototypes
void SEVENSEG_setLegs(
		bool legA,
		bool legB,
		bool legC,
		bool legD,
		bool legE,
		bool legF,
		bool legG
);
void SEVENSEG_displayErrorCode(uint8_t);
void SEVENSEG_initializeLegs(void);
// End Prototypes

#endif
