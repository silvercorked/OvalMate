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
 */

// Includes
#include "fsl_gpio.h"
#include <stdbool.h>
// End Includes

// Structs
typedef struct __pin_information {
	int pin;
	int port;
} pinInformation;
// End Structs

// Extern Variables
// Variables
extern pinInformation legA;
extern pinInformation legB;
extern pinInformation legC;
extern pinInformation legD;
extern pinInformation legE;
extern pinInformation legF;
extern pinInformation legG;
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
