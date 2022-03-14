/**
 * Title: OvalMate_servo_module_DevBoard
 * Version: 0.0.1
 * Filename: LPC55S16_Project_main_dev.c
 * Authors: Alex Wissing
 * Purpose of Program: Manage servo motor
 * How Program is Run on Target System:
 * Date Started: 2/28/2022
 * Update History:
 *	- 2/28/2022
 *		Moved this into separate "modules"
 *	- 3/4/2022:
 *		Setup servo to be driven by a ctimer directly.
 */

#include "servo.h"

// Variables
volatile uint32_t g_pwmPeriod   = 0U;
volatile uint32_t g_pulsePeriod = 0U;
ctimer_config_t ctimer_config;
uint32_t ctimer_srcClock_Hz;
uint32_t ctimer_timerClock;
// End Variables

void initializeServoPWM() {
    /* CTimer3 counter uses the AHB clock, some CTimer1 modules use the Aysnc clock */
    ctimer_srcClock_Hz = CTIMER_CLK_FREQ;

    //PRINTF("CTimer example to generate a PWM signal\r\n");

    CTIMER_GetDefaultConfig(&ctimer_config);
    ctimer_timerClock = ctimer_srcClock_Hz / (ctimer_config.prescale + 1);
    //PRINTF("setting ctimer_timerClock to %d given that ctimer_srcClock_Hz is %d and prescale is %d"
    //		, ctimer_timerClock
	//		, ctimer_srcClock_Hz
	//		, ctimer_config.prescale
	//);
    CTIMER_Init(CTIMER, &ctimer_config);
}

void setupServoPWM() {
    /* Get the PWM period match value and pulse width match value of 50hz PWM signal with 18% dutycycle */
    CTIMER_getPWMPeriodValue(SERVOFREQUENCYPWM, PENUP, ctimer_timerClock);
    CTIMER_SetupPwmPeriod(CTIMER, CTIMER_MAT_PWM_PERIOD_CHANNEL, CTIMER_MAT_OUT, g_pwmPeriod, g_pulsePeriod, false);
    //PRINTF("\n starting servo pwm with period channel %d, output channel %d, period %d, pulsePeriod %d"
	//	, CTIMER_MAT_PWM_PERIOD_CHANNEL
	//	, CTIMER_MAT_OUT
	//	, g_pwmPeriod
	//	, g_pulsePeriod
	//);
}

void updateServoPWMDutyCycle(uint8_t dutyCycle) {
	CTIMER_updatePWMPulsePeriodValue(dutyCycle);
	CTIMER_UpdatePwmPulsePeriod(CTIMER, CTIMER_MAT_OUT, g_pulsePeriod);
}

void startServoPWM() {
    CTIMER_StartTimer(CTIMER);
}

void stopServoPWM() {
	CTIMER_StopTimer(CTIMER);
}

status_t CTIMER_getPWMPeriodValue(uint32_t pwmFreqHz, uint8_t dutyCyclePercent, uint32_t timerClock_Hz) {
    /* Calculate PWM period match value */
    g_pwmPeriod = (timerClock_Hz / pwmFreqHz) - 1U;
    /* Calculate pulse width match value */
    g_pulsePeriod = (g_pwmPeriod + 1U) * (100 - dutyCyclePercent) / 100;

    return kStatus_Success;
}

status_t CTIMER_updatePWMPulsePeriodValue(uint8_t dutyCyclePercent)
{
    /* Calculate pulse width match value */
    g_pulsePeriod = (g_pwmPeriod + 1U) * (100 - dutyCyclePercent) / 100;

    return kStatus_Success;
}
