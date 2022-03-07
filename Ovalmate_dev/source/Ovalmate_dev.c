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
 */

// MAIN INCLUDE
#include "modules/mainInclude/mainInclude.h"
#include <stdio.h>
#include "fsl_debug_console.h"  // this gives access to PRINTF for debugging

void delay(void)
{
    volatile uint32_t i = 0U;
    for (i = 0U; i < 9000000U; ++i) // 20000000U for servo
    {
        __asm("NOP"); /* delay */
    }
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
	initialize7SegLegs();
	assignPinsToInterrupts();
	initializeADC();
	initializeStepperPWM();
	initializeServoPWM();
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

	// set 7 seg

	volatile uint32_t i = 0;
	while (1) {
		delay();
		i++;
		if (i % 2 == 1) {
			set7Seg(0, 0, 1, 0, 1, 0, 1);
			driveStepperPWMNonBlocking(MOTORX, 5000, 100000);
			updateServoPWMDutyCycle(PENUP);
		}
		else {
			set7Seg(1, 1, 0, 1, 0, 1, 0);
			driveStepperPWMBlocking(MOTORX, 5000, 100000);
			updateServoPWMDutyCycle(PENDOWN);
		}
		delay();
		int a = getADCValue();
		PRINTF("adc val: %d", a);
	}
	delay();
	driveStepperPWMNonBlocking(MOTORBOTH, 5, 5);
	PRINTF("printing while driving motors");
	while(1);

	delay();
	//driveStepperPWMBlock(sMotorX, 50, 2000);
	//delay();
	//driveStepperPWMBlock(sMotorX, 20, 1500);

	delay();

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
