/**
 * Title: OvalMate_buttons_module_DevBoard
 * Version: 0.0.1
 * Filename: LPC55S16_Project_main_dev.c
 * Authors: Alex Wissing
 * Purpose of Program: Initialize button interrupts
 * How Program is Run on Target System:
 * Date Started: 3/2/2022
 * Update History:
 *	- 3/3/2022:
 *		Setup basic configuration and created several helper functions that combine behaviors from different modules
 *	- 3/6/2022:
 *		Removed some helper functions as systems are changing soon. These extra functions require functions that may not be defined
 *		once changes go through. Commenting out for now
 *	- 3/8/2022:
 *		Moved initialization of all components into the "configure" function of mainInclude.
 *	- 3/14/2022:
 *		Moved PINT_Init outside of buttons.c assignPinsToInterrupts and STEPPERS_initializePins as they both try to set it, but they overwrite if PINT_Init is called twice
 *		Added new button callback to stop relevant motor rather than all motors. This can be used when finding home without killing the motor that has yet to find the wall
 *		Added two variables to check if the interrupt was the cause of the motor stopping. Should find a better way to do this, as it isn't very intuitive or clean.
 */

#include "mainInclude.h"

// Variables
bool buttonCallback_quitProgram = false;
// End Variables

/**
 * This function initializes all components needed to operate the Oval-Mate (SEVENSEG, BUTTONS, IRSENSOR, STEPPERS, and SERVO)
 * and also sets initial buttonCallback behavior.
 *
 * @params	- None
 * @return	- status_t		: always returns kStatus_Success.
 */
status_t configure() {

	/* Initialize PINT */
	PINT_Init(PINT); // need pin interrupts but can only init this once without overwritting
	// buttons.c and steppers.c need this

	SEVENSEG_initializeLegs();
	BUTTONS_assignPinsToInterrupts();
	IRSENSOR_initializeADC();
	STEPPERS_initializePins();
	STEPPERS_initializeMotors();
	SERVO_initializePWM();

	emergencyBumpCallback = buttonCallback_stopAndQuit;
	rightBumpCallback = buttonCallback_stopRelaventMotor;
	leftBumpCallback = buttonCallback_stopRelaventMotor;
	upBumpCallback = buttonCallback_stopRelaventMotor;
	downBumpCallback = buttonCallback_stopRelaventMotor;

	return kStatus_Success;
}

/**
 * buttonCallback function that stops the plotter until it is powered off. Currently reserves this functionality for the emergency button.
 *
 * @params	- pint_pin_int_t pintr			: an identifier for which pin interrupt is triggering this function call. (always kPINT_PinInt1)
 * 			- uint32_t pmatch_status		: I actually don't know what this is. Should probably print it out and try to figure it out.
 * @return	- None
 */
void buttonCallback_stopAndQuit(pint_pin_int_t pintr, uint32_t pmatch_status) {
	PRINTF("\r\nExecuting buttonCallback_stopRelaventMotors with params pintr = %d, pmatch_status = %d",
		(uint32_t) pintr,
		pmatch_status
	);
	if (pintr == kPINT_PinInt0) {
		STEPPERS_stopMotor(stepperX_p);
		STEPPERS_stopMotor(stepperY_p);
		SERVO_stopPWM();
		buttonCallback_quitProgram = true;
		stepperMotorsAllowedToDrive = false;
		servoMotorAllowedToDrive = false;
	}
}
