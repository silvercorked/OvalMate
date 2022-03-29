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
 *
 */

// MAIN INCLUDE
#include "modules/mainInclude/mainInclude.h"
#include "modules/utils/utils.h"
#include <stdio.h>
#include "fsl_debug_console.h"  // this gives access to PRINTF for debugging

uint32_t getbottomleftxcoord(uint32_t measuredY, double scaleFactor) {
	return (uint32_t) (measuredY * scaleFactor);
}

uint32_t getbottomrightycoord(uint32_t measuredX, double scaleFactor) {
	return (uint32_t) (measuredX * scaleFactor);
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

	PRINTF("Hello World\n");
	//USB_DeviceApplicationInit(); // done in BOARD_InitBootPeripherals()
	// testingUSB


	while (1) {
		USB_DeviceTasks(); // nothing runs from this? is it needed? p sure not
	}

	//while (1);
	// init
	configure(); // from MainInclude. This sets the base for all function pointers
	// end init
	//STEPPERS_sleepMotor(stepperX_p);
	//STEPPERS_sleepMotor(stepperY_p);
	/**/

	// give motors job w/ step param
	//setupStepperPWM(&motorX, 20);
	//setupStepperPWM(&motorY, 360); // causes semihost hardfault
	//startStepperPWM(motorX.timer);
	//startStepperPWM(motorY.timer);

	// drive servo to default position
	SERVO_setupPWM();
	SERVO_startPWM();

	SERVO_setPenMode(PENUP); // begin with pen high
	delay20ms();
	//drawOval();

	//STEPPERS_setHome(stepperX_p);
	//STEPPERS_setHome(stepperY_p);
	//stepperX_p->position = 5000;
	//stepperY_p->position = 5000;
	//STEPPERS_sleepMotor(stepperX_p);
	//STEPPERS_sleepMotor(stepperY_p);
	//while(1) {
	//	PRINTF("\r\n val: %d", (uint32_t) IRSENSOR_readAvgADC(1000));
	//}


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
	findHome();

	findDocumentCorners();

	STEPPERS_moveRelativeAccel(stepperX_p, -500);

	STEPPERS_moveRelativeAccel(stepperX_p, 18000);
	pollADC(stepperX_p, 2000);
	sample_s right;
	for (uint32_t i = 0; i < 100; i++) {
		if (pastSamples[i].value < 3400) {
			STEPPERS_moveBothToNoAccel(pastSamples[i].x, pastSamples[i].y);
			right = pastSamples[i];
			break;
		}
		//PRINTF("\r\n%d, %d", pastSamples[i].x, pastSamples[i].value);
	}



	STEPPERS_moveRelativeNoAccel(stepperX_p, -200);
	pollADC(stepperY_p, -2000);
	sample_s top;
	for (uint32_t i = 0; i < 100; i++) {
		if (pastSamples[i].value < 3400) {
			STEPPERS_moveBothToNoAccel(pastSamples[i].x, pastSamples[i].y);
			top = pastSamples[i];
			break;
		}
		//PRINTF("\r\n%d, %d", pastSamples[i].x, pastSamples[i].value);
	}

	PRINTF("\r\n\r\n r.x: %d, r.y: %d, t.x: %d, t.y: %d", right.x, right.y, top.x, top.y);

	point_s corner = { .x = top.x, .y = right.y };

	STEPPERS_moveBothToNoAccel(corner.x - 1312, corner.y); // 15 mm offset on x

	point_s rectangleCorners2[4];
	status_t a2 = findRectangleCorners(rectangleCorners2, 4);
	PRINTF("status: %c", a2 == kStatus_Fail ? 'F' : 'S');
	if (a2 != kStatus_Fail) {
		point_s center;
		a2 = getCenterFromRectCorners(rectangleCorners2, 4, &center);
		PRINTF("\r\n center.x: %d, center.y: %d, status: %c", center.x, center.y, a2 == kStatus_Fail ? 'F' : 'S');
		STEPPERS_moveBothToNoAccel(center.x, center.y);

		STEPPERS_moveBothRelativeNoAccel(2386, -3728); // from IR sensor mode to pen mode;
		SERVO_setPenMode(PENDOWN);
		delay20ms();
		STEPPERS_moveRelativeNoAccel(stepperY_p, 1000);
		STEPPERS_moveRelativeNoAccel(stepperY_p, -1000);
		STEPPERS_moveRelativeNoAccel(stepperX_p, 500);
		STEPPERS_moveRelativeNoAccel(stepperX_p, -500);
		SERVO_setPenMode(PENUP);
	}

	//sample_s samples[100];

	//pollADC(stepperX_p, 1000);
	//for (uint32_t i = 0; i < 100; i++) {
	//	PRINTF("\r\n %d, %d, %d", samples[i].x, samples[i].y, samples[i].value);
	//}
	/*
	bool endOfLine = false;
	while (!endOfLine) {
		pollADC(stepperX_p, 2000);
		uint8_t firstRise = findRiseStart(0); // presuming first peak is best
		if (pastSamples[firstRise].value == -1 || pastSamples[firstRise].value < 20000) {
			PRINTF("No peaks");
			endOfLine = true;
		}
		uint8_t firstFall = findFallEnd(firstRise);
		uint8_t firstPeak = findPeak(firstRise, firstFall);
		uint8_t index = firstFall + 1;
		uint8_t peakIndex = 255;
		while (index < 100) {
			uint8_t rise = findRiseStart(index);
			uint8_t fall = findFallEnd(rise);
			uint8_t peak = findPeak(rise, fall);
			if (peak == 255 || fall == 255)
				break;
			if (pastSamples[firstPeak].value > pastSamples[peak].value + 1000 && peak < 90) {
				STEPPERS_moveBothToNoAccel(pastSamples[peak].x, pastSamples[peak].y);
				peakIndex = locateBetterCenter(stepperY_p, peak, peak + 10);
				if (peakIndex == -1) {
					PRINTF("\r\n no peak found");
				}
				PRINTF("\r\n new center found: %d, x: %d, y: %d", peakIndex, pastSamples[peakIndex].x, pastSamples[peakIndex].y);
				break;
			}
			index = fall + 1;
		}
		if (peakIndex != -1)
			STEPPERS_moveBothToNoAccel(pastSamples[peakIndex].x, pastSamples[peakIndex].y);
	}

	for (uint32_t i = 0; i < 100; i++) {
		PRINTF("\r\n%d, %d", pastSamples[i].x, pastSamples[i].value);
	}
	*/

	STEPPERS_sleepMotor(stepperX_p);
	STEPPERS_sleepMotor(stepperY_p);




	/*while (1) {
		PRINTF("\r\n adcValue: %lf", readAvgADC(1000));
	}*/
	return 0;
}
