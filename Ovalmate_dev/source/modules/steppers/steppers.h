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

#include "fsl_gpio.h"
#include "fsl_ctimer.h"
#include <stdbool.h>

//#include <stdio.h>
//#include "fsl_debug_console.h"  // this gives access to PRINTF for debugging

#ifndef STEPPERS_H
#define STEPPERS_H

// Definitions
#define CTIMER0_X				CTIMER0
#define CTIMER1_Y				CTIMER1
#define CTIMER0_MATCH0_CHANNEL	kCTIMER_Match_0
#define CTIMER1_MATCH0_CHANNEL	kCTIMER_Match_0

#define CTIMER0_CLK_FREQ		12000000U	// both of these are, by default, 12MHz clocks.
#define CTIMER1_CLK_FREQ		12000000U	// this is cause 12 divides cleanly more often than 10 => more whole number step counts

// CLOCK_GetCTimerClkFreq(0U) should get CTIMER0's clock freq, but reports PLL0 is outputting 0Hz
// manually setting clock freq of 12MHz

#define MAXSTEPPERFREQ			3000U
#define STARTSTEPPERFREQ		1000U
#define MAXSTEPPERSTEPS0		(CTIMER0_CLK_FREQ / (MAXSTEPPERFREQ * 2))	//  4000 steps
#define STARTSTEPPERSTEPS0		(CTIMER0_CLK_FREQ / (STARTSTEPPERFREQ * 2))	// 12000 steps
#define MAXSTEPPERSTEPS1		(CTIMER1_CLK_FREQ / (MAXSTEPPERFREQ * 2))
#define STARTSTEPPERSTEPS1		(CTIMER1_CLK_FREQ / (STARTSTEPPERFREQ * 2))
#define DIFFSTARTTOMAX0			STARTSTEPPERSTEPS0 - MAXSTEPPERSTEPS0
#define DIFFSTARTTOMAX1			STARTSTEPPERSTEPS1 - MAXSTEPPERSTEPS1
// End Definitions

// Structs
#ifndef PININFORMATION_S
#define PININFORMATION_S
typedef struct {
	uint8_t port;
	uint8_t pin;
} pinInformation_s;
#endif
#ifndef STEPPERMOTORSTATUS_S
#define STEPPERMOTORSTATUS_S
typedef struct {
	bool running;			// if timer is started, this should be on until timer is halted
	bool startSpeed;		// currently running at start speed
	bool accelerating;		// should be on during the speeding up phase as the timer moves from start freq to max freq
	bool steady;			// after reaching max speed, this should be high
	bool slowing;			// should be on during the slowing down phase when the timer moves from max freq to start freq
} stepperMotorStatus_s;
#endif
#ifndef STEPPERMOTORPHASESTEPS_S
#define STEPPERMOTORPHASESTEPS_S
typedef struct {
	uint32_t startSpeed;		// number of steps with no acceleration at start speed (beginning of steps). This one exists for movements that never accelerate
	uint32_t accelerating;		// number of steps with acceleration (beginning of steps)
	uint32_t steady;			// number of steps at max speed (middle of steps
	uint32_t slowing;			// number of steps decelerating (end of steps)
	float accelValue;			// the rate of acceleration (increase in match value per period)
	float roundedAccel;			// when applying accel to increase match value, the excess decimal should be help (using in accelerating and slowing)
} stepperMotorPhaseSteps_s;
#endif
#ifndef STEPPERMOTOR_S
#define STEPPERMOTOR_S
typedef struct {
	CTIMER_Type* timer_p;
	pinInformation_s* pinInfo_p;
	stepperMotorPhaseSteps_s* phaseSteps_p;
	ctimer_callback_t matchCallback;	// function ** but only points to 1 functions. CTIMER_RegisterCallBack expects array (ie void**) because it can take multiple interrupts, but we want all to go to this function.
	ctimer_match_t	output;
	ctimer_match_config_t matchConfig;	// also small and we'll need it often
	stepperMotorStatus_s status;			// not a pointer for faster access cause it's small and we need it often
	uint32_t position;
	bool direction;
} stepperMotor_s;
#endif
// End Structs

// Extern Variables
extern stepperMotor_s* stepperX_p;
extern stepperMotor_s* stepperY_p;
// End Extern Variables

// Prototypes
void initializeStepperOutputPins();					// setup stepper motor output pins as GPIO ouputs
void writeStepperOutputPin(pinInformation_s* pInfo, bool highLow);

void initializeStepperMotors();											// must be called once before any ctimer interactions
status_t setupStepperMotor(stepperMotor_s*, uint32_t);					// steps
void driveStepperSteps(stepperMotor_s* motor_p, uint32_t steps);
status_t moveSteps(stepperMotor_s* motor_p, int32_t steps);
status_t moveTo(stepperMotor_s* motor_p, uint32_t target);
status_t setMotorStepsPerPhase(stepperMotorPhaseSteps_s*, uint32_t);	// assigns step values to stepperMotorPhaseSteps pointer
void startMotor(stepperMotor_s*);
void stopMotor(stepperMotor_s*);

void setHome(stepperMotor_s* motor_p);

void stepperXTimerCallback(uint32_t);				// flags
void stepperYTimerCallback(uint32_t);				// flags
void stepperGeneralTimerCallback(uint32_t, stepperMotor_s*);
// End Prototypes

#endif
