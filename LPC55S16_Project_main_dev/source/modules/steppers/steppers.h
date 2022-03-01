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

// Includes
#include "fsl_sctimer.h"
// End Includes

// Definitions
#define SCTIMER_CLK_FREQ        CLOCK_GetFreq(kCLOCK_BusClk)
#define X_SCTIMER_OUT			kSCTIMER_Out_0
#define Y_SCTIMER_OUT			kSCTIMER_Out_1
// End Definitions

typedef struct __stepper_job {
	uint32_t goalSteps;
	uint32_t currSteps;
} stepperJob;

typedef struct __stepper_motor {
	sctimer_pwm_signal_param_t pwmParam;
	bool direction;
	uint32_t pwmEventPeriod; // when a period finishes
	uint32_t pwmEventPulse; // when the duty cycle completes
	stepperJob currentJob;
	sctimer_counter_t timer;
	void* periodCallback;
	int32_t netSteps; // before deactivating, this must be divisible by 16
	// as we've been micro-stepping this whole time
} stepperMotor;

// Extern Variables
extern sctimer_config_t sctimerInfo;
extern uint32_t sctimerClock;
extern stepperMotor motorX;
extern stepperMotor motorY;
// End Extern Variables

// Prototypes
void initializeStepperPWM(void);
status_t setupStepperPWM(
	stepperMotor motor,
	uint32_t stepsToAssign
);
status_t setupPwm16BitMode(
	SCT_Type *base,
	const sctimer_pwm_signal_param_t *pwmParams,
	uint32_t pwmFreq_Hz,
	uint32_t srcClock_Hz,
	sctimer_counter_t whichCounter,
	uint32_t *periodEvent,
	uint32_t *pulseEvent
);
void startStepperPWM(sctimer_counter_t whichCounter);
void stopStepperPWM(sctimer_counter_t whichCounter);
void stepCompleteMotorXCallback(void);
void stepCompleteMotorYCallback(void);
// End Prototypes