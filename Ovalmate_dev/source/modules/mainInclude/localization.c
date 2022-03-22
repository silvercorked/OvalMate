

#include "localization.h"

sample_s pastSamples[100];

status_t findDocumentCorners() {
	// presume findHome has been called and home set
	STEPPERS_moveBothToNoAccel(0, 0);

	sample_s backgroundSample = {
		.x = stepperX_p->position,
		.y = stepperY_p->position,
		.value = IRSENSOR_readAvgADC(5000)
	}; // quick sample of background color

	PRINTF("backgroundSample val: %d", backgroundSample.value);
	// move diagonal but stop when ADC reads angled bracket for lining up document
	STEPPERS_moveRelativeNoAccelNoBlock(stepperX_p, 10000);
	STEPPERS_moveRelativeNoAccelNoBlock(stepperY_p, 10000);

	// wrapping issue
	while (!isAlignmentBracket()) {
		if (!(stepperX_p->status.running || stepperY_p->status.running)) {
			PRINTF("failed to find bracket");
			return kStatus_Fail;	// if motors stopped, fail
		}
	}
	//PRINTF("currentSample val: %d", s);
	// when we break out, reading new unique value
	STEPPERS_stopMotor(stepperX_p);
	STEPPERS_stopMotor(stepperY_p);

	//TODO
	STEPPERS_moveBothRelativeNoAccelNoBlock(10000, 10000);
	//s = (uint32_t) IRSENSOR_readAvgADC(1000);

}

status_t followBlackSquares(stepperMotor_s* majorAxis) {
	pollADC(stepperX_p, 100);
	// TODO
}

/**
 * Attempts to find 4 points of a rectangle using the ADC
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
			if (pastSamples[i].value > 27000U) {
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
	while (i < 100) {
		if (pastSamples[i].value < 24000U) {
			corner_p->x = pastSamples[i - 1].x;
			corner_p->y = pastSamples[i - 1].y;
			corner_p->value = pastSamples[i - 1].value;
			break;
		}
		i++;
	}
	return kStatus_Success;
}

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

void pollADC(stepperMotor_s* motor_p, int32_t steps) {
	uint32_t stepsInc = steps / 100;
	for (uint32_t i = 0; i < 100; i++) {
		sample_s s = {
			.x = stepperX_p->position,
			.y = stepperY_p->position,
			.value = IRSENSOR_readAvgADC(1000)
		};
		pastSamples[i] = s;
		STEPPERS_moveRelativeNoAccel(motor_p, stepsInc);
	}
}

bool isAlignmentBracket() {
	return IRSENSOR_readAvgADC(1000) > 50000;
}

bool isBlack() {
	return IRSENSOR_readAvgADC(1000) > 30000.0;
}

bool isWhite() {
	double temp = IRSENSOR_readAvgADC(1000);
	return temp > 7000 && temp < 20000;
}
