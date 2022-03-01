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

#include "sevenSegmentLED.h"

// Variables
pinInformation legA = { .port = 0, .pin = 4 }; // demo green led port 1 pin 7
pinInformation legB = { .port = 0, .pin = 5 };
pinInformation legC = { .port = 0, .pin = 6 };
pinInformation legD = { .port = 0, .pin = 8 };
pinInformation legE = { .port = 0, .pin = 13 };
pinInformation legF = { .port = 0, .pin = 14 };
pinInformation legG = { .port = 0, .pin = 18 };
	// leg a w/ logic low = 1.6v, logic high = 3.3v (this doesn't activate the LED).
	// leg b w/ logic low = 0v, logic high = 1.6v (this does activate the LED).
gpio_pin_config_t sevenSegConfig = { .pinDirection = kGPIO_DigitalOutput, .outputLogic = 0};
// End Variables

// Functions
void set7Seg(bool a, bool b, bool c, bool d, bool e, bool f, bool g) {
	GPIO_PinWrite(GPIO, legA.port, legA.pin, a); // write bool value to pin
	GPIO_PinWrite(GPIO, legB.port, legB.pin, b); // 1 = high
	GPIO_PinWrite(GPIO, legC.port, legC.pin, c); // 0 = low
	GPIO_PinWrite(GPIO, legD.port, legD.pin, d);
	GPIO_PinWrite(GPIO, legE.port, legE.pin, e);
	GPIO_PinWrite(GPIO, legF.port, legF.pin, f);
	GPIO_PinWrite(GPIO, legG.port, legG.pin, g);
}

void initialize7SegLegs() {
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