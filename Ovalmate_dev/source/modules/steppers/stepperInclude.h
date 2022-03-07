
#ifndef STEPPERPREINCLUDES
#define STEPPERPREINCLUDES

typedef enum _which_motor {
	MOTORX = 'x',
	MOTORY = 'y',
	MOTORBOTH = 'a'
} which_motor_t;

typedef struct __stepper_job {
	uint32_t goalSteps;
	uint32_t currSteps;
} stepperJob;

#endif

