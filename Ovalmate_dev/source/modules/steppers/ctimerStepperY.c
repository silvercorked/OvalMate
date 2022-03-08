
#include "ctimerStepperY.h"

#include <stdio.h>
#include "fsl_debug_console.h"  // this gives access to PRINTF for debugging

// Variables
stepperMotor_ctimer_t motorY = {
	.pwmPeriod = 0U,
	.pulsePeriod = 0U,
	.timerFreqPrescaled = 0U,
	.running = false,
	.netSteps = 0U,
	.currentJob = {
		.currSteps = 0U,
		.goalSteps = 0U
	},
	.ctimer_p = CTIMER_MOTOR_Y_OUT,
	.matchOutput = CTIMER_MAT_OUT_Y
};
stepperMotor_ctimer_t* motorY_p = &motorY;
// End Variables

void STEPPERY_initializeStepperPWM() {

    if (&(motorY_p->ctimerConfig) != NULL)
    	STEPPERY_stopStepperPWM(motorY_p); // stop motors on init

    CTIMER_GetDefaultConfig(&(motorY_p->ctimerConfig));
    motorY_p->timerFreqPrescaled = CTIMER_CLK_FREQ_Y / (motorY_p->ctimerConfig.prescale + 1);
    //PRINTF("setting ctimer_timerClock to %d given that ctimer_srcClock_Hz is %d and prescale is %d",
	//	motorY_p->timerFreqPrescaled,
	//	ctimer_srcClock_Hz_Y,
	//	motorY_p->ctimerConfig.prescale
	//);

    CTIMER_Init(motorY_p->ctimer_p, &(motorY_p->ctimerConfig));
}

void STEPPERY_setupStepperPWM(uint32_t steps, uint32_t freq) {
	motorY_p->currentJob.goalSteps = steps;
	motorY_p->currentJob.currSteps = 0;

	CTIMER_RegisterCallBack(motorY_p->ctimer_p, &(motorY_p->periodCallback), kCTIMER_SingleCallback); // handles all 8 possible interrupt events
	CTIMER_setPWMPeriodValue(freq, DUTYCYCLE);
	CTIMER_SetupPwmPeriod(motorY_p->ctimer_p, CTIMER_MAT_PWM_PERIOD_CHANNEL, motorY_p->matchOutput, motorY_p->pwmPeriod, motorY_p->pulsePeriod, true);

	//PRINTF("\n starting stepper pwm with period channel %d, output channel %d, period %d, pulsePeriod %d"
	//	, CTIMER_MAT_PWM_PERIOD_CHANNEL
	//	, motorY_p->matchOutput
	//	, motorY_p->pwmPeriod
	//	, motorY_p->pulsePeriod
	//);
}

void STEPPERY_startStepperPWM() {
    CTIMER_StartTimer(motorY_p->ctimer_p);
    motorY_p->running = true;
}

void STEPPERY_stopStepperPWM() {
	CTIMER_StopTimer(motorY_p->ctimer_p);
	motorY_p->running = false;
}

status_t CTIMER_setPWMPeriodValue(uint32_t pwmFreqHz, uint8_t dutyCyclePercent) {
    /* Calculate PWM period match value */
	motorY_p->pwmPeriod = (motorY_p->timerFreqPrescaled / pwmFreqHz) - 1U;
    /* Calculate pulse width match value */
	motorY_p->pulsePeriod = (motorY_p->pwmPeriod + 1U) * (100 - dutyCyclePercent) / 100;

    return kStatus_Success;
}

void STEPPERY_blockUntilMotorJobComplete() {
	while (motorY_p->running);
}

stepperMotor_ctimer_t* getMotorY() {
	return motorY_p;
}
