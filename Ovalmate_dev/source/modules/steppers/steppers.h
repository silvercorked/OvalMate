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
 *	- 3/8/2022:
 *		Rewrote much of the steppers module in order to facilitate a new method of interacting with these timers. Progress towards this
 *		can be seen through the git commits
 *	- 3/9/2022:
 *		Went to lab to confirm and finalize stepper motor behavior. Acceleration looks good and moveTo and moveSteps are really useful from a
 *		coordinate perspective.
 *	- 3/13/2022:
 *		Testing at ES&S suggest max speed for these motors to be around 5 MHz, which is absurd. Upping max speed to 8KHz from 3KHz
 *		Swapped function names to be more acceptable of a module approach
 */

#ifndef STEPPERS_H
#define STEPPERS_H

#include "fsl_gpio.h"
#include "fsl_ctimer.h"
#include <stdbool.h>
#include "fsl_pint.h"
#include "fsl_inputmux.h"
#include <modules/utils/delays.h>

#include <stdio.h>
#include "fsl_debug_console.h"  // this gives access to PRINTF for debugging

// Definitions
#define CTIMER0_X				CTIMER0
#define CTIMER1_Y				CTIMER1
#define CTIMER0_MATCH0_CHANNEL	kCTIMER_Match_0
#define CTIMER1_MATCH0_CHANNEL	kCTIMER_Match_0

#define CTIMER0_CLK_FREQ		12000000U	// both of these are, by default, 12MHz clocks.
#define CTIMER1_CLK_FREQ		12000000U	// this is cause 12 divides cleanly more often than 10 => more whole number step counts

// CLOCK_GetCTimerClkFreq(0U) should get CTIMER0's clock freq, but reports PLL0 is outputting 0Hz
// manually setting clock freq of 12MHz

#define MAXSTEPPERFREQ			4000U
#define STARTSTEPPERFREQ		1000U
#define MAXSTEPPERSTEPS0		(CTIMER0_CLK_FREQ / (MAXSTEPPERFREQ * 2))	//  4000 steps
#define STARTSTEPPERSTEPS0		(CTIMER0_CLK_FREQ / (STARTSTEPPERFREQ * 2))	// 12000 steps
#define MAXSTEPPERSTEPS1		(CTIMER1_CLK_FREQ / (MAXSTEPPERFREQ * 2))
#define STARTSTEPPERSTEPS1		(CTIMER1_CLK_FREQ / (STARTSTEPPERFREQ * 2))
#define DIFFSTARTTOMAX0			STARTSTEPPERSTEPS0 - MAXSTEPPERSTEPS0
#define DIFFSTARTTOMAX1			STARTSTEPPERSTEPS1 - MAXSTEPPERSTEPS1

#define PINT_PIN_INT5_SRC			kINPUTMUX_GpioPort1Pin13ToPintsel // stepperX fault interrupt inputmux val
#define PINT_PIN_INT6_SRC			kINPUTMUX_GpioPort1Pin7ToPintsel  // stepperY fault interrupt inputmux val

#define PENIROFFSETX			-2170 // -2190
#define PENIROFFSETY			3660  // 3635
// End Definitions

// Enums
#ifndef STEPPERMOTORPININFORMATIONARROFFSET
#define STEPPERMOTORPININFORMATIONARROFFSET
typedef enum {
	PWMOUT = 0U,
	DIRECTION,
	ENABLE,
	RESET,
	SLEEP,
	HOME,
	FAULT
} stepperMotorPinInformationArrayOffset;
#endif
#ifndef STEPPERMOTORMODE_S
#define STEPPERMOTORMODE_S
typedef enum {
	IRMODE = 0U,
	PENMODE
} stepperMotorMode;
#endif
// End Enums

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
	pinInformation_s* pinInfo_arr_p[7];
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

// Macro Functions
//	Blocking
#define STEPPERS_moveToAccel(motor_p, steps) \
	STEPPERS_moveTo(motor_p, steps, true, true)
#define STEPPERS_moveToNoAccel(motor_p, steps) \
	STEPPERS_moveTo(motor_p, steps, false, true)
#define STEPPERS_moveRelativeAccel(motor_p, steps) \
	STEPPERS_moveRelative(motor_p, steps, true, true)
#define STEPPERS_moveRelativeNoAccel(motor_p, steps) \
	STEPPERS_moveRelative(motor_p, steps, false, true)
//	Non-blocking
#define STEPPERS_moveToAccelNoBlock(motor_p, steps) \
	STEPPERS_moveTo(motor_p, steps, true, false)
#define STEPPERS_moveToNoAccelNoBlock(motor_p, steps) \
	STEPPERS_moveTo(motor_p, steps, false, false)
#define STEPPERS_moveRelativeAccelNoBlock(motor_p, steps) \
	STEPPERS_moveRelative(motor_p, steps, true, false)
#define STEPPERS_moveRelativeNoAccelNoBlock(motor_p, steps) \
	STEPPERS_moveRelative(motor_p, steps, false, false)

//	Multi-Motor
//		Blocking
#define STEPPERS_moveBothToAccel(targetX, targetY) \
	STEPPERS_moveBothTo(targetX, targetY, true, true)
#define STEPPERS_moveBothToNoAccel(targetX, targetY) \
	STEPPERS_moveBothTo(targetX, targetY, false, true)
#define STEPPERS_moveBothRelativeAccel(stepsX, stepsY) \
	STEPPERS_moveBothRelative(stepsX, stepsY, true, true)
#define STEPPERS_moveBothRelativeNoAccel(stepsX, stepsY) \
	STEPPERS_moveBothRelative(stepsX, stepsY, false, true)
//		Non-blocking
#define STEPPERS_moveBothToAccelNoBlock(targetX, targetY) \
	STEPPERS_moveBothTo(targetX, targetY, true, false)
#define STEPPERS_moveBothToNoAccelNoBlock(targetX, targetY) \
	STEPPERS_moveBothTo(targetX, targetY, false, false)
#define STEPPERS_moveBothRelativeAccelNoBlock(stepsX, stepsY) \
	STEPPERS_moveBothRelative(stepsX, stepsY, true, false)
#define STEPPERS_moveBothRelativeNoAccelNoBlock(stepsX, stepsY) \
	STEPPERS_moveBothRelative(stepsX, stepsY, false, false)
// End Macro Functions

// Prototypes
void STEPPERS_initializePins(void);					// setup stepper motor output pins as GPIO ouputs
void STEPPERS_initializeMotors(void);					// must be called once before any ctimer interactions

status_t STEPPERS_setupMotor(stepperMotor_s*, uint32_t, bool);					// steps
void STEPPERS_driveSteps(stepperMotor_s*, uint32_t, bool);
status_t STEPPERS_moveRelative(stepperMotor_s*, int32_t, bool, bool);
status_t STEPPERS_moveTo(stepperMotor_s*, uint32_t, bool, bool);
status_t STEPPERS_moveBothRelative(int32_t, int32_t, bool, bool);
status_t STEPPERS_moveBothTo(uint32_t, uint32_t, bool, bool);

void STEPPERS_startMotor(stepperMotor_s*);
void STEPPERS_stopMotor(stepperMotor_s*);

status_t STEPPERS_setMotorStepsPerPhase(stepperMotorPhaseSteps_s*, uint32_t, bool);	// assigns step values to stepperMotorPhaseSteps pointer

void STEPPERS_setHome(stepperMotor_s* motor_p);

stepperMotorMode STEPPERS_switchMode();
stepperMotorMode STEPPERS_getCurrentMode();
stepperMotorMode STEPPERS_setCurrentMode(stepperMotorMode mode);

void STEPPERS_XTimerCallback(uint32_t);				// flags
void STEPPERS_YTimerCallback(uint32_t);				// flags
void STEPPERS_generalTimerCallback(uint32_t, stepperMotor_s*);

void STEPPERS_XFault(pint_pin_int_t pintr, uint32_t pmatch_status);
void STEPPERS_YFault(pint_pin_int_t pintr, uint32_t pmatch_status);

void STEPPERS_sleepMotor(stepperMotor_s*);
void STEPPERS_wakeMotor(stepperMotor_s*);

void STEPPERS_writeOutputPin(pinInformation_s*, bool);
bool STEPPERS_readInputPin(pinInformation_s*);

void STEPPERS_writeDirectionPin(stepperMotor_s*, bool);
void STEPPERS_writeEnablePin(stepperMotor_s*, bool);
void STEPPERS_writeResetPin(stepperMotor_s*, bool);
void STEPPERS_writeSleepPin(stepperMotor_s*, bool);
bool STEPPERS_readHomePin(stepperMotor_s*);

bool debounceFault(pinInformation_s*);
// End Prototypes

#endif
