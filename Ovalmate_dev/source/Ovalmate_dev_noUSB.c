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
 */

// MAIN INCLUDE
#include "modules/mainInclude/mainInclude.h"
#include "modules/utils/utils.h"
#include <stdio.h>
#include "fsl_debug_console.h"  // this gives access to PRINTF for debugging

#define OVALSTEPSEPARATION		14U
#define LINESPEROVALSIDE		6U

void delay(void)
{
    volatile uint32_t i = 0U;
    for (i = 0U; i < 20000000U; ++i) // 20000000U for servo
    {
        __asm("NOP"); /* delay */
    }
}

void findHome(void) {
	while (1) {
		if (!buttonCallback_stepperMotorX_stopped)
			STEPPERS_moveRelativeNoAccel(stepperX_p, -10000);	// left interrupt expected
		if (!buttonCallback_stepperMotorY_stopped)
			STEPPERS_moveRelativeNoAccel(stepperY_p, 10000);	// up interrupt expected
		// distance isn't important, if we don't hit, it will run again
		while (stepperX_p->status.running && stepperY_p->status.running);
		if (buttonCallback_stepperMotorX_stopped && buttonCallback_stepperMotorY_stopped) {
			STEPPERS_setHome(stepperX_p);
			STEPPERS_setHome(stepperY_p);
			break;
		}
		else
			while (stepperX_p->status.running || stepperY_p->status.running);
	}
}

//uint32_t ovalCurveEquation(uint32_t x) {
//	double lx = ((double) x) / 1000000.0lf;
//	return -1500000 * (lx * lx) + 1500000; // -1.5 x^2 + 1.5 == equation of 1 half of oval as parabola. Equation in code converted to take nanometers rather than millimeters
//}

// ballistic behavior. Presumes at center of oval
void drawOval() {
	uint32_t ovalLineDistance = 15000U;			// distance between lines		(nm)
	uint32_t ovalLineStartDistance = 5000U;		// distance from top of oval	(nm)
	int8_t k = 6;								// start value

	int32_t currX = 0; // presume in center of oval
	int32_t currY = 0; // presume in center of oval

	while (k >= 0) {
		int32_t x = -(k * 150000 + 50000); // 0.15 mm * k + 0.05 mm = 6 evenly spaced divisions across 0-1 given input in space 0-6
		// negative because this will do negative side first
		int32_t relativeX = x - currX;
		uint32_t xPosSteps = nanometersToSteps((uint64_t) (relativeX < 0 ? -relativeX : relativeX)); // negative side first, so
		STEPPERS_moveRelativeNoAccel(stepperY_p, relativeX > 0 ? xPosSteps : -xPosSteps); // the parabola model is a 90 degree rotation of half the oval. So X coords actually correspond to Y's motor
		currX = x;

		double scaledX = (double) x / 1000000.0L; // scaled to 1mm (1000000 nm)
		uint32_t y = -1500000 * (scaledX * scaledX) + 1500000;
		int32_t relativeY = y - currY;
		uint32_t yPosSteps = nanometersToSteps((uint64_t)relativeY);
		STEPPERS_moveRelativeNoAccel(stepperX_p, relativeY > 0 ? -yPosSteps : yPosSteps); // x stepper drives in inverse directions compared to y, so opposite signs
		currY = y;

		while (stepperX_p->status.running && stepperY_p->status.running);

		// in position
		updateServoPWMDutyCycle(PENDOWN);
		delay();
		uint32_t lineSteps = nanometersToSteps(y << 1);
		STEPPERS_moveRelativeNoAccel(stepperX_p, lineSteps); // y * 2 == line width
		while(stepperX_p->status.running);
		currY = -y;
		updateServoPWMDutyCycle(PENUP);
		delay();
		k--;
		PRINTF("\r\n line drawn 1,"
			"\r\n\t %s%d,"
			"\r\n\t %s%d,"
			"\r\n\t %s%d"
			"\r\n\t %s%d"
			"\r\n\t %s%d"
			"\r\n\t %s%d"
			"\r\n\t %s%d"
			"\r\n\t %s%d"
			"\r\n\t %s%d",
			"x = ",
			x,
			"y = ",
			y,
			"relativeX = ",
			relativeX,
			"relativeY = ",
			relativeY,
			"xPosSteps = ",
			relativeX > 0 ? xPosSteps : -xPosSteps,
			"yPosSteps = ",
			relativeY > 0 ? yPosSteps : -yPosSteps,
			"lineSteps = ",
			lineSteps,
			"currX = ",
			currX,
			"currY = ",
			currY
		);
	}
	while (k <= 6) {
		int32_t x = k * 150000 + 50000; // 0.15 mm * k + 0.05 mm = 6 evenly spaced divisions across 0-1 given input in space 0-6
		int32_t relativeX = x - currX;
		uint32_t xPosSteps = nanometersToSteps((uint64_t) (relativeX < 0 ? -relativeX : relativeX)); // negative side first, so
		STEPPERS_moveRelativeNoAccel(stepperY_p, relativeX > 0 ? xPosSteps : -xPosSteps); // the parabola model is a 90 degree rotation of half the oval. So X coords actually correspond to Y's motor
		currX = x;

		double scaledX = (double) x / 1000000.0L; // scaled to 1mm (1000000 nm)
		uint32_t y = -1500000 * (scaledX * scaledX) + 1500000;
		int32_t relativeY = y - currY;
		uint32_t yPosSteps = nanometersToSteps((uint64_t)relativeY);
		STEPPERS_moveRelativeNoAccel(stepperX_p, relativeY > 0 ? -yPosSteps : yPosSteps); // x stepper drives in inverse directions compared to y, so opposite signs
		currY = y;

		while (stepperX_p->status.running && stepperY_p->status.running);

		// in position
		updateServoPWMDutyCycle(PENDOWN);
		delay();
		uint32_t lineSteps = nanometersToSteps(y << 1);
		STEPPERS_moveRelativeNoAccel(stepperX_p, lineSteps); // y * 2 == line width
		while(stepperX_p->status.running);
		currY = -y;
		updateServoPWMDutyCycle(PENUP);
		delay();
		k++;
		PRINTF("\r\n line drawn 2,"
			"\r\n\t %s%d,"
			"\r\n\t %s%d,"
			"\r\n\t %s%d"
			"\r\n\t %s%d"
			"\r\n\t %s%d"
			"\r\n\t %s%d"
			"\r\n\t %s%d"
			"\r\n\t %s%d"
			"\r\n\t %s%d",
			"x = ",
			x,
			"y = ",
			y,
			"relativeX = ",
			relativeX,
			"relativeY = ",
			relativeY,
			"xPosSteps = ",
			relativeX > 0 ? xPosSteps : -xPosSteps,
			"yPosSteps = ",
			relativeY > 0 ? yPosSteps : -yPosSteps,
			"lineSteps = ",
			lineSteps,
			"currX = ",
			currX,
			"currY = ",
			currY
		);
	}

	//updateServoPWMDutyCycle(PENDOWN);
	//delay();
	//STEPPERS_moveRelativeNoAccel(stepperX_p, -(startPosSteps << 1));

	/*
	// if at center of oval, need to move up 1mm - 0.03mm.
	int32_t startPositionSteps = -millimetersToSteps(0.97L); // negative because we want to go upwards to top of oval
	STEPPERS_moveRelativeNoAccel(stepperY_p, startPositionSteps);
	while (stepperY_p->status.running);
	// at (1.5, 0.03)mm
	uint8_t k = 0;
	double xCoord = k * 0.14L + 0.03L;
	uint32_t xStepsHalf = millimetersToSteps(xCoord);
	STEPPERS_moveRelativeNoAccel(stepperX_p, xStepsHalf);
	while(stepperX_p->status.running);
	updateServoPWMDutyCycle(PENDOWN);
	delay();
	STEPPERS_moveRelativeNoAccel(stepperX_p, -(xStepsHalf << 1));
	while(stepperX_p->status.running);
	updateServoPWMDutyCycle(PENUP);
	PRINTF("\r\n oval drawn, \r\n\t startPosSteps = %d, \r\n\t xCoord = %lf, \r\n\t xStepsHalf = %d", startPositionSteps, xCoord, xStepsHalf);
	*/
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
	setupServoPWM();
	startServoPWM();

	STEPPERS_moveRelativeAccel(stepperX_p, 10000);
	while (stepperX_p->status.running);
	delay();
	drawOval();



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
