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
// End Variables

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

	emergencyBumpCallback = buttonCallback_stopRelaventMotor;
	rightBumpCallback = buttonCallback_stopRelaventMotor;
	leftBumpCallback = buttonCallback_stopRelaventMotor;
	upBumpCallback = buttonCallback_stopRelaventMotor;
	downBumpCallback = buttonCallback_stopRelaventMotor;

	//initializeSteppers();

	//addMotorCallback(MOTORBOTH, stopMotorIfJobComplete);

	return kStatus_Success;
}

//void motorCallback_scheduleNextJob(stepperMotor_t* motorP) {
//	PRINTF("\r\nExecuting motorCallback_scheduleNextJob with params motor = %c, nextJobSteps = %d",
		//motorToWhichMotor(*motorP),
		//motorCallback_scheduleNextJob_steps
//	);
//	motorCallback_printCurrSteps(motorP);
	//if (motorStopped && motorCallback_scheduleNextJob_steps != 0) {
	//	setupStepperPWM(&motor, motorCallback_scheduleNextJob_steps);
	//	startStepperPWM(motor.timer);
	//	motorCallback_scheduleNextJob_steps--;
	//	if (motorCallback_scheduleNextJob_steps < 10)
	//		stopStepperPWM(motor.timer);
	//}
//}
//
//void motorCallback_printCurrSteps(stepperMotor_t* motor) {
//	PRINTF("\r\ncurr steps: %d, goalSteps: %d", motor->currentJob.currSteps, motor->currentJob.goalSteps);
//}
//
//void stopMotorIfJobComplete(stepperMotor_t* motor) {
//	motor->netSteps += motor->direction == 0 ? 1 : -1;
//	motor->currentJob.currSteps++;
//	if (motor->currentJob.currSteps == motor->currentJob.goalSteps)
//		stopStepperPWM(getWhich(motor));
//}
//
//
