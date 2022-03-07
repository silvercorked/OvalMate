
#include <modules/steppers/stepperYctimer.h>

#include <stdio.h>
#include "fsl_debug_console.h"  // this gives access to PRINTF for debugging

// Variables
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

uint32_t ctimer_srcClock_Hz_Y;
// End Variables

void STEPPERY_startStepperPWM() {
    CTIMER_StartTimer(motorY_p->ctimer_p);
    motorY_p->running = true;
}

void STEPPERY_stopStepperPWM() {
	CTIMER_StopTimer(motorY_p->ctimer_p);
	motorY_p->running = false;
}

void STEPPERY_initializeStepperPWM() {
    /* CTimer3 counter uses the AHB clock, some CTimer1 modules use the Aysnc clock */
    ctimer_srcClock_Hz_Y = CTIMER_CLK_FREQ_Y;

    CTIMER_GetDefaultConfig(&(motorY_p->ctimerConfig));
    motorY_p->timerFreqPrescaled = ctimer_srcClock_Hz_Y / (motorY_p->ctimerConfig.prescale + 1);

    CTIMER_Init(motorY_p->ctimer_p, &(motorY_p->ctimerConfig));
}

void STEPPERY_setupStepperPWM(uint32_t steps, uint32_t freq) {
		stopStepperPWM(motorY_p);

		motorY_p->currentJob.goalSteps = steps;
		motorY_p->currentJob.currSteps = 0;

		CTIMER_RegisterCallBack(motorY_p->ctimer_p, &(motorY_p->periodCallback), kCTIMER_SingleCallback); // handles all 8 possible interrupt events
		CTIMER_setPWMPeriodValue(motorY_p, freq, DUTYCYCLE);
		CTIMER_SetupPwmPeriod(motorY_p->ctimer_p, CTIMER_MAT_PWM_PERIOD_CHANNEL, motorY_p->matchOutput, motorY_p->pwmPeriod, motorY_p->pulsePeriod, true);
}

status_t CTIMER_setPWMPeriodValue(stepperMotor_ctimer* m, uint32_t pwmFreqHz, uint8_t dutyCyclePercent) {
    /* Calculate PWM period match value */
    m->pwmPeriod = (m->timerFreqPrescaled / pwmFreqHz) - 1U;
    /* Calculate pulse width match value */
    m->pulsePeriod = (m->pwmPeriod + 1U) * (100 - dutyCyclePercent) / 100;

    return kStatus_Success;
}

void STEPPERY_generalCallback(uint32_t f) {
	//PRINTF("\r\n hit callback for motor %c, goalSteps = %d, currSteps = %d", whichMotor, m->currentJob.goalSteps, m->currentJob.currSteps);
	motorY_p->netSteps++;
	if (motorY_p->currentJob.currSteps == motorY_p->currentJob.goalSteps) {
		PRINTF("\r\n Job Done y");
		stopStepperPWM();
	}
	motorY_p->currentJob.currSteps++;
}

void STEPPERY_blockUntilMotorJobComplete() {
	while (motorY_p->running);
}
