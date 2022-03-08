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
 *		Added which_motor_t type. Added running bool to stepperMotor type
 */

// Includes
#include "pin_mux.h"
#include "fsl_sctimer.h"
// End Includes

// Definitions
#define SCTIMER_CLK_FREQ			CLOCK_GetFreq(kCLOCK_BusClk)
#define SCTIMER_MOTOR_X_OUT			kSCTIMER_Out_0
#define MAX_UP_COUNTER_VALUE		(0xFFFFFFFFU * 256U) // max 32 bit = 0xFFFFFFFF * up to 256 prescale
#define MAX_UPDOWN_COUNTER_VALUE	(0x1FFFFFFFFU * 256U) // max 32 bit * 2 (bi-directional) * up to 256 prescale
// End Definitions

// Structs
#ifndef STEPPERJOBSCTIMER
#define STEPPERJOBSCTIMER
typedef struct __stepper_job {
	uint32_t goalSteps;
	uint32_t currSteps;
} stepperJob_t;
#endif
#ifndef STEPPERCONFIGSCTIMER
#define STEPPERCONFIGSCTIMER
typedef struct __stepper_config {
	sctimer_event_active_direction_t activeDir; // direction of counter bi-directional counting
	uint32_t eventCounter;						// when a period finishes
	uint16_t matchValue;						// match for timer
	sctimer_counter_t timer;					// MOTOX or MOTORY or kSCTIMER_Counter_U or MOTORBOTH // kSCTIMER_Counter_U not used
	sctimer_out_t output;						// output pin (SCTIMER_MOTOR_X_OUT or SCTIMER_MOTOR_Y_OUT)

} stepperConfig_t;
#endif
#ifndef STEPPERMOTORSCTIMER
#define STEPPERMOTORSCTIMER
typedef struct __stepper_motor_sctimer {
	bool direction; // direction of stepper movement. tied to stepper motor controller
	stepperConfig_t config;
	stepperJob_t currentJob;
	void* periodCallback;
	bool running;
	int32_t netSteps; // before deactivating, this must be divisible by 16
	// as we've been micro-stepping this whole time
} stepperMotor_sctimer_t;
#endif
// End Structs

// Extern Variables
extern stepperMotor_sctimer_t motorX;
extern stepperMotor_sctimer_t* motorX_p;
// End Extern Variables

// Macro Functions
#define STEPPERX_driveStepperPWMBlocking(steps, freq) \
	STEPPERX_driveStepperPWM(steps, freq, true)

#define STEPPERX_driveStepperPWMNonBlocking(steps, freq) \
	STEPPERX_driveStepperPWM(steps, freq, false)
// End Macro Functions

// Prototypes
// management
status_t SCTIMER_Calculate32BitCounterConfig(
	uint64_t,
	uint8_t*,
	uint16_t*,
	sctimer_event_active_direction_t*
);
status_t STEPPERX_initializeStepperPWM(uint32_t, uint32_t);							// create config (including setting prescaler for large number of steps)
status_t STEPPERX_setupStepperPWM();				// stop motors, create events to give pwm behavior
void STEPPERX_startStepperPWM();							// start timer, set running
void STEPPERX_stopStepperPWM();							// halt timer, clear events, clear running
void STEPPERX_driveStepperPWM(uint32_t, uint32_t, bool);			// run init, setup and start

// callbacks
void STEPPERX_blockUntilMotorJobComplete();

// conversion
stepperMotor_sctimer_t* getMotorX();
// End Prototypes
