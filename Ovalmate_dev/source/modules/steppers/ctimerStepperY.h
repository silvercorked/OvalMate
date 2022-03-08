// Includes
#include <stdbool.h>
#include "fsl_ctimer.h"
// End Includes

// Definitions
#define CTIMER_MOTOR_Y_OUT     	CTIMER0         /* Timer 2 */
#define CTIMER_MAT_OUT_Y	kCTIMER_Match_1
#define CTIMER_CLK_FREQ_Y	CLOCK_GetCTimerClkFreq(0U)
#define DUTYCYCLE			50U
#ifndef CTIMER_MAT_PWM_PERIOD_CHANNEL
#define CTIMER_MAT_PWM_PERIOD_CHANNEL kCTIMER_Match_3 // this probably uses another channel for the period and triggers off that as an interrupt
#endif
// End Definitions

// Macro Functions
#define STEPPERY_driveStepperPWMBlocking(steps, freq) \
	STEPPERY_driveStepperPWM(steps, freq, true)

#define STEPPERY_driveStepperPWMNonBlocking(steps, freq) \
	STEPPERY_driveStepperPWM(steps, freq, false)
// End Macro Functions

// Structs
#ifndef STEPPERJOBSCTIMER
#define STEPPERJOBSCTIMER
typedef struct __stepper_job {
	uint32_t goalSteps;
	uint32_t currSteps;
} stepperJob_t;
#endif
#ifndef STEPPERMOTORCTIMER
#define STEPPERMOTORCTIMER
typedef struct __stepper_motor_ctimer {
	volatile uint32_t pwmPeriod;
	volatile uint32_t pulsePeriod;
	bool direction;
	stepperJob_t currentJob;
	uint32_t timerFreqPrescaled;
	bool running;
	ctimer_config_t ctimerConfig;
	ctimer_match_t matchOutput;
	CTIMER_Type* ctimer_p;
	ctimer_callback_t periodCallback;
	int32_t netSteps; // before deactivating, this must be divisible by 16
	// as we've been micro-stepping this whole time
} stepperMotor_ctimer_t;
#endif

// Extern Variables
extern stepperMotor_ctimer_t* motorY_p;
// End Extern Variables

// Prototypes
void STEPPERY_initializeStepperPWM();
void STEPPERY_setupStepperPWM(uint32_t, uint32_t);
status_t CTIMER_setPWMPeriodValue(uint32_t, uint8_t);
void STEPPERY_startStepperPWM();
void STEPPERY_stopStepperPWM();

// callbacks
void STEPPERY_blockUntilMotorJobComplete();

// conversion
stepperMotor_ctimer_t* getMotorY();
// End Prototypes
