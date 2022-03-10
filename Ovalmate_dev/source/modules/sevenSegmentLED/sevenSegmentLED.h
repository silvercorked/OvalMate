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

// Extern Variables
// Variables
extern pinInformation_s legA;
extern pinInformation_s legB;
extern pinInformation_s legC;
extern pinInformation_s legD;
extern pinInformation_s legE;
extern pinInformation_s legF;
extern pinInformation_s legG;
	// leg a w/ logic low = 1.6v, logic high = 3.3v (this doesn't activate the LED).
	// leg b w/ logic low = 0v, logic high = 1.6v (this does activate the LED).
extern gpio_pin_config_t sevenSegConfig;
// End Extern Variables

// Prototypes
void set7Seg(
		bool legA,
		bool legB,
		bool legC,
		bool legD,
		bool legE,
		bool legF,
		bool legG
);
void initialize7SegLegs(void);
// End Prototypes

#endif
