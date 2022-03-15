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
 *	- 3/4/2022:
 *		Setup servo to be driven by a ctimer directly.
 *	- 3/9/2022:
 *		Added definition safeguards to avoid duplicate declarations
 *	- 3/13/2022:
 *		Set proper values for servo movement.
 */

#ifndef SERVO_H
#define SERVO_H

// Includes
#include <stdbool.h>
#include "fsl_ctimer.h"
// End Includes

// Definitions
#define CTIMER          CTIMER3         /* Timer 3 */
#define CTIMER_MAT_OUT  kCTIMER_Match_2 /* Match output 2 */
#define CTIMER_CLK_FREQ CLOCK_GetCTimerClkFreq(3U) // 3U for clock 3
							// 1MHz => 50Hz = 20000 counts => 20ms
							// 20% duty cycle = 2ms pulse = 90 degree
#define PENUP			9U	// 18% duty cycle = 1.8ms pulse = 54 degree
							// 15% duty cycle = 1.5ms pulse = 0 degrees
#define PENDOWN			7U	// 14% duty cycle = 1.4ms pulse = -9 degree
							// 10% duty cycle = 1ms pulse = -90 degree
							// https://content.instructables.com/ORIG/FA2/O1SS/J7ARLNBW/FA2O1SSJ7ARLNBW.pdf
#define SERVOFREQUENCYPWM 50U
#ifndef CTIMER_MAT_PWM_PERIOD_CHANNEL
#define CTIMER_MAT_PWM_PERIOD_CHANNEL kCTIMER_Match_3
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

// Macro Functions
	// this macro provides a more readable name for a programmer. Pass it PENUP or PENDOWN
#define SERVO_setPenMode(penMode) \
	SERVO_updatePWMDutyCycle(penMode)
// End Macro Functions

// Prototypes
void SERVO_startPWM(void);
void SERVO_stopPWM(void);
void SERVO_setupPWM(void);
status_t SERVO_getPWMPeriodValue(
	uint32_t pwmFreqHz,
	uint8_t dutyCyclePercent,
	uint32_t timerClockHz
);
status_t SERVO_updatePWMPulsePeriodValue(uint8_t dutyCyclePercent);
void SERVO_updatePWMDutyCycle(uint8_t dutyCycle);
void SERVO_initializePWM(void);
// End Prototypes

#endif
