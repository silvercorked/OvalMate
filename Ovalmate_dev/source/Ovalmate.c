/*
 * Title: OvalMate_DevBoard
 * Version: 0.0.2
 * Filename: LPC55S16_Project_main_dev.c
 * Authors: Alex Wissing
 * Purpose of Program: Operate a LPC55S16-EVK devlopment board
 * 		with the needed capabilities for the OvalMate project
 * 		(USB device, PWM to 2 steppers and a servo motor, Bump switch interrupts,
 * 		LED handling for error codes and status codes via 7-segment display).
 * How Program is Run on Target System:
 * Date Started: 2/5/2022
 * Update History:
 * 	- 2/8/2022:
 * 		Investigated the Steppers further and got an example driving two steppers from SCTIMER in 32-bit mode
 *	- 2/14/2022:
 *		Added ir sensor checking via ADC
 *	- 2/20/2022:
 *		Added 7seg, bump interrupts, and servo via ctimer
 *	- 2/28/2022:
 *		Moved sctimer, ctimer, 7seg, bump interrupts, and ADC into modules and included them in this main file.
 *		Changed sctimer strategy to use 16-bit mode timers. This gives each timer their own frequency counter, but frequency can't be too low (~1000Hz is too slow, ie can't load the clock with a counter val that fits in 16-bits)
 *	- 3/3/2022:
 *		Removed inputs in favor of singlular include witht helper funcs. Modified code for additional test data.
 *		Getting hard faults when using the SCT_H 16-bit high timer. Low timer works fine.
 *	- 3/9/2022:
 *		Modifed example code in main to test ctimer based stepper motor functions.
 *	- 3/14/2022:
 *		Added findHome and drawOval functions. Will likely move them into other files later.
 *	- 3/15/2022:
 *		Tested findHome and drawOval. Both looking good. drawOval is slow due to delays put in for testing. Will need to remove them.
 *	- 3/16/2022:
 *		Removed slows from drawOval. It runs nicely. Started to reformat it as it is rather long and probably doesn't need to be.
 *		It's much shorter and easier to read now. Start messing with the IR sensor and trying to pull values.
 *	- 3/18/2022:
 *		Wrote a function to draw a rectangle with the pen. Not useful later, but for testing our algorithm for finding the sides of a rectangle, it should be useful.
 *		It was not useful. Turns out the Pen's mark is actually more reflective than the white paper. white ~ 10k, black ~ 40k, pen mark ~ 7k.
 *	- 3/19/2022:
 *		Created an initial algorithm for finding the corners of a rectangle. If we can accurately measure the top right, top left, and bottom left corners,
 *		then we should know every position on the document.
 *	-3/20/2022:
 *		Began rearranging functions and placing them in more understandable spaces to avoid cluttering this main file
 *	- 4/1/2022:
 *		Testing localization functions
 *  - 4/6/2022:
 *  	Created functions for finding expected X coordinate as steps.
 *	- 4/9/2022:
 *		Established basic process once USB information has been given (preset blackdots to simulate correct USB behavior.
 *	- 4/10/2022:
 *		Improved method of driving to mark ovals by hiding math calculations under stepper movement to next blackdot.
 *
 */

// MAIN INCLUDE
#include <modules/utils/conversion.h>
#include "modules/mainInclude/mainInclude.h"
#include <stdio.h>
#include "fsl_debug_console.h"  // this gives access to PRINTF for debugging

uint32_t getStepsXforCol(uint32_t col) { // 4mm spacing, 4mm square
	return nanometersToSteps(col * 8000000); // from center of top left
} // no +1 because top left is first col, wheras first row is one off
uint32_t getStepsYforRow(uint32_t row) {
	return nanometersToSteps((row + 1) * 5000000); // 3mm square, 2mm spacing per row
}

/*
 * @brief   Application entry point.
 */
int main(void) {

	/* Init board hardware. */
	BOARD_InitBootPins();
	BOARD_InitBootClocks();
	BOARD_InitBootPeripherals();
#ifndef BOARD_INIT_DEBUG_CONSOLE_PERIPHERAL
	/* Init FSL debug console. */
	BOARD_InitDebugConsole();
#endif
	status_t ok = kStatus_Success;
	PRINTF("Hello World\n");
	//USB_DeviceApplicationInit(); // done in BOARD_InitBootPeripherals()
	// init

	ok = configure(); // from MainInclude. This sets the base for all function pointers
	// end init
	if (ok != kStatus_Success) {
		PRINTF("\r\n failed to configure");
		return 0;
	}

	// drive servo to default position
	SERVO_setupPWM();
	SERVO_startPWM();

	SERVO_setPenMode(PENUP); // begin with pen high
	delay20ms();

//	for (uint8_t i = 0; i < 24; i++) {
//		BLACKDOTS_setPoint(0, i); // mark all in first column
//	}
//
//	for (uint8_t i = 0; i < 24; i++) {
//		blackDotCoordinate_s dot = {};
//		BLACKDOTS_getNext(&dot);
//		PRINTF("\r\n next: (row: %d, col: %d)", dot.row, dot.col);
//		BLACKDOTS_clearPoint(dot.col, dot.row);
//	}

//	for (uint8_t x = 0; x < 24; x++) {
//		for (uint8_t y = 1; y < 50; y++) {
//			if ((x == 6 || x == 18) && y % 2 == 0) {
//				BLACKDOTS_setPoint(x, y); // mark evens in col 6 and 18
//			}
//			else if ((x == 0 || x == 12) && y % 2 == 1) {
//				BLACKDOTS_setPoint(x, y); // marks odds in col 0 and 12
//			}
//		}
//	}
	for (uint8_t y = 1; y < 50; y++) { // skip first on testing ballot
		if (y % 2 == 0) continue;
		for (uint8_t x = 0; x < 24; x++) {
			if (x == 0 || x == 6 || x == 12 || x == 18)
				BLACKDOTS_setPoint(x, y); // mark all in first column
		}
	}

	SEVENSEG_setLegs_F(); // finding home
	ok = findHome();
	if (ok != kStatus_Success) {
		PRINTF("Failed to Find Home");
		SEVENSEG_displayErrorCode(0x7F); // 0x7F = failed to find home
		return 0;
	}
	SEVENSEG_setLegs_D(); // finding document corners
	ok = findDocumentCorners();
	if (ok != kStatus_Success) {
		STEPPERS_moveBothToNoAccel(5000, 3000); // reset for next run
		SEVENSEG_displayErrorCode(0x7E); // 0x7E = failed to find document corners
		STEPPERS_sleepMotor(stepperX_p);
		STEPPERS_sleepMotor(stepperY_p);
		return 0;
	}

	PRINTF("\r\n\r\n ready to find black dots");

	// printing dots
	blackDotCoordinate_s dot = {};
	uint32_t topLeftXDestination; uint32_t topLeftYDestination;
	uint32_t topRightXDestination; uint32_t topRightYDestintation;
	uint32_t bottomLeftXDestination; uint32_t bottomLeftYDestination;
	float xBallotPercent; float yBallotPercent;
	uint32_t xPos; uint32_t yPos;
	bool marksComplete = false;
	STEPPERS_setCurrentMode(PENMODE);

	SEVENSEG_setLegs_C(); // finding first location
	BLACKDOTS_getNext(&dot);
	if (dot.row != 0xFF) {
		topLeftXDestination = topLeft.x + getStepsXforCol(dot.col);
		topLeftYDestination = topLeft.y + getStepsYforRow(dot.row); // similar result to trYd
		topRightXDestination = topRight.x - getStepsXforCol(23 - dot.col); // 23 = max value col
		topRightYDestintation = topRight.y + getStepsYforRow(dot.row); // similar result to tlYd
		bottomLeftXDestination = bottomLeft.x + getStepsXforCol(dot.col) + 230; // similar result to tlXd
		bottomLeftYDestination = (bottomLeft.y - getStepsYforRow(49 - dot.row)) + 1340; // 49 = max value row
		xBallotPercent = (dot.col / (float) 24);
		yBallotPercent = (dot.row / (float) 50);
		xPos = (uint32_t) ((topLeftXDestination * (1.0f - xBallotPercent) + topRightXDestination * xBallotPercent)
			* (1.0f - yBallotPercent)
			) + bottomLeftXDestination * yBallotPercent;
		yPos = (uint32_t) ((topLeftYDestination * (1.0f - xBallotPercent) + topRightYDestintation * xBallotPercent)
			* (1.0f - yBallotPercent)
			) + bottomLeftYDestination * yBallotPercent;
		while(!marksComplete) {
			PRINTF("\r\n xPos %d, yPos %d, xPos adjusted %d, yPos adjusted %d \r\n    topLeftXD %d, topLeftYD %d \r\n    topRightXD %d, topRightYD %d \r\n    bottomLeftXD %d, bottomLeftYD %d \r\n", xPos, yPos, xPos - PENIROFFSETX, yPos - PENIROFFSETY, topLeftXDestination, topLeftYDestination, topRightXDestination, topRightYDestintation, bottomLeftXDestination, bottomLeftYDestination);
			STEPPERS_moveBothToAccelNoBlock(
				//topLeftXDestination,
				xPos - PENIROFFSETX,
				yPos - PENIROFFSETY
			); // drive but dont block and calculate next position
			BLACKDOTS_clearPoint(dot.col, dot.row);
			PRINTF("col: %d, row: %d\r\n", dot.col, dot.row);
			BLACKDOTS_getNext(&dot);
			if (dot.row == 0xFF) // job done
				marksComplete = true; // will end at the end of this while loop, math is pointless when this is true
			topLeftXDestination = topLeft.x + getStepsXforCol(dot.col);
			topLeftYDestination = topLeft.y + getStepsYforRow(dot.row); // similar result to trYd
			topRightXDestination = topRight.x - getStepsXforCol(23 - dot.col); // 23 = max value col
			topRightYDestintation = topRight.y + getStepsYforRow(dot.row); // similar result to tlYd
			bottomLeftXDestination = bottomLeft.x + getStepsXforCol(dot.col) + 220; // similar result to tlXd
			bottomLeftYDestination = (bottomLeft.y - getStepsYforRow(49 - dot.row)) + 1340; // 49 = max value row
			xBallotPercent = (dot.col / (float) 24);
			yBallotPercent = (dot.row / (float) 50);
			xPos = (uint32_t) ((topLeftXDestination * (1.0f - xBallotPercent) + topRightXDestination * xBallotPercent)
				* (1.0f - yBallotPercent)
				) + bottomLeftXDestination * yBallotPercent;
			yPos = (uint32_t) ((topLeftYDestination * (1.0f - xBallotPercent) + topRightYDestintation * xBallotPercent)
				* (1.0f - yBallotPercent)
				) + bottomLeftYDestination * yBallotPercent;
			while (stepperX_p->status.running || stepperY_p->status.running); // calculate next while driving, then mark
			SEVENSEG_setLegs_B(); // marking oval
			SERVO_setPenMode(PENDOWN);
			delay20ms();
			ADVSTEPPERS_drawOval();
			SERVO_setPenMode(PENUP);
			SEVENSEG_setLegs_C(); // driving to next position
			delay20ms();
		}
	}
	SEVENSEG_setLegs_A(); // moving to ready position
	STEPPERS_switchMode(); // IRMODE
	delay20ms();
	STEPPERS_moveBothToAccel(6000, 3000); // reset for next run
	SEVENSEG_setLegs_0(); // job complete

	STEPPERS_sleepMotor(stepperX_p);
	STEPPERS_sleepMotor(stepperY_p);

	while(1);

	return 0;
}

// Top Left Rect
// x: 5.95
// y: 12.05 from top

// Top Right Last Col (10mm from right side)
// x: 215.90 - (13.95 + 2) // 199.95mm
// y: 12.05

// 4mm white space on cols
// 4mm thick square on cols

// 3mm black quare on rows
// 3.5 mm white space on rows

// Bottom left
// x: 5.95
// y: 15.55 + (70 * 6.5) + 1.5 // 482.6 - (9.05 + 1.5) // 472.05

// scale factor on similar triangles = 472.05 / 199.95 = 2.36084021005
// take the triangle formed by the found locations and the point directly horizontal the top left rect
// and directly vertical the top right last col
// call it 1
// take the triangle fromed by the top left rect, bottom left rect (position unknown), and the point
// directly vertical the top left rect and directly horizontal the bottom left rect
// call it 2
//    /|c
//  k/ | y
// a/t_|b
//    x
// c is top left rect. a is position of reference element (top right last col or bottom left rect).
// b is the expected position, (there shouldn't be a change in one axis given perfect printing and alignment, but those errors produce the point c
// k is the known distance these points are seperated by
// x is the measured x component of the distance k
// y is the measured y component of the distance k (knowing these gives the angle t)

// x2 = x1 * scale factor (2.36084)
// y2 = y1 * scale factor (2.36084)
