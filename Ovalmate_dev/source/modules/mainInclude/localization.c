/**
 * Title: OvalMate_stepper_module_DevBoard
 * Version: 0.0.1
 * Filename: LPC55S16_Project_main_dev.c
 * Authors: Alex Wissing
 * Purpose of Program: Find Pen location and deduce location of document artifacts
 * How Program is Run on Target System:
 * Date Started: 3/26/2022
 * Update History:
 *	- 3/26/2022
 *		Setup localization with findHome and pollADC
 *	- 3/27/2022
 *		Added polling over an ADC range (replace only part of the 100 sample range with new range.
 *		Added functions for finding different patterns in polledSample range
 *	- 3/28/2022
 *		Setup functions for finding rectangles and getting a center point given the 4 corners of a rectangle
 *	- 3/29/2022
 *		Created functions to check if we are above certain IR values which represent white paper, black paper, and the mounting brackets
 *	- 3/30/2022
 *		Editing findDocumentCorners to locate the top left and top right rectangles, then verify with the bottom left rectangle
 *	- 4/2/2022
 *		For some reason, the "center" of the top right rectangle is several steps to the right. Top Left is spot on. Likely a bug with
 *		the findRectangleCorners function. Adding function to calculate "error" that can be used to subtract from found "center."
 *	- 4/4/2022
 *		Couldn't find issue with finding top right rectangle center. Comparing the points found from the topLeft and topRight, topRight is noticably smaller,
 *		so perhaps the reading on the end is incorrect.
 *	- 4/6/2022
 *		Looking for bottomLeft corner based on top right and top left via similar triangles. It seems off in both x and y, once again, consistently. Applying
 *		a corrective value may resolve this, but is much less satisfying than the math working out
 *	- 4/8/2022
 *		Created functions to find peaks and valleys in peakSamples array. Created functions to segregate IRSensor values into specific colors.
 *		Corrected and established a baseline for findDocumentCorners. Can now somewhat consistently find top left and top right corners.
 */

#include "localization.h"

sample_s pastSamples[100];
point_s topLeft;
point_s topRight;
point_s bottomLeft;
bool buttonCallback_stepperMotorX_stopped = false;
bool buttonCallback_stepperMotorY_stopped = false;

/**
 * The estimations of centers appears to be off but consistently off.
 * Attempts to corrective sum for error produced in measure centers in the X direction.
 * The value produced is the error correction for locating the right side of the rectangle.
 *
 * @params	- uint8_t x		: the column which is being found.
 * @return	- int32_t		: the corrective sum.
 */
int32_t errorStepsFromLeft(uint8_t x) { // for fixing x error on rect readings
	return nanometersToSteps(40319 * x + 2001180); // 0.0403194 * x + 2.00118
}

/**
 * The estimations of centers appears to be off but consistently off.
 * subtracts 2mm from errorStepsFromLeftDistance to allow compaarison with center estimates.
 *
 * @params	- uint8_t x		: the column which is being found.
 * @return	- int32_t		: the corrective sum.
 */
int32_t correctiveSteps(uint8_t x) {
	return nanometersToSteps(2000000) - errorStepsFromLeft(x);
}

/**
 * Attempts to find the topLeft, topRight, and bottomLeft corner centers and store them in three global point structures.
 *
 * @params	- None
 * @return	- status_t		: kStatus_Success if corners would found, kStatus_Fail otherwise.
 */
status_t findDocumentCorners() {
	// presume findHome has been called and home set
	STEPPERS_moveBothToNoAccel(0, 0);
	int32_t errorInX0 = correctiveSteps(0);
	int32_t errorInX24 = correctiveSteps(24);
	sample_s backgroundSample = {
		.x = stepperX_p->position,
		.y = stepperY_p->position,
		.value = IRSENSOR_readAvgADC(5000)
	}; // quick sample of background color

	PRINTF("backgroundSample val: %d", backgroundSample.value);
	// move diagonal but stop when ADC reads angled bracket for lining up document
	STEPPERS_moveBothRelativeNoAccelNoBlock(10000, 10000);
	while (!aboveAlignmentBracket() && (stepperX_p->status.running || stepperY_p->status.running)) {
		if (!(stepperX_p->status.running || stepperY_p->status.running)) {
			PRINTF("failed to find bracket");
			return kStatus_Fail;	// if motors stopped, fail
		}
	}
	STEPPERS_stopMotor(stepperX_p);
	STEPPERS_stopMotor(stepperY_p);
	PRINTF("\r\n should be above angle bracket");
	// drive down until we reach otherside of alignment bracket
	STEPPERS_moveRelativeNoAccelNoBlock(stepperY_p, 10000);
	while (aboveAlignmentBracket() && stepperY_p->status.running) {
		if (!stepperY_p->status.running) {
			PRINTF("failed to drive past alignment bracket");
			return kStatus_Fail;
		}
	} // continue drive until we reach white paper
	while (!aboveWhite() && stepperY_p->status.running) {
		if (!stepperY_p->status.running) {
			PRINTF("failed to find white under bracket");
			return kStatus_Fail;
		}
	}
	STEPPERS_stopMotor(stepperY_p);
	PRINTF("\r\n should be above white");

	STEPPERS_moveBothRelativeNoAccel(650, 580); // get closer to rectangle (col 0, row -1)

	// find rectangle center and mark
	point_s rectangleCorners[4];
	status_t status = findRectangleCorners(rectangleCorners, 4);
	PRINTF("\r\n status: %c", status == kStatus_Fail ? 'F' : 'S');
	if (status == kStatus_Fail)
		return status;
	status = getCenterFromRectCorners(rectangleCorners, 4, &topLeft);
	if (status == kStatus_Fail)
		return status;
	PRINTF("\r\n center.x: %d, center.y: %d, status: %c", topLeft.x, topLeft.y, status == kStatus_Fail ? 'F' : 'S');
	PRINTF("\r\n corrected center.x: %d, center.y: %d,", topLeft.x + errorInX0, topLeft.y);
	STEPPERS_moveBothToNoAccel(topLeft.x + errorInX0, topLeft.y);
	STEPPERS_switchMode(); // PENMODE
	SERVO_setPenMode(PENDOWN);
	delay20ms();
	STEPPERS_moveRelativeNoAccel(stepperY_p, 500);
	STEPPERS_moveRelativeNoAccel(stepperY_p, -500);
	STEPPERS_moveRelativeNoAccel(stepperX_p, 500);
	STEPPERS_moveRelativeNoAccel(stepperX_p, -500);
	SERVO_setPenMode(PENUP);
	delay20ms();
	STEPPERS_switchMode(); // IRMODE
	delay20ms();

	// move above found center and drive to other side of ballot
	STEPPERS_moveBothRelativeAccel(17000, -300);

	uint32_t totalX = 2000;
	sample_s right = { .x = 0, .y = 0, .value = 0xFFFF };
	while (1) {
		pollADC(stepperX_p, 200);
		for (uint32_t i = 0; i < 100; i++) {
			if (isBlack(pastSamples[i].value)) {
				STEPPERS_moveBothToNoAccel(pastSamples[i].x, pastSamples[i].y);
				right = pastSamples[i];
				break;
			}
		}
		if (right.value != 0xFFFF || totalX <= 0) {
			break;
		}
		totalX -= 200;
	}
	if (right.value == 0xFFFF) {// right not set
		PRINTF("\r\n right sample not set");
		return kStatus_Fail;
	}

	uint32_t totalY = 2000;
	sample_s top = { .x = 0, .y = 0, .value = 0xFFFF };
	STEPPERS_moveRelativeNoAccel(stepperX_p, -200);
	while (1) {
		pollADC(stepperY_p, -200);
		for (uint32_t i = 0; i < 100; i++) {
			if (isBlack(pastSamples[i].value)) {
				STEPPERS_moveBothToNoAccel(pastSamples[i].x, pastSamples[i].y);
				top = pastSamples[i];
				break;
			}
		}
		if (top.value != 0xFFFF || totalY <= 0) {
			break;
		}
		totalY -= 200;
	}
	if (top.value == 0xFFFF) {// top not set
		PRINTF("\r\n top sample not set");
		return kStatus_Fail;
	}
	PRINTF("\r\n\r\n r.x: %d, r.y: %d, t.x: %d, t.y: %d", right.x, right.y, top.x, top.y);
	point_s corner = { .x = top.x, .y = right.y };

	// based on corner location, drive to second black rectangle (col 23, row -1)
	STEPPERS_moveBothToNoAccel(corner.x - 1212, corner.y + 100); // 15 mm offset on x // getting closer on y
	point_s rectangleCorners2[4];
	status = findRectangleCorners(rectangleCorners2, 4);
	PRINTF("status: %c", status == kStatus_Fail ? 'F' : 'S');
	if (status == kStatus_Fail)
		return status;
	status = getCenterFromRectCorners(rectangleCorners2, 4, &topRight);
	if (status == kStatus_Fail)
		return status;
	PRINTF("\r\n center.x: %d, center.y: %d, status: %c", topRight.x, topRight.y, status == kStatus_Fail ? 'F' : 'S');
	PRINTF("\r\n corrected center.x: %d, center.y: %d,", topRight.x + errorInX24, topRight.y);
	topRight.x = topRight.x + errorInX24;
	STEPPERS_moveBothToNoAccel(topRight.x, topRight.y);
	STEPPERS_switchMode(); // PENMODE
	SERVO_setPenMode(PENDOWN);
	delay20ms();
	STEPPERS_moveRelativeNoAccel(stepperY_p, 500);
	STEPPERS_moveRelativeNoAccel(stepperY_p, -500);
	STEPPERS_moveRelativeNoAccel(stepperX_p, 500);
	STEPPERS_moveRelativeNoAccel(stepperX_p, -500);
	SERVO_setPenMode(PENUP);
	delay20ms();
	STEPPERS_switchMode(); // IRMODE
	delay20ms();

	// calculate location of bottom left (col 0, row 50
	uint32_t diffX = (topRight.x - topLeft.x);
	int32_t diffY = -(topRight.y - topLeft.y);
	int32_t xOffsetExpected = (int32_t) (diffY * SCALEFACTOR);
	bottomLeft.x = (xOffsetExpected) + (topLeft.x);
	int32_t yOffsetExpected = (int32_t) (diffX * SCALEFACTOR);
	bottomLeft.y = yOffsetExpected + topLeft.y;
	PRINTF("\r\n X: %d, Y: %d", bottomLeft.x, bottomLeft.y);

	return kStatus_Success;

}

/**
 * Tries to find the Home position (top left) in the plotter space by moving in a
 * diagonal. The plotter does not know where it is when this function is running,
 * so it will not avoid any obstacles. It must be positioned properly before power up.
 *
 * @params	- None
 * @return	- status_t		: kStatus_Success if the plotter hits the top left corner, kStatus_Fail if motors are inoperable.
 */
status_t findHome() {
	status_t status1 = kStatus_Success;
	status_t status2 = kStatus_Success;
	while (1) {
		if (!buttonCallback_stepperMotorX_stopped)
			status1 = STEPPERS_moveRelativeNoAccelNoBlock(stepperX_p, -10000);	// left interrupt expected
		if (!buttonCallback_stepperMotorY_stopped)
			status2 = STEPPERS_moveRelativeNoAccelNoBlock(stepperY_p, -10000);	// up interrupt expected
		if (status1 == kStatus_Fail || status2 == kStatus_Fail) return kStatus_Fail;
		while (stepperX_p->status.running && stepperY_p->status.running);
		if (buttonCallback_stepperMotorX_stopped && buttonCallback_stepperMotorY_stopped) {
			STEPPERS_setHome(stepperX_p);
			STEPPERS_setHome(stepperY_p);
			return kStatus_Success;
		}
		else {
			while (stepperX_p->status.running && stepperY_p->status.running);
		}
	}
}

/**
 * Tries to find the Home position (top left) in the plotter space by moving in a
 * diagonal. The plotter does not know where it is when this function is running,
 * so it will not avoid any obstacles. It must be positioned properly before power up.
 *
 * @params	- point_s[]	rectPoints		: an array of 4 point_s that represent the corners of the rectangle in question.
 * @params	- uint8_t	size			: the size of the array (if it isn't 4, this always returns kStatus_Fail).
 * @return	- status_t					: kStatus_Success if the rectangle's center was found, kStatus_Fail otherwise.
 */
status_t findRectangleCorners(point_s rectPoints[], uint8_t size) {
	if (size != 4) {
		return kStatus_Fail; // failure. the rect points will be used to set the points in the array. If it isn't the right size, we can't use it
	}
	// presume above rectangle
	sample_s upRight;
	sample_s downRight;
	sample_s downLeft;
	sample_s upLeft;

	status_t runningStatus = findRectangleCorner(&upRight, false, true, false, false, false);
	PRINTF("\r\n upRight.x: %d, upRight.y: %d, upRight.value: %d", upRight.x, upRight.y, upRight.value);
	if (runningStatus != kStatus_Success)
		return runningStatus;
	STEPPERS_moveBothToNoAccel(upRight.x, upRight.y + 10); // go to corner, but more towards middle of rect
	runningStatus = findRectangleCorner(&downRight, false, false, true, false, false);
	PRINTF("\r\n downRight.x: %d, downRight.y: %d, downRight.value: %d", downRight.x, downRight.y, downRight.value);
	if (runningStatus != kStatus_Success)
			return runningStatus;
	STEPPERS_moveBothToNoAccel(downRight.x, downRight.y + 10); // go to corner, but more towards middle of rect
	runningStatus = findRectangleCorner(&downLeft, false, false, false, true, false);
	PRINTF("\r\n downLeft.x: %d, downLeft.y: %d, downLeft.value: %d", downLeft.x, downLeft.y, downLeft.value);
	if (runningStatus != kStatus_Success)
			return runningStatus;
	STEPPERS_moveBothToNoAccel(downLeft.x, downLeft.y + 10); // go to corner, but more towards middle of rect
	runningStatus = findRectangleCorner(&upLeft, false, false, false, false, true);
	PRINTF("\r\n upLeft.x: %d, upLeft.y: %d, upLeft.value: %d", upLeft.x, upLeft.y, upLeft.value);
	if (runningStatus != kStatus_Success)
			return runningStatus;
	rectPoints[0].x = upRight.x; rectPoints[0].y = upRight.y;
	rectPoints[1].x = downRight.x; rectPoints[1].y = downRight.y;
	rectPoints[2].x = downLeft.x; rectPoints[2].y = downLeft.y;
	rectPoints[3].x = upLeft.x; rectPoints[3].y = upLeft.y;
	//STEPPERS_sleepMotor(stepperX_p);
	//STEPPERS_sleepMotor(stepperY_p);
	//while (1);

	return kStatus_Success;
}

/**
 * Tries to find one corner of a rectangle. The corner it attempts to find depends on the booleans passed in.
 *
 * @params	- sample_s*	corner_p		: an empty sample to be filled by this function.
 * @params	- bool		slim			: true if the rectangle being measured is expected to be slim (2x3mm), otherwise (4x3mm)
 * @params	- bool		upRight			: true if the desired corner is the top right corner of the rectangle
 * @params	- bool		downRight		: true if the desired corner is the bottom right corner of the rectangle
 * @params	- bool		downLeft		: true if the desired corner is the bottom left corner of the rectangle
 * @params	- bool		upLeft			: true if the desired corner is the top left corner of the rectangle
 * @return	- status_t		: kStatus_Success if the corner was found, kStatus_Fail otherwise.
 */
status_t findRectangleCorner(sample_s* corner_p, bool slim, bool upRight, bool downRight, bool downLeft, bool upLeft) {
	bool startFound = false;
	sample_s start;
	uint32_t i = 0;
	uint32_t tries = 0;
	while (!startFound) {
		tries++;
		if (tries > 3) {
			return kStatus_Fail; // couldn't find rectangle
		}
		if (upRight) { // presume start above rectangle
			pollADC(stepperY_p, 200); // 200 * 0.01143 = 2.286mm
		}
		else if (downRight) { // presume start to the right of rectangle
			pollADC(stepperX_p, -200); // 200 * 0.01143 = 2.286mm
		}
		else if (downLeft) { // presume start below rectangle
			pollADC(stepperY_p, -200); // 200 * 0.01143 == 2.286mm
		}
		else { // upLeft, presume start to the left of rectangle
			pollADC(stepperX_p, 200); // 200 * 0.01143 = 2.286mm
		}
		i = 0;
		while (i < 100) {
			//PRINTF("\r\n approaching: %d, at: x %d, y %d", pastSamples[i].value, pastSamples[i].x, pastSamples[i].y);
			if (isBlack(pastSamples[i].value)) {
				start = pastSamples[i];
				startFound = true;
				break;
			}
			i++;
		}
	}
	PRINTF("\r\n moving motors to startx: %d, starty: %d", start.x, start.y);
	STEPPERS_moveToNoAccel(stepperX_p, start.x);
	STEPPERS_moveToNoAccel(stepperY_p, start.y);
	while (stepperX_p->status.running || stepperY_p->status.running);
	if (upRight) { // drive right until ADC no longer reads rect
		pollADC(stepperX_p, slim ? 200 : 400); // 400 steps = 0.01143*400 ~~ 4.5mm to the right
	}
	else if (downRight) { // drive down until ADC no longer reads rect
		pollADC(stepperY_p, slim ? 200 : 400); // 400 steps = 0.01143*400 ~~ 4.5mm to the right
	}
	else if (downLeft) { // drive left until ADC no longer reads rect
		pollADC(stepperX_p, slim ? -200 : -400); // 400 steps = 0.01143*400 ~~ 4.5mm to the right
	}
	else { // upLeft, drive up until ADC no longer reads rect
		pollADC(stepperY_p, slim ? -200 : -400); // 400 steps = 0.01143*400 ~~ 4.5mm to the right
	}
	i = 0;
	startFound = false;
	corner_p->x = pastSamples[99].x;
	corner_p->y = pastSamples[99].y;
	corner_p->value = pastSamples[99].value;
	while (i < 100) {
		//PRINTF("\r\n approaching: %d, at %d, %c", pastSamples[i].value, i, pastSamples[i].value < 330000U ? "S" : "F");
		if (pastSamples[i].value < ISBLACKTHRESHOLD - 3000U) {
			corner_p->x = pastSamples[i - 1].x;
			corner_p->y = pastSamples[i - 1].y;
			corner_p->value = pastSamples[i - 1].value;
			startFound = true;
			break;
		}
		i++;
	}
	return startFound ? kStatus_Success : kStatus_Fail;
}

/**
 * Attempt to calculate the center of the rectangle.
 *
 * @params	- point_s[]	points_arr		: an array of 4 point_s that represent the corners of the rectangle in question.
 * @params	- uint8_t	size			: the size of the array (if it isn't 4, this always returns kStatus_Fail).
 * @params	- point_s*	center			: an empty center to be filled by this function.
 * @return	- status_t					: kStatus_Success if the rectangle's center was found, kStatus_Fail otherwise.
 */
status_t getCenterFromRectCorners(const point_s points_arr[], uint8_t size, point_s* center) {
	if (size != 4) {
		return kStatus_Fail; // failure. the rect points will be used to set the points in the array. If it isn't the right size, we can't use it
	}
	// points_arr[0] == topRight corner
	// points_arr[1] == bottomRight corner
	// points_arr[2] == bottomLeft corner
	// points_arr[3] == top
	point_s upRight = points_arr[0];
	point_s downRight = points_arr[1];
	point_s downLeft = points_arr[2];
	point_s upLeft = points_arr[3];
	uint32_t meanUp = (upLeft.y + upRight.y) / 2; // need to find consistent UP. we took 2 samples, so we can use that
	uint32_t meanRight = (upRight.x + downRight.x) / 2;
	uint32_t meanDown = (downLeft.y + downRight.y) / 2;
	uint32_t meanLeft = (upLeft.x + downLeft.x) / 2;

	uint32_t rectCenterX = (meanLeft + meanRight) / 2;
	uint32_t rectCenterY = (meanUp + meanDown) / 2;

	center->x = rectCenterX; center->y = rectCenterY;
	return kStatus_Success;
}

/**
 * Polls the ADC while moving a distance. Utilizes and overwrites the entirety of the pastSamples array.
 *
 * @params	- stepperMotor_s*	motor_p	: the motor to drive while polling.
 * @params	- int32_t			steps	: the number of steps to move over the course of gathering 100 samples.
 * @return	- None
 */
void pollADC(stepperMotor_s* motor_p, int32_t steps) {
	pollADCRange(motor_p, steps, 0, 100);
}

/**
 * Polls the ADC while moving a distance. Utilizes and overwites specified indexes of the pastSamples array.
 *
 * @params	- stepperMotor_s*	motor_p	: the motor to drive while polling.
 * @params	- int32_t			steps	: the number of steps to move over the course of gathering 100 samples.
 * @params	- uint8_t			start	: the start index to store ADC values.
 * @params	- uint8_t			end		: the end index to store ADC values.
 * @return	- None
 */
void pollADCRange(stepperMotor_s* motor_p, int32_t steps, uint8_t start, uint8_t end) {
	if (start >= end || start >= 100 || end > 100)
		return; // fail, don't run.
	uint32_t stepsInc = steps / (end - start);
	for (uint32_t i = start; i < end; i++) {
		sample_s s = {
			.x = stepperX_p->position,
			.y = stepperY_p->position,
			.value = IRSENSOR_readAvgADC(1000)
		};
		pastSamples[i] = s;
		STEPPERS_moveRelativeNoAccel(motor_p, stepsInc);
	}
}

/**
 * !!CURRENTLY UNUSED!!
 * Attempts to find a better center point by polling for a peak using the ADC around.
 *
 * @params	- stepperMotor_s*	motor_p	: the motor to drive while polling.
 * @params	- uint8_t			start	: the start index to store ADC values.
 * @params	- uint8_t			end		: the end index to store ADC values.
 * @return	- uint8_t					: the index of the peak sample.
 */
uint8_t locateBetterCenter(stepperMotor_s* motor_p, uint8_t start, uint8_t end) {
	uint8_t index = -1;
	uint8_t mid = (end - start) / 2;
	if (start > end || start > 100 || end > 100)
		return index;
	int32_t steps = mid * 100;
	pollADCRange(motor_p, steps, start, start + mid); // poll range on desired axis
	STEPPERS_moveRelativeNoAccel(motor_p, -steps); // move back to start
	pollADCRange(motor_p, -steps, start + mid, end); // go in other direction
	STEPPERS_moveRelativeNoAccel(motor_p, steps);
	return findPeak(start, end); // return index of high point to allow caller to recenter on peak
}

/**
 * !!CURRENTLY UNUSED!!
 * Attempts to find a peak within the pastSamples array.
 *
 * @params	- uint8_t			start	: the start index to store ADC values.
 * @params	- uint8_t			end		: the end index to store ADC values.
 * @return	- uint8_t					: the index of the peak sample.
 */
uint8_t findPeak(uint8_t start, uint8_t end) {
	uint8_t index = -1;
	uint16_t max = 0;
	if (start > end || start > 100 || end > 100)
		return index;
	for (uint8_t i = start; i < end; i++) {
		if (pastSamples[i].value > max) {
			index = i;
			max = pastSamples[i].value;
		}
	}
	return index;
}

/**
 * !!CURRENTLY UNUSED!!
 * Attempts to find the beginning of a rise in the pastSamples array.
 *
 * @params	- uint8_t			start	: the start index to store ADC values.
 * @return	- uint8_t					: the index of the start of the rise.
 */
uint8_t findRiseStart(uint8_t start) {
	uint8_t index = -1;
	if (start > 100)
		return index;
	for (uint32_t i = start; i < 100; i++) {
		if (isBlack(pastSamples[i].value)) {
			index = i;
			break;
		}
	}
	return index;
}

/**
 * !!CURRENTLY UNUSED!!
 * Attempts to find the end of a fall in the pastSamples array.
 *
 * @params	- uint8_t			start	: the start index to store ADC values.
 * @return	- uint8_t					: the index of the end of the fall.
 */
uint8_t findFallEnd(uint8_t start) {
	uint8_t index = -1;
	if (start > 100)
		return index;
	for (uint32_t i = start; i < 100; i++) {
		if (!isBlack(pastSamples[i].value)) {
			index = i;
			break;
		}
	}
	return index;
}

/**
 * Polls and checks if the plotter head is currently above the alignment brackets (or IR-based equivalent).
 *
 * @params	- None
 * @return	- bool		: true if above an alignment bracket (or IR equivalent). False otherwise.
 */
bool aboveAlignmentBracket() {
	return isAlignmentBracket((uint32_t) IRSENSOR_readAvgADC(1000));
}

/**
 * Polls and checks if the plotter head is currently above something black (or IR-based equivalent).
 *
 * @params	- None
 * @return	- bool		: true if above something black (or IR equivalent). False otherwise.
 */
bool aboveBlack() {
	return isBlack((uint32_t) IRSENSOR_readAvgADC(1000));
}

/**
 * Polls and checks if the plotter head is currently above something white (or IR-based equivalent).
 *
 * @params	- None
 * @return	- bool		: true if above something white (or IR equivalent). False otherwise.
 */
bool aboveWhite() {
	return isWhite((uint32_t) IRSENSOR_readAvgADC(1000));
}

/**
 * Checks if the given IR adc reading indicates an alignment bracket.
 *
 * @params	- None
 * @return	- bool		: true if adcVal > threshold. False otherwise.
 */
bool isAlignmentBracket(uint32_t adcVal) {
	return adcVal > 50000;
}

/**
 * Checks if the given IR adc reading indicates something black.
 *
 * @params	- None
 * @return	- bool		: true if adcVal > threshold. False otherwise.
 */
bool isBlack(uint32_t adcVal) {
	return adcVal > ISBLACKTHRESHOLD;
}

/**
 * Checks if the given IR adc reading indicates something white.
 *
 * @params	- None
 * @return	- bool		: true if adcVal < thresholdUpper && adcval > thresholdLower. False otherwise.
 */
bool isWhite(uint32_t adcVal) {
	return adcVal > ISWHITELOWERTHRESHOLD && adcVal < ISWHITEUPPERTHRESHOLD;
}

/**
 * buttonCallback function that stops both motors and indicates they both have been stopped by button callbacks.
 *
 * @params	- pint_pin_int_t pintr			: an identifier for which pin interrupt is triggering this function call. (always kPINT_PinInt1)
 * 			- uint32_t pmatch_status		: I actually don't know what this is. Should probably print it out and try to figure it out.
 * @return	- None
 */
void buttonCallback_stopMotors(pint_pin_int_t pintr, uint32_t pmatch_status) {
	PRINTF("\r\nExecuting buttonCallback_stopMotors with params pintr = %d, pmatch_status = %d",
		(uint32_t) pintr,
		pmatch_status
	);
	STEPPERS_stopMotor(stepperX_p);
	STEPPERS_stopMotor(stepperY_p);
	buttonCallback_stepperMotorX_stopped = true;
	buttonCallback_stepperMotorY_stopped = true;
	//stopStepperPWM(MOTORX);
	//stopStepperPWM(MOTORY);
}

/**
 * buttonCallback function that stops motors that affect movement towards or away from the button pressed.
 *
 * @params	- pint_pin_int_t pintr			: an identifier for which pin interrupt is triggering this function call. (always kPINT_PinInt1)
 * 			- uint32_t pmatch_status		: I actually don't know what this is. Should probably print it out and try to figure it out.
 * @return	- None
 */
void buttonCallback_stopRelaventMotor(pint_pin_int_t pintr, uint32_t pmatch_status) {
	PRINTF("\r\nExecuting buttonCallback_stopRelaventMotors with params pintr = %d, pmatch_status = %d",
		(uint32_t) pintr,
		pmatch_status
	);
	// right bump switch (kPINT_PinInt1)
	// left bump switch (kPINT_PinInt2)
	// down bump switch (kPINT_PinInt3)
	// up bump switch (kPINT_PinInt4)
	// emergency bump switch (kPINT_PinInt0)
	if (pintr == kPINT_PinInt1 || pintr == kPINT_PinInt2) {
		STEPPERS_stopMotor(stepperX_p);
		buttonCallback_stepperMotorX_stopped = true;
	}
	else if (pintr == kPINT_PinInt3 || pintr == kPINT_PinInt4) {
		STEPPERS_stopMotor(stepperY_p);
		buttonCallback_stepperMotorY_stopped = true;
	}
	else {
		// emergency pintr or no button interrupt (ie this allows the pints on fault condition to use this still
		STEPPERS_stopMotor(stepperX_p);
		STEPPERS_stopMotor(stepperY_p);
		buttonCallback_stepperMotorX_stopped = true;
		buttonCallback_stepperMotorY_stopped = true;
	}
}
