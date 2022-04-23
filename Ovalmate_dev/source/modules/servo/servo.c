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
 *	- 3/19/2022:
 *		Added comments to all functions in preparation for Software Design Review
 */

#include "servo.h"

// Variables
volatile uint32_t g_pwmPeriod   = 0U;
volatile uint32_t g_pulsePeriod = 0U;
ctimer_config_t ctimer_config;
uint32_t ctimer_srcClock_Hz;
uint32_t ctimer_timerClock;
bool servoMotorAllowedToDrive = true;
// End Variables

// Functions
/**
 * Initialize both servo motor.
 *
 * Run this once before using the servo motor. No need to call again.
 *
 * @params	- None
 * @return	- None
 */
void SERVO_initializePWM() {
    /* CTimer3 counter uses the AHB clock, some CTimer1 modules use the Aysnc clock */
    ctimer_srcClock_Hz = CTIMER_CLK_FREQ;

    CTIMER_GetDefaultConfig(&ctimer_config);
    ctimer_timerClock = ctimer_srcClock_Hz / (ctimer_config.prescale + 1);

    CTIMER_Init(CTIMER, &ctimer_config);
}

/**
 * Sets up the initial state for the servo motor and calculates the duty cycle.
 * Sets up match register of CTIMER associated with the servo motor.
 *
 * @params	- None
 * @return	- None
 */
void SERVO_setupPWM() {
    /* Get the PWM period match value and pulse width match value of 50hz PWM signal with 18% dutycycle */
	SERVO_getPWMPeriodValue(SERVOFREQUENCYPWM, PENUP, ctimer_timerClock);
    CTIMER_SetupPwmPeriod(CTIMER, CTIMER_MAT_PWM_PERIOD_CHANNEL, CTIMER_MAT_OUT, g_pwmPeriod, g_pulsePeriod, false);
}

/**
 * Modifies the running timer with a new duty cycle. This is used to change from PENUP to PENDOWN.
 *
 * @params	- uint8_t dutyCycle	: the desired dutycycle.
 * @return	- None
 */
void SERVO_updatePWMDutyCycle(uint8_t dutyCycle) {
	if (!servoMotorAllowedToDrive) return;
	SERVO_updatePWMPulsePeriodValue(dutyCycle);
	CTIMER_UpdatePwmPulsePeriod(CTIMER, CTIMER_MAT_OUT, g_pulsePeriod);
}

/**
 * Starts the Servo's ctimer.
 *
 * @params	- None
 * @return	- None
 */
void SERVO_startPWM() {
	if (!servoMotorAllowedToDrive) return;
    CTIMER_StartTimer(CTIMER);
}

/**
 * Stops the Servo's ctimer.
 *
 * @params	- None
 * @return	- None
 */
void SERVO_stopPWM() {
	CTIMER_StopTimer(CTIMER);
}

/**
 * Calculate the match value for the period.
 * Calculate the proportion of the period where the signal should be held high to satisfy a given dutyCycle.
 *
 * @params	- uint32_t pwmFreqHz		: the desired frequency of the period.
 * 			- uint8_t dutyCyclePercent	: the desired dutycycle as a percent from 0-100
 * 			- uint32_t timerClock_Hz	: the timer's clock frequency
 * @return	- status_t					: always kStatus_Success
 */
status_t SERVO_getPWMPeriodValue(uint32_t pwmFreqHz, uint8_t dutyCyclePercent, uint32_t timerClock_Hz) {
    /* Calculate PWM period match value */
    g_pwmPeriod = (timerClock_Hz / pwmFreqHz) - 1U;
    /* Calculate pulse width match value */
    g_pulsePeriod = (g_pwmPeriod + 1U) * (100 - dutyCyclePercent) / 100;

    return kStatus_Success;
}

/**
 * Calculate the amount proportion of the period where the signal should be held high to satisfy a given dutyCycle.
 *
 * @params	- uint8_t dutyCyclePercent	: the desired dutycycle as a percent from 0-100
 * @return	- status_t					: always kStatus_Success
 */
status_t SERVO_updatePWMPulsePeriodValue(uint8_t dutyCyclePercent) {
    /* Calculate pulse width match value */
    g_pulsePeriod = (g_pwmPeriod + 1U) * (100 - dutyCyclePercent) / 100;

    return kStatus_Success;
}
// End Functions
