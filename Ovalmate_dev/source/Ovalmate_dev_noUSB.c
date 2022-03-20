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

#define ADCVALUETOLERANCE 1000

typedef struct {
	uint32_t x;
	uint32_t y;
	uint32_t value;
} sample_s;

sample_s backgroundSample;
sample_s blackSample;
sample_s whiteSample;

void drawRectangle();
status_t findRectangleCorners(uint32_t**, uint8_t);
void findRectangleCorner(sample_s*, bool, bool, bool, bool, bool);

sample_s pastSamples[100];

/** ballistic behavior. Presumes at center of oval
 * Oval is 3mm x 2mm (width x height). To avoid loss from floating point precision, convert to nanometers and divide oval in half.
 * Half oval = 1.5mm x 1mm = 1500000nm x 1000000nm. Steppers w/ microstepping at 16 = 0.01143mm = 11430nm per step.
 * Half oval = parabola. Rotate 90 degrees for concave downward parabola. Region defined by y > 0 and parabola need to be shaded.
 * width of pen mark + bleed of pen ~= 0.25mm. 1.5mm x 1mm => 263 steps x 175 steps, so 4 lines across width of oval and mirrored on negative side => filled
 * On parabola, equation in mm is y = -1.5 * (x^2) + 1.5. In nm, -1500000 ((x / 1000000)^2) + 1500000.
 */

#define OVALLINES				12
#define OVALLINESHALF			(OVALLINES / 2)
#define OVALLINESTARTPERCENT	5				// percentage of start and end that will be ignored in considering range
#define OVALSTARTOFFSET			OVALLINESTARTPERCENT * 10000U
uint32_t OVALLINEOFFSET =		((1.0 - (1.0 / (OVALLINESTARTPERCENT * 2.0))) * 1000000.0) / OVALLINESHALF; // Take twice percent off for both ends, get range we'll divide based on one side of oval and scale to nm
void drawOval() {
	int8_t k = OVALLINESHALF;				// start value
	int32_t currX = 0; // presume in center of oval
	int32_t currY = 0; // presume in center of oval
	int8_t kInc = -1;

	SERVO_setPenMode(PENDOWN);
	delay20ms();

	while (k <= OVALLINESHALF) { // negate x result if doing negative side
		int32_t x = kInc == -1 ? -(k * OVALLINEOFFSET + OVALSTARTOFFSET) : k * OVALLINEOFFSET + OVALSTARTOFFSET; // 0.15 mm * k + 0.05 mm = 6 evenly spaced divisions across 0-1 given input in space 0-6
		int32_t relativeX = x - currX;
		uint32_t xPosSteps = nanometersToSteps((uint64_t) (relativeX < 0 ? -relativeX : relativeX)); // negative side first, so

		double scaledX = (double) x / 1000000.0L; // scaled to 1mm (1000000 nm)
		uint32_t y = -1500000 * (scaledX * scaledX) + 1500000; // -1.5mm * x^2 + 1.5mm = equation of parabola
		int32_t relativeY = y - currY;
		uint32_t yPosSteps = nanometersToSteps((uint64_t)relativeY);
		STEPPERS_moveRelativeNoAccel(stepperY_p, relativeX > 0 ? xPosSteps : -xPosSteps); // the parabola model is a 90 degree rotation of half the oval. So X coords actually correspond to Y's motor
		STEPPERS_moveRelativeNoAccel(stepperX_p, relativeY > 0 ? -yPosSteps : yPosSteps);

		while (stepperX_p->status.running || stepperY_p->status.running);
		currX = x;
		currY = y; // in position
		uint32_t lineSteps = nanometersToSteps(y << 1);
		STEPPERS_moveRelativeNoAccel(stepperX_p, lineSteps); // y * 2 == line width
		while(stepperX_p->status.running);
		currY = -y;
		k += kInc;
		if (k == -1) {
			kInc = 1;
			k++;
		}
		delay1ms();
	}
	SERVO_setPenMode(PENUP);
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

	// init
	configure(); // from MainInclude. This sets the base for all function pointers
	// end init

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

	STEPPERS_setHome(stepperX_p);
	STEPPERS_setHome(stepperY_p);

	//drawRectangle();
	//sleepMotors();

	uint32_t rectangleCorners[4];
	status_t a = findRectangleCorners(&rectangleCorners, 4);
	PRINTF("status: %c", a == kStatus_Fail ? "F" : "S");
	//sample_s samples[100];

	//pollADC(stepperX_p, 1000);
	//for (uint32_t i = 0; i < 100; i++) {
	//	PRINTF("\r\n %d, %d, %d", samples[i].x, samples[i].y, samples[i].value);
	//}

	STEPPERS_sleepMotor(stepperX_p);
	STEPPERS_sleepMotor(stepperY_p);


	/*while (1) {
		PRINTF("\r\n adcValue: %lf", readAvgADC(1000));
	}*/



	//STEPPERS_moveRelativeAccel(stepperY_p, 4000);
	//while(stepperY_p->status.running);

	/*for (uint32_t i = 0; i < 10; i++) {
		STEPPERS_moveToAccel(stepperX_p, 10000);
		while(stepperX_p->status.running);

		//SERVO_setPenMode(PENUP);
		STEPPERS_moveToAccel(stepperX_p, 0);
		while(stepperX_p->status.running);

		STEPPERS_moveRelativeNoAccel(stepperY_p, 5);
		while (stepperY_p->status.running);
	}
	*/

	/*
	STEPPERS_moveRelativeNoAccel(stepperY_p, 10000);
	uint32_t adcVal = 0;
	while (adcVal < 16000) {
		adcVal = IRSENSOR_getADCValue();
		PRINTF("\r\n adc Val: %d", adcVal);
	}
	STEPPERS_stopMotor(stepperY_p);
	PRINTF("StepperY pos: %d", stepperY_p->position);
	*/


	/*while(1) {
		STEPPERS_moveRelative(stepperY_p, 10000, true);
		while (stepperY_p->status.running);
		STEPPERS_moveRelative(stepperY_p, -10000, true);
		while (stepperY_p->status.running);
	}*/

	/*
	while (1) {
		STEPPERS_moveRelativeAccel(stepperX_p, 1000);
		STEPPERS_moveRelativeAccel(stepperY_p, -1000);
		while (stepperX_p->status.running && stepperY_p->status.running);
		STEPPERS_moveRelativeAccel(stepperX_p, -1000);
		STEPPERS_moveRelativeAccel(stepperY_p, 1000);
		while (stepperX_p->status.running && stepperY_p->status.running);
		delay();
	}*/
	//delay();
	//while(1);

	//findHome();

	//STEPPERS_moveRelative(motor_p, steps, accel)


	/*while (1) {
		STEPPERS_moveRelativeNoAccel(stepperX_p, -100000);
		while(stepperX_p->status.running);
		if (buttonCallback_stepperMotorX_stopped) {
			STEPPERS_setHome(stepperX_p);
			buttonCallback_stepperMotorX_stopped = false; // clear flag
			break;
		}
		// left is currently held high on dev board. has to be dropped low and back up to produce effect
	}
	/*
	while (1) {
		STEPPERS_setHome(stepperX_p);
		STEPPERS_setHome(stepperY_p);
		STEPPERS_moveTo(stepperX_p, 40000);
		STEPPERS_moveTo(stepperY_p, 50000);
		while (stepperX_p->status.running || stepperY_p->status.running);
		delay();
		delay();
	}

	// set 7 seg

//	volatile uint32_t i = 0;
//	while (1) {
//		delay();
//		i++;
//		if (i % 2 == 1) {
//			set7Seg(0, 0, 1, 0, 1, 0, 1);
//			driveStepperPWMNonBlocking(MOTORX, 5000, 100000);
//			updateServoPWMDutyCycle(PENUP);
//		}
//		else {
//			set7Seg(1, 1, 0, 1, 0, 1, 0);
//			driveStepperPWMBlocking(MOTORX, 5000, 100000);
//			updateServoPWMDutyCycle(PENDOWN);
//		}
//		delay();
//		int a = getADCValue();
//		PRINTF("adc val: %d", a);
//	}
//	delay();
//	driveStepperPWMNonBlocking(MOTORBOTH, 5, 5);
//	PRINTF("printing while driving motors");
//	while(1) {
//		USB_DeviceTasks(); // handles usb behavior
//	}

	//delay();
	//driveStepperPWMBlock(sMotorX, 50, 2000);
	//delay();
	//driveStepperPWMBlock(sMotorX, 20, 1500);

	//delay();

	//driveStepperPWMBlock(sMotorBoth, 20, 1000);
	//driveStepperPWMBlock(sMotorBoth, 50, 2000);
	//driveStepperPWMBlock(sMotorBoth, 20, 1000);


	/* Force the counter to be placed into memory. */
//	volatile static int i = 0 ;
//	/* Enter an infinite loop, just incrementing a counter. */
//	while(1) {
//		i++ ;
//		/* 'Dummy' NOP to allow source level single stepping of
//			tight while() loop */
//		delay();
//		if (i % 2 == 1) {
//			setupStepperPWM(&motorX, 20);
//			startStepperPWM(motorX.timer);
//			updateServoPWMDutyCycle(PENDOWN);
//			PRINTF("\r\nPENDOWN");
//			set7Seg(0, 0, 1, 0, 1, 0, 1);
//		}
//		else {
//			updateServoPWMDutyCycle(PENUP);
//			PRINTF("\r\nPENUP");
//			set7Seg(1, 1, 0, 1, 0, 1, 0);
//		}
//		__asm volatile ("nop");
//	}
	return 0;
}

void findCorners() {
	// presume findHome has been called and home set

	STEPPERS_moveToNoAccel(stepperX_p, 0);
	STEPPERS_moveToNoAccel(stepperY_p, 0);
	while (stepperX_p->status.running || stepperY_p->status.running);

	sample_s backgroundSample = {
		.x = stepperX_p->position,
		.y = stepperY_p->position,
		.value = readAvgADC(5000)
	}; // quick sample of background color

	PRINTF("backgroundSample val: %d", backgroundSample.value);
	// move diagonal but stop when ADC reads angled bracket for lining up document
	STEPPERS_moveRelativeNoAccel(stepperX_p, 10000);
	STEPPERS_moveRelativeNoAccel(stepperY_p, 10000);

	uint32_t s = backgroundSample.value;
	// wrapping issue
	while (backgroundSample.value <= s + ADCVALUETOLERANCE && backgroundSample.value <= s + ADCVALUETOLERANCE) {
		s = readAvgADC(1000);
		if (!stepperX_p->status.running) { // keep driving until we find it
			STEPPERS_moveRelativeNoAccel(stepperX_p, 10000);
		}
		else if (!stepperY_p->status.running) {
			STEPPERS_moveRelativeNoAccel(stepperY_p, 10000);
		}
	}
	PRINTF("currentSample val: %d", s);
	// when we break out, reading new unique value
	STEPPERS_stopMotor(stepperX_p);
	STEPPERS_stopMotor(stepperY_p);

	//TODO
}

/**
 * Attempts to find 4 points of a rectangle using the ADC
 */
status_t findRectangleCorners(uint32_t* rectPoints[], uint8_t size) {
	if (size != 4) {
		return kStatus_Fail; // failure. the rect points will be used to set the points in the array. If it isn't the right size, we can't use it
	}
	// presume above rectangle
	sample_s upRight;
	sample_s downRight;
	sample_s downLeft;
	sample_s upLeft;
	findRectangleCorner(&upRight, false, true, false, false, false);
	PRINTF("\r\n upRight.x: %d, upRight.y: %d, upRight.value: %d", upRight.x, upRight.y, upRight.value);
	STEPPERS_moveToNoAccel(stepperX_p, upRight.x);
	STEPPERS_moveToNoAccel(stepperY_p, upRight.y + 10); // go to corner, but more towards middle of rect
	//while(stepperX_p->)
	findRectangleCorner(&downRight, false, false, true, false, false);
	PRINTF("\r\n downRight.x: %d, downRight.y: %d, downRight.value: %d", downRight.x, downRight.y, downRight.value);
	while(1);
	findRectangleCorner(&downLeft, false, false, false, true, false);
	findRectangleCorner(&upLeft, false, false, false, false, true);
	//findUpRight();

	/*PRINTF("\r\n start.x: %d, start.y: %d, start.value: %d, cornerUR.x: %d, cornerUR.y: %d, cornerUR.value: %d, cornerDR.x: %d, cornerDR.y: %d, cornerDR.value: %d\r\n",
		start.x,
		start.y,
		start.value,
		cornerUpRight.x,
		cornerUpRight.y,
		cornerUpRight.value,
		cornerDownRight.x,
		cornerDownRight.y,
		cornerDownRight.value
	);*/
	sleepMotors();
	// on black now, start steppering right to find first corner
	while (1);

	return kStatus_Success;
}

void findRectangleCorner(sample_s* corner_p, bool slim, bool upRight, bool downRight, bool downLeft, bool upLeft) {
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
}

// 3000 x 250 debugging oval (for moving around a rectangle and finding IR center
void drawRectangle() {
	SERVO_setPenMode(PENDOWN);
	delay20ms();
	uint32_t i = 0;
	while (i < 250) {
		STEPPERS_moveRelativeAccel(stepperX_p, i % 10 == 0 ? -3000 : 3000);
		while(stepperX_p->status.running);
		STEPPERS_moveRelativeNoAccel(stepperY_p, 5);
		while (stepperY_p->status.running);
		delay1ms();
		i+= 5;
	}
	SERVO_setPenMode(PENUP);
	delay20ms();
}
