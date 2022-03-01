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
 */

// CORE INCLUDES
#include <stdio.h>
#include "board.h"
// #include "clock_config.h" // not used atm, but commonly used
#include "fsl_debug_console.h"
// ;#include "LPC55S16.h" // somehow not used rn
#include "peripherals.h"
#include "pin_mux.h"

// MODULE INCLUDES
#include "modules/sevenSegmentLED/sevenSegmentLED.h" // 7 segment display
#include "modules/irSensor/irSensor.h" // irSensor via ADC
#include "modules/servo/servo.h" // servo via CTIMER
#include "modules/buttons/buttons.h" // button interrupts via pin_mux
#include "modules/steppers/steppers.h" // stepper via SCTIMER 2 in 16-bit mode

void delay(void)
{
    volatile uint32_t i = 0U;
    for (i = 0U; i < 20000000U; ++i)
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

	initialize7SegLegs();
	assignPinsToInterrupts();
	initializeADC();
	initializeStepperPWM();
	setupStepperPWM(motorX, 360);
	setupStepperPWM(motorY, 360);
	startStepperPWM(motorX.timer);
	startStepperPWM(motorY.timer);
	initializeServoPWM();
	setupServoPWM();
	startServoPWM();

	set7Seg(0, 0, 1, 0, 1, 0, 1);

	/* Force the counter to be placed into memory. */
	volatile static int i = 0 ;
	/* Enter an infinite loop, just incrementing a counter. */
	while(1) {
		i++ ;
		/* 'Dummy' NOP to allow source level single stepping of
			tight while() loop */
		delay();
		if (i % 2 == 1) {
			updateServoPWMDutyCycle(PENDOWN);
			PRINTF("\r\nPENDOWN");
			set7Seg(0, 0, 1, 0, 1, 0, 1);
		}
		else {
			updateServoPWMDutyCycle(PENUP);
			PRINTF("\r\nPENUP");
			set7Seg(1, 1, 0, 1, 0, 1, 0);
		}
		__asm volatile ("nop");
	}
	return 0;
}
