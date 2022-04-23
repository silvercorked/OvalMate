/**
 * Title: OvalMate_stepper_module_DevBoard
 * Version: 0.0.1
 * Filename: LPC55S16_Project_main_dev.c
 * Authors: Alex Wissing
 * Purpose of Program: Complex behaviors and shape drawing using the Stepper motors, the IR sensor, and the Servo motor
 * How Program is Run on Target System:
 * Date Started: 2/28/2022
 * Update History:
 *	- 3/22/2022
 *		Moved drawOval into it's own space, seperate from mainInclude
 *	- 3/23/2022
 *		Created debugging function drawRectangle
 *		Turned out to not be very useful because the IR received from the drawn Rectangle was less than the white paper. Weird.
 *	- 3/31/2022
 *		Added prefix to advanced stepper functions
 */

#ifndef ADVANCEDSTEPPERS_H
#define ADVANCEDSTEPPERS_H

// Includes
#include <modules/utils/conversion.h>
#include "modules/irSensor/irSensor.h"
#include "modules/servo/servo.h"
#include "modules/steppers/steppers.h"
// End Includes

// Definitions
#define OVALLINES				14
#define OVALLINESHALF			(OVALLINES / 2)
// End Definitions

// Structs
typedef struct {
	uint32_t x;
	uint32_t y;
} point_s;
typedef struct {
	uint32_t x;
	uint32_t y;
	uint32_t value;
} sample_s;
// End Structs

// Prototypes
void ADVSTEPPERS_drawOval(void);
void ADVSTEPPERS_drawRectangle(void);
status_t ADVSTEPPERS_moveRelative1KHz(stepperMotor_s*, int32_t);
// End Prototypes

#endif
