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
 *	- 3/14/2022:
 *		modified seven seg function names to better represent the modular approach
 *		Added displayErrorCode function. Takes a 8 bit val and uses the first 7 bits as flags to set legs, 0th pos -> a leg, 6th pos -> g leg
 */

#include "sevenSegmentLED.h"

// Variables
pinInformation_s legA = { .port = 0, .pin = 4 }; // demo green led port 1 pin 7
pinInformation_s legB = { .port = 0, .pin = 5 };
pinInformation_s legC = { .port = 0, .pin = 6 };
pinInformation_s legD = { .port = 0, .pin = 8 };
pinInformation_s legE = { .port = 0, .pin = 13 };
pinInformation_s legF = { .port = 0, .pin = 14 };
pinInformation_s legG = { .port = 0, .pin = 18 };
	// leg a w/ logic low = 1.6v, logic high = 3.3v (this doesn't activate the LED).
	// leg b w/ logic low = 0v, logic high = 1.6v (this does activate the LED).
gpio_pin_config_t sevenSegConfig = { .pinDirection = kGPIO_DigitalOutput, .outputLogic = 0};
// End Variables

// Functions
void SEVENSEG_setLegs(bool a, bool b, bool c, bool d, bool e, bool f, bool g) {
	GPIO_PinWrite(GPIO, legA.port, legA.pin, a); // write bool value to pin
	GPIO_PinWrite(GPIO, legB.port, legB.pin, b); // 1 = high
	GPIO_PinWrite(GPIO, legC.port, legC.pin, c); // 0 = low
	GPIO_PinWrite(GPIO, legD.port, legD.pin, d);
	GPIO_PinWrite(GPIO, legE.port, legE.pin, e);
	GPIO_PinWrite(GPIO, legF.port, legF.pin, f);
	GPIO_PinWrite(GPIO, legG.port, legG.pin, g);
}

void SEVENSEG_displayErrorCode(uint8_t errorCode) {
	bool a = errorCode & (1 << 0);
	bool b = errorCode & (1 << 1);
	bool c = errorCode & (1 << 2);
	bool d = errorCode & (1 << 3);
	bool e = errorCode & (1 << 4);
	bool f = errorCode & (1 << 5);
	bool g = errorCode & (1 << 6);
	SEVENSEG_setLegs(a, b, c, d, e, f, g);
}

void SEVENSEG_initializeLegs() {
    /* Init output LED GPIO. */
    //GPIO_PortInit(GPIO, legA.port);
    GPIO_PinInit(GPIO, legA.port, legA.pin, &sevenSegConfig);
	GPIO_PinInit(GPIO, legB.port, legB.pin, &sevenSegConfig);
	GPIO_PinInit(GPIO, legC.port, legC.pin, &sevenSegConfig);
	GPIO_PinInit(GPIO, legD.port, legD.pin, &sevenSegConfig);
	GPIO_PinInit(GPIO, legE.port, legE.pin, &sevenSegConfig);
	GPIO_PinInit(GPIO, legF.port, legF.pin, &sevenSegConfig);
	GPIO_PinInit(GPIO, legG.port, legG.pin, &sevenSegConfig);
}
// End Functions
