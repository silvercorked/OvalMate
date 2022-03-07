/**
 * Title: OvalMate_servo_module_DevBoard
 * Version: 0.0.1
 * Filename: LPC55S16_Project_main_dev.c
 * Authors: Alex Wissing
 * Purpose of Program: Manage servo motor
 * How Program is Run on Target System:
 * Date Started: 2/28/2022
 *	- 2/28/2022
 *		Moved this into separate "modules"
 */

// Includes
#include <stdbool.h>
#include "fsl_ctimer.h"
// End Includes

// Definitions
#define CTIMER          CTIMER3         /* Timer 3 */
#define CTIMER_MAT_OUT  kCTIMER_Match_2 /* Match output 2 */
#define CTIMER_CLK_FREQ CLOCK_GetCTimerClkFreq(3U) // 3U for clock 3
							// 1MHz => 50Hz = 20000 counts => 20ms
#define PENDOWN			10U	// 20% duty cycle = 2ms pulse = 90 degree
#define PENUP			7U	// 14% duty cylce = 1.5ms pulse = 0 degree
							// 10% duty cycle = 1ms pulse = -90 degree
							// https://content.instructables.com/ORIG/FA2/O1SS/J7ARLNBW/FA2O1SSJ7ARLNBW.pdf
#define SERVOFREQUENCYPWM 50U
#ifndef CTIMER_MAT_PWM_PERIOD_CHANNEL
#define CTIMER_MAT_PWM_PERIOD_CHANNEL kCTIMER_Match_3 // what's this for?
#endif
// End Definitions

// Extern Variables
// Variables
extern volatile uint32_t g_pwmPeriod;
extern volatile uint32_t g_pulsePeriod;
extern ctimer_config_t ctimer_config;
extern uint32_t ctimer_srcClock_Hz;
extern uint32_t ctimer_timerClock;
// End Variables
// End Extern Variables

// Prototypes
void startServoPWM(void);
void stopServoPWM(void);
void setupServoPWM(void);
status_t CTIMER_getPWMPeriodValue(
	uint32_t pwmFreqHz,
	uint8_t dutyCyclePercent,
	uint32_t timerClockHz
);
status_t CTIMER_updatePWMPulsePeriodValue(uint8_t dutyCyclePercent);
void updateServoPWMDutyCycle(uint8_t dutyCycle);
void initializeServoPWM(void);
// End Prototypes
