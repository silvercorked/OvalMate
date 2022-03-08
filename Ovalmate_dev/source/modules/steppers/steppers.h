

// Includes
#include "microsecondTimer.h"
#include "fsl_gpio.h"
// End Includes

// Definitions

// End Definitions

// Structs
#ifndef PININFORMATION
#define PININFORMATION
typedef struct __pin_information {
	uint8_t port;
	uint8_t pin;
} pinInformation_t;
#endif
typedef struct __stepper_motor {
	uint32_t position;
	uint32_t target;
	float currSpeed;
	float maxSpeed;
	float acceleration;
	uint32_t usInterval;
	uint32_t lastUsCount;
	pinInformation_t pinInfo;
} stepperMotor_t;
// End Structs

// Extern Variables
extern stepperMotor* motorX_p;
extern stepperMotor* motorY_p;
// End Extern Variables

// Enums
typedef enum __which_Motor { // using the pointers as enum vals keeps them small and allows casting for conversion
	MOTORX = motorX_p,
	MOTORY = motorY_p
} whichMotor;
// End Enums

// Prototypes
void initializeStepperPins();
void clearTarget(whichMotor, uint32_t); // set target to coord & clear pin
void setPosition(whichMotor, uint32_t);
uint32_t distanceFromTarget(whichMotor);
void computeNewSpeed(whichMotor);
status_t checkForStep(whichMotor);
void step(whichMotor);
status_t setMaxSpeed(whichMotor, float);
status_t setAcceleration(whichMotor, float);
status_t setSpeed(whichMotor, float);
status_t setPin(whichMotor);
status_t clearPin(whichMotor);
status_t togglePin(whichMotor);
// End Prototypes
