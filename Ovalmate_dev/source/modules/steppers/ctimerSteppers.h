
// Includes
#include <stdbool.h>
#include "fsl_ctimer.h"
#include "stepperInclude.h"
// End Includes

// Definitions
#define X_OUT_TIMER			CTIMER2
#define Y_OUT_TIMER     	CTIMER0         /* Timer 2 */
#define CTIMER_MAT_OUT_X  	kCTIMER_Match_3 /* Match output 0 */ // both x and y use match 2
#define CTIMER_MAT_OUT_Y	kCTIMER_Match_1
#define CTIMER_CLK_FREQ_X	CLOCK_GetCTimerClkFreq(0U) // 1U for clock 1 // all ctimers use same freq but good to grab just in case
#define CTIMER_CLK_FREQ_Y	CLOCK_GetCTimerClkFreq(2U)
#define DUTYCYCLE			50U

#define SPEED_L1			1000U
#define SPEED_L2			1500U
#define SPEED_MAIN			2000U

#ifndef CTIMER_MAT_PWM_PERIOD_CHANNEL
#define CTIMER_MAT_PWM_PERIOD_CHANNEL kCTIMER_Match_2 // this probably uses another channel for the period and triggers off that as an interrupt
#endif // both using this is fine because they are different ctimers. otherwise these would collide and overwrite
// End Definitions

typedef struct __stepper_motor {
	volatile uint32_t pwmPeriod;
	volatile uint32_t pulsePeriod;
	bool direction;
	stepperJob currentJob;
	uint32_t timerFreqPrescaled;
	bool running;
	ctimer_config_t ctimerConfig;
	ctimer_match_t matchOutput;
	CTIMER_Type* ctimer_p;
	ctimer_callback_t periodCallback;
	int32_t netSteps; // before deactivating, this must be divisible by 16
	// as we've been micro-stepping this whole time
} stepperMotor_ctimer;

// Extern Variables
// Variables
extern stepperMotor_ctimer* motorX_p;
extern stepperMotor_ctimer* motorY_p;
// End Variables
// End Extern Variables

// Prototypes
stepperMotor_ctimer* getMotor(which_motor_t);
which_motor_t whichMotor(stepperMotor_ctimer*);
void startStepperPWM(stepperMotor_ctimer*);
void stopStepperPWM(stepperMotor_ctimer*);
void setupStepperPWM(which_motor_t, uint32_t, uint32_t);
status_t CTIMER_setPWMPeriodValue(stepperMotor_ctimer*, uint32_t, uint8_t);
void ctimerXCallback(uint32_t);
void ctimerYCallback(uint32_t);
void ctimerGeneralCallback(uint32_t, which_motor_t);
void blockUntilMotorJobComplete(which_motor_t);
void initializeStepperPWM();
// End Prototypes
