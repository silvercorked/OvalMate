
#include <modules/steppers/ctimerSteppers.h>

#include <stdio.h>
#include "fsl_debug_console.h"  // this gives access to PRINTF for debugging

// Variables
stepperMotor_ctimer motorX = {
	.pwmPeriod = 0U,
	.pulsePeriod = 0U,
	.timerFreqPrescaled = 0U,
	.running = false,
	.netSteps = 0U,
	.currentJob = {
		.currSteps = 0U,
		.goalSteps = 0U
	},
	.ctimer_p = X_OUT_TIMER,
	.matchOutput = CTIMER_MAT_OUT_X,
	.periodCallback = ctimerXCallback
};
stepperMotor_ctimer* motorX_p = &motorX;
stepperMotor_ctimer motorY = {
	.pwmPeriod = 0U,
	.pulsePeriod = 0U,
	.timerFreqPrescaled = 0U,
	.running = false,
	.netSteps = 0U,
	.currentJob = {
		.currSteps = 0U,
		.goalSteps = 0U
	},
	.ctimer_p = Y_OUT_TIMER,
	.matchOutput = CTIMER_MAT_OUT_Y,
	.periodCallback = ctimerYCallback
};
stepperMotor_ctimer* motorY_p = &motorY;

uint32_t ctimer_srcClock_Hz_X;
uint32_t ctimer_srcClock_Hz_Y;
// End Variables

void initializeStepperPWM() {
    /* CTimer3 counter uses the AHB clock, some CTimer1 modules use the Aysnc clock */
    ctimer_srcClock_Hz_X = CTIMER_CLK_FREQ_X;
    ctimer_srcClock_Hz_Y = CTIMER_CLK_FREQ_Y;

    CTIMER_GetDefaultConfig(&(motorX_p->ctimerConfig));
    motorX_p->timerFreqPrescaled = ctimer_srcClock_Hz_X / (motorX_p->ctimerConfig.prescale + 1);
    CTIMER_GetDefaultConfig(&(motorY_p->ctimerConfig));
    motorY_p->timerFreqPrescaled = ctimer_srcClock_Hz_Y / (motorY_p->ctimerConfig.prescale + 1);

    //PRINTF("setting ctimer_timerClock to %d given that ctimer_srcClock_Hz is %d and prescale is %d",
	//	motorX_p->timerFreqPrescaled,
	//	ctimer_srcClock_Hz_X,
	//	motorX_p->ctimerConfig.prescale
	//);
    //PRINTF("setting ctimer_timerClock to %d given that ctimer_srcClock_Hz is %d and prescale is %d",
	//	motorY_p->timerFreqPrescaled,
	//	ctimer_srcClock_Hz_Y,
	//	motorY_p->ctimerConfig.prescale
	//);

    CTIMER_Init(motorX_p->ctimer_p, &(motorX_p->ctimerConfig));
    CTIMER_Init(motorY_p->ctimer_p, &(motorY_p->ctimerConfig));
}

void setupStepperPWM(which_motor_t whichMotor, uint32_t steps, uint32_t freq) {
	if (whichMotor != MOTORBOTH) {
		stepperMotor_ctimer* m = getMotor(whichMotor);

		stopStepperPWM(m);

		m->currentJob.goalSteps = steps;
		m->currentJob.currSteps = 0;

		CTIMER_RegisterCallBack(m->ctimer_p, &(m->periodCallback), kCTIMER_SingleCallback); // handles all 8 possible interrupt events
		CTIMER_setPWMPeriodValue(m, freq, DUTYCYCLE);
		CTIMER_SetupPwmPeriod(m->ctimer_p, CTIMER_MAT_PWM_PERIOD_CHANNEL, m->matchOutput, m->pwmPeriod, m->pulsePeriod, true);

	    //PRINTF("\n starting stepper pwm with period channel %d, output channel %d, period %d, pulsePeriod %d"
		//	, CTIMER_MAT_PWM_PERIOD_CHANNEL
		//	, m->matchOutput
		//	, m->pwmPeriod
		//	, m->pulsePeriod
		//);
		return;
	}
	setupStepperPWM(MOTORX, steps, freq);
	setupStepperPWM(MOTORY, steps, freq);
}

void startStepperPWM(stepperMotor_ctimer* m) {
    CTIMER_StartTimer(m->ctimer_p);
    m->running = true;
}

void stopStepperPWM(stepperMotor_ctimer* m) {
	CTIMER_StopTimer(m->ctimer_p);
	m->running = false;
}

status_t CTIMER_setPWMPeriodValue(stepperMotor_ctimer* m, uint32_t pwmFreqHz, uint8_t dutyCyclePercent) {
    /* Calculate PWM period match value */
    m->pwmPeriod = (m->timerFreqPrescaled / pwmFreqHz) - 1U;
    /* Calculate pulse width match value */
    m->pulsePeriod = (m->pwmPeriod + 1U) * (100 - dutyCyclePercent) / 100;

    return kStatus_Success;
}

void ctimerXCallback(uint32_t f) {
	ctimerGeneralCallback(f, MOTORX);
}
void ctimerYCallback(uint32_t f) {
	ctimerGeneralCallback(f, MOTORY);
}
void ctimerGeneralCallback(uint32_t flags, which_motor_t whichMotor) {
	stepperMotor_ctimer* m = getMotor(whichMotor);
	//PRINTF("\r\n hit callback for motor %c, goalSteps = %d, currSteps = %d", whichMotor, m->currentJob.goalSteps, m->currentJob.currSteps);
	m->netSteps++;
	if (m->currentJob.currSteps == m->currentJob.goalSteps) {
		PRINTF("\r\n Job Done %c", whichMotor);
		stopStepperPWM(m);
	}
	m->currentJob.currSteps++;
}

void blockUntilMotorJobComplete(which_motor_t whichMotor) {
	if (whichMotor == MOTORBOTH)
		while (motorX_p->running || motorY_p->running);
	else if (whichMotor == MOTORX)
		while (motorX_p->running);
	else if (whichMotor == MOTORY)
		while (motorY_p->running);
}

stepperMotor_ctimer* getMotor(which_motor_t whichMotor) {
	if (whichMotor == MOTORY) {
		return motorY_p;
	}
	return motorX_p;
}
which_motor_t whichMotor(stepperMotor_ctimer* m) {
	if (m->ctimer_p == motorY_p->ctimer_p)
		return MOTORY;
	return MOTORX;
}
