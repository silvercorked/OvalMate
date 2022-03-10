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
 *	-3/8/2022:
 *		Moved initialization of all components into the "configure" function of mainInclude.
 */

#include "mainInclude.h"


status_t configure() {

	initialize7SegLegs();
	assignPinsToInterrupts();
	initializeADC();
	initializeStepperPins();
	initializeStepperMotors();
	initializeServoPWM();

	emergencyBumpCallback = buttonCallback_stopMotors;
	rightBumpCallback = buttonCallback_stopMotors;
	leftBumpCallback = buttonCallback_stopMotors;
	upBumpCallback = buttonCallback_stopMotors;
	downBumpCallback = buttonCallback_stopMotors;

	//initializeSteppers();

	//addMotorCallback(MOTORBOTH, stopMotorIfJobComplete);

	return kStatus_Success;
}

void buttonCallback_stopMotors(pint_pin_int_t pintr, uint32_t pmatch_status) {
	PRINTF("\r\nExecuting buttonCallback_stopMotors with params pintr = %d, pmatch_status = %d",
		(uint32_t) pintr,
		pmatch_status
	);
	stopMotor(stepperX_p);
	stopMotor(stepperY_p);
	//stopStepperPWM(MOTORX);
	//stopStepperPWM(MOTORY);
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
