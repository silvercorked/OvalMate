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
 */

#include "steppers.h"

// Variables
sctimer_config_t sctimerInfo;
uint32_t sctimerClock;
stepperMotor motorX = {
	.direction = 0,					// bool
	.netSteps = 0,					// keep track of net steps, so (steps in 0 dir) - (steps in 1 dir)
	.pwmEventPeriod = 0,	// uninitialized
	.pwmEventPulse = 1,	// uninitialized
	.pwmParam = {
		.output = X_SCTIMER_OUT,
		.level = kSCTIMER_HighTrue,
		.dutyCyclePercent = 50
	},
	.timer = kSCTIMER_Counter_L,
	.currentJob = {}
};
stepperMotor motorY = {
	.direction = 0,
	.netSteps = 0,
	.pwmEventPeriod = 2,	// uninitialized
	.pwmEventPulse = 3,	// uninitialized
	.pwmParam = {
		.output = Y_SCTIMER_OUT,
		.level = kSCTIMER_HighTrue,
		.dutyCyclePercent = 50
	},
	.timer = kSCTIMER_Counter_H,
	.currentJob = {}
};
void* callbackX;
void* callbackY;
// End Variables

void stepCompleteMotorXCallback() {
	motorX.netSteps += motorX.direction == 0 ? 1 : -1; // update netSteps
	motorX.currentJob.currSteps++;
	if (motorX.currentJob.currSteps == motorX.currentJob.goalSteps)
		stopStepperPWM(motorX.timer);
}

void stepCompleteMotorYCallback() {
	motorY.netSteps += motorY.direction == 0 ? 1 : -1; // update netSteps
	motorY.currentJob.currSteps++;
	if (motorY.currentJob.currSteps == motorY.currentJob.goalSteps)
		stopStepperPWM(motorY.timer);
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

status_t setupStepperPWM(stepperMotor motor, uint32_t stepsToAssign) {
	if (motor.pwmEventPeriod < 0xFFFFFFF0) {
		SCTIMER_DisableInterrupts(SCT0, (1 << motor.pwmEventPeriod)); // clear interrupts
		SCTIMER_DisableInterrupts(SCT0, (1 << motor.pwmEventPulse)); // not yet set
	}
	stepperJob newJob = {
		.goalSteps = stepsToAssign, // create and overwrite job
		.currSteps = 0
	};
	motor.currentJob = newJob; // set job

	if (setupPwm16BitMode(			// load and config timer
		SCT0,						// sctimer module
		&(motor.pwmParam),			// pwmParam per motor (only diff is output pin)
		2000U,						// frequency
		sctimerClock,				// sctimer clock frequency
		motor.timer,
		&(motor.pwmEventPeriod),	// event pointer (set internally)
		&(motor.pwmEventPulse)		// event pointer (set internally)
	) == kStatus_Fail) {			// fails if there isn't space for events
		return kStatus_Fail;
	}
	SCTIMER_EnableInterrupts(SCT0, (1 << motor.pwmEventPeriod)); // set interrupt enable for new event
	SCTIMER_SetCallback(SCT0, motor.periodCallback, motor.pwmEventPeriod); // set callback to standard callback

	return kStatus_Success;
}

void startStepperPWM(sctimer_counter_t whichCounter) {
    SCTIMER_StartTimer(SCT0, whichCounter);
}

void stopStepperPWM(sctimer_counter_t whichCounter) {
	SCTIMER_StopTimer(SCT0, whichCounter);
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
status_t setupPwm16BitMode(
	SCT_Type *base,
	const sctimer_pwm_signal_param_t *pwmParams,
	uint32_t pwmFreq_Hz,
	uint32_t srcClock_Hz,
	sctimer_counter_t whichCounter,
	uint32_t *periodEvent,
	uint32_t *pulseEvent
) {
    assert(NULL != pwmParams);
    assert(0U != srcClock_Hz);
    assert(0U != pwmFreq_Hz);
    assert((uint32_t)pwmParams->output < (uint32_t)FSL_FEATURE_SCT_NUMBER_OF_OUTPUTS);
    assert(0U == (base->CONFIG & SCT_CONFIG_UNIFY_MASK));

    /* If we do not have enough events available (this function will create two events),
     * the function will return fail.
     */
    status_t status = kStatus_Fail;
    status_t status2;
    uint32_t period, pulsePeriod = 0;
    uint32_t sctClock    = srcClock_Hz / (((base->CTRL & SCT_CTRL_PRE_L_MASK) >> SCT_CTRL_PRE_L_SHIFT) + 1U);
    uint32_t periodEventTemp = 0, pulseEventTemp = 0;
    uint32_t reg;
    //sctimer_pwm_mode_t mode = kSCTIMER_CenterAlignedPwm;

    SCTIMER_ClearEventInState(base, *periodEvent, SCTIMER_GetCurrentState(SCT0));
    SCTIMER_ClearEventInState(base, *pulseEvent, SCTIMER_GetCurrentState(SCT0)); // clear old events

    // check we have enough space for events // don't need to check if we only ever use 2
    //if ((s_currentEvent + 2U) <= (uint32_t)FSL_FEATURE_SCT_NUMBER_OF_EVENTS) {
	/* Use bi-directional mode for center-aligned PWM */
	//if (mode == kSCTIMER_CenterAlignedPwm) {
		base->CTRL |= SCT_CTRL_BIDIR_L_MASK;
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

	pulsePeriod = (uint32_t)(((uint64_t)period * pwmParams->dutyCyclePercent) / 100U);

	if (pwmParams->dutyCyclePercent >= 100U) {
		//if (mode == kSCTIMER_EdgeAlignedPwm) {
		//    pulsePeriod = period + 2U;
		//}
		//else {
			pulsePeriod = period - 1U;
		//}
	}

	/* Schedule an event when we reach the PWM period */
	status = SCTIMER_CreateAndScheduleEvent(
		base,						// STC0
		kSCTIMER_MatchEventOnly,	// after complete period
		period,						// number of timer ticks
		0,							// input or output affected. MatchEventOnly causes this param to be ignored
		whichCounter,				// which counter desired
		&periodEventTemp			// event pointer (uint)
	);

	/* Schedule an event when we reach the pulse width */
	status2 = SCTIMER_CreateAndScheduleEvent(
		base,						// SCT0
		kSCTIMER_MatchEventOnly,	// after complete period
		pulsePeriod,				// number of timer ticks
		0,							// input or output affected. MatchEventOnly causes this param to be ignored
		whichCounter,				// which counter desired
		&pulseEventTemp				// event pointer (uint)
	);

	if ((kStatus_Success == status) && (kStatus_Success == status2))
	{
		/* Reset the counter when we reach the PWM period */
		SCTIMER_SetupCounterLimitAction(base, whichCounter, periodEventTemp);

		/* Return the period event to the user */
		*periodEvent = periodEventTemp;

		/* For high-true level */
		if ((uint32_t)pwmParams->level == (uint32_t)kSCTIMER_HighTrue)
		{
			/* Set the initial output level to low which is the inactive state */
			base->OUTPUT &= ~(1UL << (uint32_t)pwmParams->output);

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
				SCTIMER_SetupOutputClearAction(base, (uint32_t)pwmParams->output, pulseEventTemp);
				/* Reverse output when down counting */
				reg = base->OUTPUTDIRCTRL;
				reg &= ~((uint32_t)SCT_OUTPUTDIRCTRL_SETCLR0_MASK << (2U * (uint32_t)pwmParams->output));
				reg |= (1UL << (2U * (uint32_t)pwmParams->output));
				base->OUTPUTDIRCTRL = reg;
			//}
		}
		/* For low-true level */
		else
		{
			/* Set the initial output level to high which is the inactive state */
			base->OUTPUT |= (1UL << (uint32_t)pwmParams->output);

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
				SCTIMER_SetupOutputSetAction(base, (uint32_t)pwmParams->output, pulseEventTemp);
				/* Reverse output when down counting */
				reg = base->OUTPUTDIRCTRL;
				reg &= ~((uint32_t)SCT_OUTPUTDIRCTRL_SETCLR0_MASK << (2U * (uint32_t)pwmParams->output));
				reg |= (1UL << (2U * (uint32_t)pwmParams->output));
				base->OUTPUTDIRCTRL = reg;
			//}
		}
		*pulseEvent = pulseEventTemp;
	}
    //}

    return status & status2;
}
