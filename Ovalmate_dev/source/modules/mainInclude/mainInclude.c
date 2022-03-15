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

// Variables
bool buttonCallback_stepperMotorX_stopped = false;
bool buttonCallback_stepperMotorY_stopped = false;
// End Variables


status_t configure() {

	/* Initialize PINT */
	PINT_Init(PINT); // need pin interrupts but can only init this once without overwritting
	// buttons.c and steppers.c need this

	initialize7SegLegs();
	assignPinsToInterrupts();
	initializeADC();
	STEPPERS_initializePins();
	STEPPERS_initializeMotors();
	initializeServoPWM();

	emergencyBumpCallback = buttonCallback_stopRelaventMotor;
	rightBumpCallback = buttonCallback_stopRelaventMotor;
	leftBumpCallback = buttonCallback_stopRelaventMotor;
	upBumpCallback = buttonCallback_stopRelaventMotor;
	downBumpCallback = buttonCallback_stopRelaventMotor;

	//initializeSteppers();

	//addMotorCallback(MOTORBOTH, stopMotorIfJobComplete);

	return kStatus_Success;
}

void buttonCallback_stopMotors(pint_pin_int_t pintr, uint32_t pmatch_status) {
	PRINTF("\r\nExecuting buttonCallback_stopMotors with params pintr = %d, pmatch_status = %d",
		(uint32_t) pintr,
		pmatch_status
	);
	STEPPERS_stopMotor(stepperX_p);
	STEPPERS_stopMotor(stepperY_p);
	buttonCallback_stepperMotorX_stopped = true;
	buttonCallback_stepperMotorY_stopped = true;
	//stopStepperPWM(MOTORX);
	//stopStepperPWM(MOTORY);
}

void buttonCallback_stopRelaventMotor(pint_pin_int_t pintr, uint32_t pmatch_status) {
	PRINTF("\r\nExecuting buttonCallback_stopRelaventMotors with params pintr = %d, pmatch_status = %d",
		(uint32_t) pintr,
		pmatch_status
	);
	// right bump switch (kPINT_PinInt1)
	// left bump switch (kPINT_PinInt2)
	// down bump switch (kPINT_PinInt3)
	// up bump switch (kPINT_PinInt4)
	// emergency bump switch (kPINT_PinInt0)
	if (pintr == kPINT_PinInt1 || pintr == kPINT_PinInt2) {
		STEPPERS_stopMotor(stepperX_p);
		buttonCallback_stepperMotorX_stopped = true;
	}
	else if (pintr == kPINT_PinInt3 || pintr == kPINT_PinInt4) {
		STEPPERS_stopMotor(stepperY_p);
		buttonCallback_stepperMotorY_stopped = true;
	}
	else {
		// emergency pintr or no button interrupt (ie this allows the pints on fault condition to use this still
		STEPPERS_stopMotor(stepperX_p);
		STEPPERS_stopMotor(stepperY_p);
		buttonCallback_stepperMotorX_stopped = true;
		buttonCallback_stepperMotorY_stopped = true;
	}

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
