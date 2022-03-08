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
sctimer_config_t sctimerInfo;
stepperMotor_sctimer_t motorX = {
	.direction = 0,							// direction for motor controller
	.netSteps = 0,							// keep track of net steps, so (steps in 0 dir) - (steps in 1 dir)
	.config = {
		.output = SCTIMER_MOTOR_X_OUT,		//
		.activeDir = 0,						// need to set to avoid null pointer
		.matchValue = 0,
		.eventCounter = 0,					// set event number
		.timer = kSCTIMER_Counter_U
	},
	.currentJob = { },
	.running = false
};
stepperMotor_sctimer_t* motorX_p = (stepperMotor_sctimer_t*) (&motorX);
// End Variables
// MANAGEMENT
status_t STEPPERX_Calculate32BitCounterConfig(
	uint64_t rawCountValue,
	uint8_t* prescale,
	uint16_t* matchValue,
	sctimer_event_active_direction_t* activeDir
) {
    if (rawCountValue < MAX_UP_COUNTER_VALUE) {
        *prescale   = (uint8_t)(rawCountValue / 0xFFFFFFFFU);
        *matchValue = (uint16_t)(rawCountValue / (*prescale + 1U));
        *activeDir  = kSCTIMER_ActiveIndependent;
    }
    else if (rawCountValue < MAX_UPDOWN_COUNTER_VALUE) {
        *prescale   = (uint8_t)(rawCountValue / 0x1FFFFFFFFU);
        *matchValue = (uint16_t)(0x1FFFFFFFFU - rawCountValue / (*prescale + 1));
        *activeDir  = kSCTIMER_ActiveInCountDown;
    }
    else
        return kStatus_Fail;
    return kStatus_Success;
}

status_t STEPPERX_initializeStepperPWM(uint32_t steps, uint32_t freq) {
	STEPPERX_stopStepperPWM(); // stop em if they are running to clear way for new job

	/* Print a note to terminal */
	SCTIMER_GetDefaultConfig(&sctimerInfo);
	///* Add judgement for change clock source*/
	//#if defined(SCTIMER_NEED_CHANGE_CLOCK_SOURCE)
	//sctimerInfo.clockMode   = DEMO_CLOCK_MODE;
	//sctimerInfo.clockSelect = DEMO_CLOCK_SEL;
	//#endif

	motorX_p->currentJob.currSteps = 0;
	motorX_p->currentJob.goalSteps = steps;

	if (
		SCTIMER_Calculate32BitCounterConfig(
			(SCTIMER_CLK_FREQ / freq) - 1U, //MSEC_TO_COUNT(200U, SCTIMER_CLK_FREQ),
			&sctimerInfo.prescale_l,
			&(motorX_p->config.matchValue),
			&(motorX_p->config.activeDir)
		) == kStatus_Fail
	) {
		PRINTF("\r\nSCTimer 32-bit unity counter is out of range\r\n");
		return kStatus_Fail;
	}
	if (motorX_p->config.activeDir != kSCTIMER_ActiveIndependent) {
		sctimerInfo.enableBidirection_l = true;
	}

	SCTIMER_Init(SCT0, &sctimerInfo);

	return kStatus_Success;
}

status_t STEPPERX_setupStepperPWM() {
	/* Schedule a match event for the 16-bit low counter every 0.1 seconds */
	if (
		SCTIMER_CreateAndScheduleEvent(
			SCT0,						// base
			kSCTIMER_MatchEventOnly,	//
			motorX_p->config.matchValue,				// Match value to trigger
			0,							// ignored on kSCTIMER_MatchEventOnly
			motorX_p->config.timer,						// kSCTIMER_Counter_L or kSCTIMER_Counter_H
			&(motorX_p->config.eventCounter)			// eventCounterL or eventCounterH
		) == kStatus_Fail
	) {
		return kStatus_Fail;
	}

	/* Toggle first output when the 16-bit low counter event occurs */
	SCTIMER_SetupOutputToggleAction(SCT0, motorX_p->config.output, motorX_p->config.eventCounter);
	/* Reset Counter L when the 16-bit low counter event occurs */
	SCTIMER_SetupCounterLimitAction(SCT0, motorX_p->config.timer, motorX_p->config.eventCounter);
	/* Setup the 16-bit low counter event active direction */
	SCTIMER_SetupEventActiveDirection(SCT0, motorX_p->config.activeDir, motorX_p->config.eventCounter);

	SCTIMER_EnableInterrupts(SCT0, (1 << motorX_p->config.eventCounter));				// set interrupt enable for event
	SCTIMER_SetCallback(SCT0, motorX_p->periodCallback, motorX_p->config.eventCounter); // set callback on period complete

	return kStatus_Success;


//	/* Schedule a match event for the 16-bit low counter every 0.1 seconds */
//	if (
//		SCTIMER_CreateAndScheduleEvent(
//			SCT0,
//			kSCTIMER_MatchEventOnly,
//			matchValueL,
//			0,
//			kSCTIMER_Counter_L,
//			&eventCounterL
//		) == kStatus_Fail
//	) {
//		return -1;
//	}
//
//	/* Toggle first output when the 16-bit low counter event occurs */
//	SCTIMER_SetupOutputToggleAction(SCT0, SCTIMER_MOTOR_X_OUT, eventCounterL);
//	/* Reset Counter L when the 16-bit low counter event occurs */
//	SCTIMER_SetupCounterLimitAction(SCT0, kSCTIMER_Counter_L, eventCounterL);
//	/* Setup the 16-bit low counter event active direction */
//	SCTIMER_SetupEventActiveDirection(SCT0, activeDirL, eventCounterL);
//	/* Schedule a match event for the 16-bit high counter every 0.2 seconds */
//	if (
//		SCTIMER_CreateAndScheduleEvent(
//			SCT0,
//			kSCTIMER_MatchEventOnly,
//			matchValueH,
//			0,
//			kSCTIMER_Counter_H,
//			&eventCounterH
//		) == kStatus_Fail
//	) {
//		return -1;
//	}
//	/* Setup the 16-bit high counter event active direction */
//	SCTIMER_SetupEventActiveDirection(SCT0, activeDirH, eventCounterH);
//	/* Toggle second output when the 16-bit high counter event occurs */
//	SCTIMER_SetupOutputToggleAction(SCT0, SCTIMER_MOTOR_Y_OUT, eventCounterH);
//	/* Reset Counter H when the 16-bit high counter event occurs */
//	SCTIMER_SetupCounterLimitAction(SCT0, kSCTIMER_Counter_H, eventCounterH);
}

void STEPPERX_startStepperPWM() {
	/* Start the 16-bit low and high counter */ // sctimer_counter_t // kSCTIMER_Counter_L | kSCTIMER_Counter_H
	SCTIMER_StartTimer(SCT0, motorX_p->config.timer);
	motorX_p->running = true;
}

void STEPPERX_stopStepperPWM() {
	SCTIMER_StopTimer(SCT0, motorX_p->config.timer);
	uint8_t state = SCTIMER_GetCurrentState(SCT0);
	SCTIMER_ClearEventInState(SCT0, motorX_p->config.eventCounter, state);
	motorX_p->running = false;
}

void STEPPERX_driveStepperPWM(uint32_t steps, uint32_t freq, bool blocking) {
	STEPPERX_initializeStepperPWM(steps, freq);
	STEPPERX_startStepperPWM();
	if (blocking)
		STEPPERX_blockUntilMotorJobComplete();
}

// CALLBACKS
//void STEPPERX_stepCompleteMotorXCallback() {
//	for (uint8_t i = 0; i < 4; i++) {
//		if (*(stepperMotorXCallback[i]) != NULL) {
//			(*(stepperMotorXCallback[i]))(motorX_p); // call function from callback array
//		} else {
//			break;
//		}
//	}
//}

void STEPPERX_blockUntilMotorJobComplete() {
	while (motorX_p->running);
}

//uint8_t STEPPERX_getMotorCallbackLength() {
//	uint8_t i = 0;
//	for (; i < 4 && stepperMotorXCallback[i] != NULL; i++)
//		;
//	return i;
//}
//
//status_t STEPPERX_addMotorCallback(void (*f)(stepperMotor_sctimer_t*)) {
//	uint8_t xLen = getMotorCallbackLength(MOTORX);
//	if (xLen >= 4)
//		return kStatus_Fail;
//	else {
//		stepperMotorXCallback[xLen] = f;
//	}
//	return kStatus_Success;
//}
//
//status_t STEPPERX_removeMotorCallback(uint8_t index) {
//	if (index > 3)
//		return kStatus_Fail;
//	stepperMotorXCallback[index] = NULL;
//	return kStatus_Success;
//}
//
//status_t STEPPERX_clearMotorCallbacks() {
//	for (uint8_t i = 0; i < 4; i++)
//		STEPPERX_removeMotorCallback(i);
//	return kStatus_Success;
//}

// CONVERSION
stepperMotor_sctimer_t* getMotorX() {
	return motorX_p;
}
