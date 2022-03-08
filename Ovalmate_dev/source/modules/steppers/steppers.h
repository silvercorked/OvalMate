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
#include "sctimerStepperX.h"
#include "ctimerStepperY.h"
// End Includes

// Definitions

// End Definitions

// Enums
typedef enum __which_motor {
	MOTORX = 's',
	MOTORY = 'c',
	MOTORBOTH = 'a'
} whichMotor;
// End Enums

// Extern Variables
extern void (*stepperMotorXCallback[4])(whichMotor); // callback arr[4]
extern void (*stepperMotorYCallback[4])(whichMotor); // callback arr[4]
// End Extern Variables

// Macro Functions
#define driveStepperPWMBlocking(whichMotor, steps, freq) \
	driveStepperPWM(whichMotor, steps, freq, true)

#define driveStepperPWMNonBlocking(whichMotor, steps, freq) \
	driveStepperPWM(whichMotor, steps, freq, false)
// End Macro Functions

// Prototypes
// management
void initializeSteppers();										// call to set callback funcs
status_t prepareStepperPWM(whichMotor, uint32_t, uint32_t);		// run init and setup
void driveStepperPWM(whichMotor, uint32_t, uint32_t, bool);		// run prep and start
void startStepperPWM(whichMotor);								// start timer, set running
void stopStepperPWM(whichMotor);								// halt timer, clear events, clear running
void STEPPERX_completeStepStepperMotorXCallback();
void STEPPERY_completeStepStepperMotorYCallback(uint32_t);

// callbacks
void completeStepStepperMotorXCallback(void);
void completeStepStepperMotorYCallback(void);
void blockUntilMotorJobComplete(whichMotor);
uint8_t getMotorCallbackLength(whichMotor);
status_t addMotorCallback(whichMotor, void (*f)(whichMotor));
status_t removeMotorCallback(whichMotor, uint8_t);
status_t clearMotorCallbacks(whichMotor);

// conversion
void* getMotor(whichMotor); // can't have types cause types conflict
whichMotor getWhich(void*); // can't have types cause types conflict
// End Prototypes
