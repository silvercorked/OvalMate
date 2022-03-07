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

#include "sctimerSteppers.h"

// temp
#include <stdio.h>
#include "fsl_debug_console.h"  // this gives access to PRINTF for debugging

// Variables
sctimer_config_t sctimerInfo;
uint32_t sctimerClock;
stepperMotor_sctimer motorX = {
		.direction = 0,					// bool
		.netSteps = 0,// keep track of net steps, so (steps in 0 dir) - (steps in 1 dir)
		.pwmEventPeriod = 0,	// uninitialized
		.pwmEventPulse = 1,	// uninitialized
		.pwmParam = { .output = X_SCTIMER_OUT, .level = kSCTIMER_HighTrue,
				.dutyCyclePercent = 50 }, .timer = kSCTIMER_Counter_L,
		.currentJob = { }, .running = false };
stepperMotor_sctimer motorY = { .direction = 0, .netSteps = 0,
		.pwmEventPeriod = 2,	// uninitialized
		.pwmEventPulse = 3,	// uninitialized
		.pwmParam = { .output = Y_SCTIMER_OUT, .level = kSCTIMER_HighTrue,
				.dutyCyclePercent = 50 }, .timer = kSCTIMER_Counter_H,
		.currentJob = { }, .running = false };
// 4 functions can be used
void (*stepperMotorXCallback[4])(stepperMotor_sctimer*); // passed a bool arg "motorStopped" and motor arg "motor(X|Y)
void (*stepperMotorYCallback[4])(stepperMotor_sctimer*); // passed a bool arg "motorStopped" and motor arg "motor(X|Y)
// End Variables

void stepCompleteMotorXCallback() {
	for (uint8_t i = 0; i < 4; i++) {
		if (*(stepperMotorXCallback[i]) != NULL) {
			(*(stepperMotorXCallback[i]))(&motorX); // call function from callback array
		} else {
			break;
		}
	}
}

void stepCompleteMotorYCallback() {
	for (uint8_t i = 0; i < 4; i++) {
		if (*(stepperMotorYCallback[i]) != NULL) {
			(*(stepperMotorYCallback[i]))(&motorY); // call function from callback array
		} else {
			break;
		}
	}
}

void blockUntilMotorJobComplete(which_motor_t whichMotor) {
	if (whichMotor == MOTORBOTH)
		while (motorX.running || motorY.running);
	else if (whichMotor == MOTORX) {
		while (motorX.running) {
			PRINTF("\r\n motorX.running: %d", motorX.running);

		}
		PRINTF("MotorX stopped");
	}
	else
		while (motorY.running);
}

uint8_t getMotorCallbackLength(which_motor_t whichMotor) {
	assert(whichMotor != MOTORBOTH);
	void (**m)(stepperMotor_sctimer*) = stepperMotorXCallback;
	if (whichMotor == MOTORY)
		m = stepperMotorYCallback;
	uint8_t i = 0;
	for (; m[i] != NULL && i < 4; i++)
		;
	return i;
}

status_t addMotorCallback(which_motor_t whichMotor, void (*f)(stepperMotor_sctimer*)) {
	if (whichMotor == MOTORBOTH) {
		uint8_t xLen = getMotorCallbackLength(MOTORX);
		uint8_t yLen = getMotorCallbackLength(MOTORY);
		if (xLen >= 4 || yLen >= 4)
			return kStatus_Fail;
		else {
			stepperMotorXCallback[xLen] = f;
			stepperMotorYCallback[yLen] = f;
		}
	} else if (whichMotor == MOTORX) {
		uint8_t xLen = getMotorCallbackLength(MOTORX);
		if (xLen >= 4)
			return kStatus_Fail;
		else {
			stepperMotorXCallback[xLen] = f;
		}
	} else {
		uint8_t yLen = getMotorCallbackLength(MOTORY);
		if (yLen >= 4)
			return kStatus_Fail;
		else {
			stepperMotorYCallback[yLen] = f;
		}
	}
	return kStatus_Success;
}

status_t removeMotorCallback(which_motor_t whichMotor, uint8_t index) {
	if (index > 3)
		return kStatus_Fail;
	if (whichMotor == MOTORX || whichMotor == MOTORBOTH)
		stepperMotorXCallback[index] = NULL;
	if (whichMotor == MOTORY || whichMotor == MOTORBOTH)
		stepperMotorYCallback[index] = NULL;
	return kStatus_Success;
}

status_t clearMotorCallbacks(which_motor_t whichMotor) {
	for (uint8_t i = 0; i < 4; i++)
		removeMotorCallback(whichMotor, i);
	return kStatus_Success;
}

void initializeStepperPWM() {
	sctimerClock = SCTIMER_CLK_FREQ;

	/* Print a note to terminal */
	//PRINTF("\r\nSCTimer example to output 2 center-aligned PWM signals @ %d Hz\r\n", sctimerClock);
	//PRINTF("\r\nProbe the signal using an oscilloscope\r\n");
	SCTIMER_GetDefaultConfig(&sctimerInfo);
	sctimerInfo.enableCounterUnify = 0; // set 16 bit mode for 2 timers

	motorX.periodCallback = stepCompleteMotorXCallback;
	motorY.periodCallback = stepCompleteMotorYCallback;

	/* Initialize SCTimer module */
	SCTIMER_Init(SCT0, &sctimerInfo);
	EnableIRQ(SCT0_IRQn); // going to need interrupts
}

status_t setupStepperPWM(stepperMotor_sctimer* motorP, uint32_t stepsToAssign, uint32_t frequency) {
	stopStepperPWM(motorP->timer);
	if (motorP->pwmEventPeriod < 0xFFFFFFF0) {
		SCTIMER_DisableInterrupts(SCT0, (1 << motorP->pwmEventPeriod)); // clear interrupts
		SCTIMER_DisableInterrupts(SCT0, (1 << motorP->pwmEventPulse)); // not yet set
	}
	stepperJob newJob = { .goalSteps = stepsToAssign, // create and overwrite job
			.currSteps = 0 };
	// need to set from pointer since we're modifying motorP's currentJob
	motorP->currentJob = newJob;	// set job

	if (setupPwm16BitMode(			// load and config timer
			SCT0,// sctimer module
			&(motorP->pwmParam),// pwmParam per motor (only diff is output pin)
			frequency,					// frequency
			sctimerClock,				// sctimer clock frequency
			motorP->timer, &(motorP->pwmEventPeriod),// event pointer (set internally)
			&(motorP->pwmEventPulse)		// event pointer (set internally)
			) == kStatus_Fail) {		// fails if there isn't space for events
		return kStatus_Fail;
	}
	SCTIMER_EnableInterrupts(SCT0, (1 << motorP->pwmEventPeriod)); // set interrupt enable for new event
	SCTIMER_SetCallback(SCT0, motorP->periodCallback, motorP->pwmEventPeriod); // set callback to standard callback

	return kStatus_Success;
}

void startStepperPWM(sctimer_counter_t whichCounter) {
	SCTIMER_StartTimer(SCT0, whichCounter);
	if (motorX.timer == whichCounter)
		motorX.running = true;
	else
		motorY.running = true;
}

void stopStepperPWM(sctimer_counter_t whichCounter) {
	SCTIMER_StopTimer(SCT0, whichCounter);
	if (motorX.timer == whichCounter)
		motorX.running = false;
	else
		motorY.running = false;
}

which_motor_t motorToWhichMotor(stepperMotor_sctimer motor) {
	if (motor.pwmParam.output == X_SCTIMER_OUT)
		return MOTORX;
	else if (motor.pwmParam.output == Y_SCTIMER_OUT)
		return MOTORY;
	return 0; // if an uninitialized motor gets pass in here
}

stepperMotor_sctimer* whichMotorToMotor(which_motor_t which) {
	if (which == MOTORX)
		return &motorX;
	else
		return &motorY; // both will return motorY
}

/*!
 * brief Configures the PWM signal parameters.
 *
 * param base        SCTimer peripheral base address
 * param pwmParams   PWM parameters to configure the output
 * param mode        PWM operation mode, options available in enumeration ::sctimer_pwm_mode_t
 * param pwmFreq_Hz  PWM signal frequency in Hz
 * param srcClock_Hz SCTimer counter clock in Hz
 * param event       Pointer to a variable where the PWM period event number is stored
 *
 * return kStatus_Success on success
 *         kStatus_Fail If we have hit the limit in terms of number of events created or if
 *                      an incorrect PWM dutycylce is passed in.
 */
status_t setupPwm16BitMode(SCT_Type *base,
		const sctimer_pwm_signal_param_t *pwmParams, uint32_t pwmFreq_Hz,
		uint32_t srcClock_Hz, sctimer_counter_t whichCounter,
		uint32_t *periodEvent, uint32_t *pulseEvent) {
	assert(NULL != pwmParams);
	assert(0U != srcClock_Hz);
	assert(0U != pwmFreq_Hz);
	assert(
			(uint32_t)pwmParams->output < (uint32_t)FSL_FEATURE_SCT_NUMBER_OF_OUTPUTS);
	assert(0U == (base->CONFIG & SCT_CONFIG_UNIFY_MASK));

	/* If we do not have enough events available (this function will create two events),
	 * the function will return fail.
	 */
	status_t status = kStatus_Fail;
	status_t status2;
	uint32_t period, pulsePeriod = 0;
	uint32_t sctClock =	0;
	if (whichCounter == kSCTIMER_Counter_L) {
		sctClock = srcClock_Hz / (((base->CTRL & SCT_CTRL_PRE_L_MASK) >> SCT_CTRL_PRE_L_SHIFT) + 1U);
	}
	else if (whichCounter == kSCTIMER_Counter_H) {
		sctClock = srcClock_Hz / (((base->CTRL & SCT_CTRL_PRE_H_MASK) >> SCT_CTRL_PRE_H_SHIFT) + 1U);
	}
	assert(sctClock != 0U);
	uint32_t periodEventTemp = 0, pulseEventTemp = 0;
	uint32_t reg;
	//sctimer_pwm_mode_t mode = kSCTIMER_CenterAlignedPwm;

	SCTIMER_ClearEventInState(base, *periodEvent,
			SCTIMER_GetCurrentState(SCT0));
	SCTIMER_ClearEventInState(base, *pulseEvent, SCTIMER_GetCurrentState(SCT0)); // clear old events

	// check we have enough space for events // don't need to check if we only ever use 2
	//if ((s_currentEvent + 2U) <= (uint32_t)FSL_FEATURE_SCT_NUMBER_OF_EVENTS) {
	/* Use bi-directional mode for center-aligned PWM */
	//if (mode == kSCTIMER_CenterAlignedPwm) {
	if (whichCounter == kSCTIMER_Counter_H) {
		base->CTRL |= SCT_CTRL_BIDIR_H_MASK;
	}
	else {
		base->CTRL |= SCT_CTRL_BIDIR_L_MASK;
	}
	//}

	/* Calculate PWM period match value */
	//if (mode == kSCTIMER_EdgeAlignedPwm) {
	//    period = (sctClock / pwmFreq_Hz) - 1U;
	//}
	//else {
	period = sctClock / (pwmFreq_Hz * 2U);
	// because we only use center align, we get twice the frequencies
	//}

	/* Calculate pulse width and period match value:
	 * For EdgeAlignedPwm, "pulsePeriod = 0" results in 0% dutycyle, "pulsePeriod = period - 1U" results in 100%
	 * dutycyle. For CenterAlignedPwm, , "pulsePeriod = 0" results in 0% dutycyle, "pulsePeriod = period + 2U"
	 * results in 100% dutycyle.
	 */

	pulsePeriod = (uint32_t) (((uint64_t) period * pwmParams->dutyCyclePercent)
			/ 100U);

	if (pwmParams->dutyCyclePercent >= 100U) {
		//if (mode == kSCTIMER_EdgeAlignedPwm) {
		//    pulsePeriod = period + 2U;
		//}
		//else {
		pulsePeriod = period - 1U;
		//}
	}

	/* Schedule an event when we reach the PWM period */
	status = SCTIMER_CreateAndScheduleEvent(base,						// STC0
			kSCTIMER_MatchEventOnly,	// after complete period
			period,						// number of timer ticks
			0,// input or output affected. MatchEventOnly causes this param to be ignored
			whichCounter,				// which counter desired
			&periodEventTemp			// event pointer (uint)
			);

	/* Schedule an event when we reach the pulse width */
	status2 = SCTIMER_CreateAndScheduleEvent(base,						// SCT0
			kSCTIMER_MatchEventOnly,	// after complete period
			pulsePeriod,				// number of timer ticks
			0,// input or output affected. MatchEventOnly causes this param to be ignored
			whichCounter,				// which counter desired
			&pulseEventTemp				// event pointer (uint)
			);

	if ((kStatus_Success == status) && (kStatus_Success == status2)) {
		/* Reset the counter when we reach the PWM period */
		SCTIMER_SetupCounterLimitAction(base, whichCounter, periodEventTemp);

		/* Return the period event to the user */
		*periodEvent = periodEventTemp;

		/* For high-true level */
		if ((uint32_t) pwmParams->level == (uint32_t) kSCTIMER_HighTrue) {
			/* Set the initial output level to low which is the inactive state */
			base->OUTPUT &= ~(1UL << (uint32_t) pwmParams->output);

			//if (mode == kSCTIMER_EdgeAlignedPwm)
			//{
			//    /* Set the output when we reach the PWM period */
			//    SCTIMER_SetupOutputSetAction(base, (uint32_t)pwmParams->output, periodEvent);
			//    /* Clear the output when we reach the PWM pulse value */
			//    SCTIMER_SetupOutputClearAction(base, (uint32_t)pwmParams->output, pulseEvent);
			//}
			//else
			//{
			/* Clear the output when we reach the PWM pulse event */
			SCTIMER_SetupOutputClearAction(base, (uint32_t) pwmParams->output,
					pulseEventTemp);
			/* Reverse output when down counting */
			reg = base->OUTPUTDIRCTRL;
			reg &= ~((uint32_t) SCT_OUTPUTDIRCTRL_SETCLR0_MASK
					<< (2U * (uint32_t) pwmParams->output));
			reg |= (1UL << (2U * (uint32_t) pwmParams->output));
			base->OUTPUTDIRCTRL = reg;
			//}
		}
		/* For low-true level */
		else {
			/* Set the initial output level to high which is the inactive state */
			base->OUTPUT |= (1UL << (uint32_t) pwmParams->output);

			//if (mode == kSCTIMER_EdgeAlignedPwm)
			//{
			//    /* Clear the output when we reach the PWM period */
			//    SCTIMER_SetupOutputClearAction(base, (uint32_t)pwmParams->output, periodEvent);
			//    /* Set the output when we reach the PWM pulse value */
			//    SCTIMER_SetupOutputSetAction(base, (uint32_t)pwmParams->output, pulseEvent);
			//}
			//else
			//{
			/* Set the output when we reach the PWM pulse event */
			SCTIMER_SetupOutputSetAction(base, (uint32_t) pwmParams->output,
					pulseEventTemp);
			/* Reverse output when down counting */
			reg = base->OUTPUTDIRCTRL;
			reg &= ~((uint32_t) SCT_OUTPUTDIRCTRL_SETCLR0_MASK
					<< (2U * (uint32_t) pwmParams->output));
			reg |= (1UL << (2U * (uint32_t) pwmParams->output));
			base->OUTPUTDIRCTRL = reg;
			//}
		}
		*pulseEvent = pulseEventTemp;
	}
	//}

	return status & status2;
}

