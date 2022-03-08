/**
 * Title: OvalMate_stepper_module_DevBoard
 * Version: 0.0.1
 * Filename: LPC55S16_Project_main_dev.c
 * Authors: Alex Wissing
 * Purpose of Program: Manage X and Y stepper motors
 * How Program is Run on Target System:
 * Date Started: 2/28/2022
 * Update History:
 *	- 2/28/2022
 *		Moved into separate "module" file.
 *		Created method that might allow for better timer control. took the SCTIMER_SetupPwm() function from nxp
 *			and created my own version that used the 16-bit timers. Will test tomorrow if it works, but it built and debugged.
 *	- 3/2/2022:
 *		Added functions for handling stepperMotor callbacks. Corrected type declarations for stepper motor callbacks.
 *		Changed stepper motor callbacks into an array of length 4 which allows up to 4 functions to be set and run each period interrupt event.
 *		Added blocking function to waste CPU time until motor jobs complete.
 *		Changed Start and Stop motor functions to assign running boolean.
 *		Added conversion functions for swapping between the whichMotor_t type and the motor pointers.
 *		Fixed a bug in SetupStepperPMW where currentJob was not assigned correctly.
 *		Added conditional timer configurations in setupPwm16BitMode in hopes of resolving hardfaults.
 */

// temp
#include <stdio.h>
#include "fsl_debug_console.h"  // this gives access to PRINTF for debugging

// Includes
#include "steppers.h"
// End Includes

// Variables
void (*stepperMotorXCallback[4])(whichMotor); // callback arr[4]
void (*stepperMotorYCallback[4])(whichMotor); // callback arr[4]
// End Variables

// Functions
void initializeSteppers() {
	motorX_p->periodCallback = STEPPERX_completeStepStepperMotorXCallback;
	motorY_p->periodCallback = STEPPERY_completeStepStepperMotorYCallback;
}

status_t prepareStepperPWM(whichMotor, uint32_t, uint32_t);		// run init and setup
void driveStepperPWM(whichMotor, uint32_t, uint32_t, bool);		// run prep and start
void startStepperPWM(whichMotor);								// start timer, set running
void stopStepperPWM(whichMotor);								// halt timer, clear events, clear running

void STEPPERX_completeStepStepperMotorXCallback() {
	for (uint8_t i = 0; i < 4; i++) {
		if (*(stepperMotorXCallback[i]) != NULL)
			(*(stepperMotorXCallback[i]))(MOTORX); // call function from callback array
		else
			break;
	}
}
void STEPPERY_completeStepStepperMotorYCallback(uint32_t flags) {
	for (uint8_t i = 0; i < 4; i++) {
		if (*(stepperMotorYCallback[i]) != NULL)
			(*(stepperMotorYCallback[i]))(MOTORY); // call function from callback array
		else
			break;
	}
}

void stopMotorOnJobComplete(whichMotor which) {
	motorY_p->netSteps++;
		if (motorY_p->currentJob.currSteps == motorY_p->currentJob.goalSteps) {
			PRINTF("\r\n Job Done ctimer");
			STEPPERY_stopStepperPWM();
		}
		motorY_p->currentJob.currSteps++;
}

void blockUntilMotorJobComplete(whichMotor which) {
	if (which == MOTORBOTH)
		while (motorX_p->running || motorY_p->running);
	else if (which == MOTORX)
		while (motorX_p->running);
	else
		while (motorY_p->running);
}

uint8_t getMotorCallbackLength(whichMotor which) {
	assert(which != MOTORBOTH);
	uint8_t i = 0;
	if (which == MOTORX)
		for (; i < 4 && stepperMotorXCallback[i] != NULL; i++);
	else
		for (; i < 4 && stepperMotorYCallback[i] != NULL; i++);
	return i;
}

status_t addMotorCallback(whichMotor which, void (*f)(whichMotor)) {
	if (which == MOTORBOTH) {
		uint8_t xLen = getMotorCallbackLength(MOTORX);
		uint8_t yLen = getMotorCallbackLength(MOTORY);
		if (xLen >= 4 || yLen >= 4)
			return kStatus_Fail;
		else {
			stepperMotorXCallback[xLen] = f;
			stepperMotorYCallback[yLen] = f;
		}
	}
	else if (which == MOTORX) {
		uint8_t xLen = getMotorCallbackLength(MOTORX);
		if (xLen >= 4)
			return kStatus_Fail;
		else
			stepperMotorXCallback[xLen] = f;
	}
	else {
		uint8_t yLen = getMotorCallbackLength(MOTORY);
		if (yLen >= 4)
			return kStatus_Fail;
		else
			stepperMotorYCallback[yLen] = f;
	}
	return kStatus_Success;
}

status_t removeMotorCallback(whichMotor which, uint8_t index) {
	if (index > 3)
		return kStatus_Fail;
	if (which == MOTORX || which == MOTORBOTH)
		stepperMotorXCallback[index] = NULL;
	if (which == MOTORY || which == MOTORBOTH)
		stepperMotorYCallback[index] = NULL;
	return kStatus_Success;
}

status_t clearMotorCallbacks(whichMotor which) {
	for (uint8_t i = 0; i < 4; i++)
		removeMotorCallback(which, i);
	return kStatus_Success;
}

void* getMotor(whichMotor which) {
	if (which == MOTORY)
		return motorY_p;
	return motorX_p;
}
