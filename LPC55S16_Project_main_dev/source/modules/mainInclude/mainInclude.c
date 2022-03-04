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
 */

#include "mainInclude.h"

uint32_t motorCallback_scheduleNextJob_steps = 50;

status_t configure() {
	emergencyBumpCallback = buttonCallback_stopMotors;
	rightBumpCallback = buttonCallback_stopMotors;
	leftBumpCallback = buttonCallback_stopMotors;
	upBumpCallback = buttonCallback_stopMotors;
	downBumpCallback = buttonCallback_stopMotors;

	addMotorCallback(sMotorBoth, stopMotorIfJobComplete);

	return kStatus_Success;
}

void buttonCallback_stopMotors(pint_pin_int_t pintr, uint32_t pmatch_status) {
	PRINTF("\r\nExecuting buttonCallback_stopMotors with params pintr = %d, pmatch_status = %d",
		(uint32_t) pintr,
		pmatch_status
	);
	stopStepperPWM(motorX.timer);
	stopStepperPWM(motorY.timer);
}

void motorCallback_scheduleNextJob(stepperMotor* motorP) {
	PRINTF("\r\nExecuting motorCallback_scheduleNextJob with params motor = %c, nextJobSteps = %d",
		motorToWhichMotor(*motorP),
		motorCallback_scheduleNextJob_steps
	);
	motorCallback_printCurrSteps(motorP);
	//if (motorStopped && motorCallback_scheduleNextJob_steps != 0) {
	//	setupStepperPWM(&motor, motorCallback_scheduleNextJob_steps);
	//	startStepperPWM(motor.timer);
	//	motorCallback_scheduleNextJob_steps--;
	//	if (motorCallback_scheduleNextJob_steps < 10)
	//		stopStepperPWM(motor.timer);
	//}
}

void motorCallback_printCurrSteps(stepperMotor* motor) {
	PRINTF("\r\ncurr steps: %d, goalSteps: %d", motor->currentJob.currSteps, motor->currentJob.goalSteps);
}

void stopMotorIfJobComplete(stepperMotor *motor) {
	motor->netSteps += motor->direction == 0 ? 1 : -1;
	motor->currentJob.currSteps++;
	if (motor->currentJob.currSteps == motor->currentJob.goalSteps)
		stopStepperPWM(motor->timer);
}

void driveStepperPWM(which_motor_t which, uint32_t steps, uint32_t freq, bool blocking) {
	if (which == sMotorBoth) {
		setupStepperPWM(&motorX, steps, freq);
		setupStepperPWM(&motorY, steps, freq);
		startStepperPWM(motorX.timer);
		startStepperPWM(motorY.timer);
		if (blocking)
			blockUntilMotorJobComplete(which);
	}
	else {
		stepperMotor* m = whichMotorToMotor(which);
		setupStepperPWM(m, steps, freq);
		startStepperPWM(m->timer);
		if (blocking)
			blockUntilMotorJobComplete(which);
	}

}


