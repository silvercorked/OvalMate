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
 *	- 3/8/2022:
 *		Rewrote much of the steppers module in order to facilitate a new method of interacting with these timers. Progress towards this
 *		can be seen through the git commits
 *	- 3/9/2022:
 *		Went to lab to confirm and finalize stepper motor behavior. Acceleration looks good and moveTo and moveSteps are really useful from a
 *		coordinate perspective.
 *		Added in additional pins: direction, home, fault, enable, reset, and sleep for both motors. Changed pin writing function
 *		Added new pins to pin initialization function. Added fault pins as pin interrupts 5 and 6.
 *		Set direction GPIO in moveSteps function. Changed pinInfo_p member on stepperMotor_s to pinInfo_arr_p
 *		which holds an array of 7 pinInformation_s pointers
 *	- 3/13/2022:
 *		Fixed bug in relative move. Negative steps were pass incorrectly to further functions.
 *		Setup enable pins to active motor controller from microcontroller.
 *		Increased limit for no acceleration mode to a full revolution at 3200 steps.
 *		Added functions for setting pins.
 *		Swapped function names to be more acceptable of a module approach.
 *  - 3/14/2022:
 *		Swapped direction pin setting for stepperY. StepperY rotates opposite stepperX. So one of them needed to be inverted.
 *		This was done by handling the case in the STEPPER_writeDirectionPin function. As long as that function is used for setting steppers,
 *		The use can be safe to assume X and Y point in the same direction.
 *	- 3/18/2022:
 *		Added sleepMotor, wakeMotor, moveBothTo (and associated macro functions), and moveBothRelative (and associated macro functions)
 *	- 3/19/2022:
 *		Added comments to all functions in preparation for Software Design Review
 *	- 3/24/2022:
 *		Added mode switching and mode tracking for drawing from IR centered point
 *	- 3/29/2022:
 *		Corrected return types of mode switching functions
 */

#include "steppers.h"

// Variables
stepperMotorMode currentMode = IRMODE;
pinInformation_s stepperX_pwmOut =		{ .port = 0, .pin = 2  };
pinInformation_s stepperY_pwmOut =		{ .port = 0, .pin = 3  };

pinInformation_s stepperX_direction =	{ .port = 1, .pin = 6  };
pinInformation_s stepperX_home =		{ .port = 1, .pin = 4  };
pinInformation_s stepperX_fault =		{ .port = 1, .pin = 13 };
pinInformation_s stepperX_enable =		{ .port = 1, .pin = 20 };
pinInformation_s stepperX_reset =		{ .port = 0, .pin = 7  };
pinInformation_s stepperX_sleep =		{ .port = 1, .pin = 24 };

pinInformation_s stepperY_direction =	{ .port = 1, .pin = 30 };
pinInformation_s stepperY_home =		{ .port = 0, .pin = 1  };
pinInformation_s stepperY_fault =		{ .port = 1, .pin = 7  };
pinInformation_s stepperY_enable =		{ .port = 1, .pin = 1  };
pinInformation_s stepperY_reset =		{ .port = 1, .pin = 29 };
pinInformation_s stepperY_sleep =		{ .port = 1, .pin = 11 };

stepperMotorPhaseSteps_s stepperX_phaseSteps = {
	// .startSpeed = 0
	// .accelerating = 0
	// .steady = 0
	// .slowing = 0
};
stepperMotorPhaseSteps_s stepperY_phaseSteps = {
	// .startSpeed = 0
	// .accelerating = 0
	// .steady = 0
	// .slowing = 0
};
stepperMotor_s stepperX = {
	.timer_p = CTIMER0_X,
	.pinInfo_arr_p = {
		&stepperX_pwmOut,				// PWMOUT,
		&stepperX_direction,			// DIRECITON,
		&stepperX_enable,				// ENABLE,
		&stepperX_reset,				// RESET
		&stepperX_sleep,				// SLEEP
		&stepperX_home,					// HOME
		&stepperX_fault					// FAULT
	},
	.phaseSteps_p = &stepperX_phaseSteps,
	.matchCallback = STEPPERS_XTimerCallback,
	.output = CTIMER0_MATCH0_CHANNEL,
	.matchConfig = { },
	.status = { }, // defaults are false
	// .position = 0
	// .direction = false
};
stepperMotor_s stepperY = {
	.timer_p = CTIMER1_Y,
	.pinInfo_arr_p = {
		&stepperY_pwmOut,				// PWMOUT,
		&stepperY_direction,			// DIRECITON,
		&stepperY_enable,				// ENABLE,
		&stepperY_reset,				// RESET
		&stepperY_sleep,				// SLEEP
		&stepperY_home,					// HOME
		&stepperY_fault					// FAULT
	},
	.phaseSteps_p = &stepperY_phaseSteps,
	.matchCallback = STEPPERS_YTimerCallback,
	.output = CTIMER1_MATCH0_CHANNEL,
	.matchConfig = { },
	.status = { }
	// .position = 0
	// .direction = false
};
stepperMotor_s* stepperX_p = &stepperX;
stepperMotor_s* stepperY_p = &stepperY;
// End Variables

// Functions
/*
 * Initializes pins used by Stepper Motors. Pins set are as follows
 * 		port	pin		What's it used for?
 * 		0		2		Output, PWM to stepper motor controller for StepperX
 * 		0		3		Output, PWM to stepper motor controller for StepperY
 * 	StepperX
 * 		1		6		Output, Direction Pin, stepperX																						(low active)
 * 		1		4		Input,  Home Pin, If high, motor is at home position, ie (stepCount % 3200 = 0)										(low active)
 * 		1		13		Input,  Fault Pin, Pin Interrupt 5, Signals a electrical fault and triggers the interrupt handler STEPPERS_XFault	(low active)
 * 		1		20		Output, Enable Pin, CS- of motor controller																			(low active)
 * 		0		7		Output, Reset Pin, resets motor controller if raised high. Used when waking up motors								(low active)
 * 		1		24		Output, Sleep Pin, sleeps motor if dropped low. Motors are movable if low and use less power						(low active)
 * 	StepperY
 * 		1		30		Output, Direction Pin, stepperY																						(low active)
 * 		0		1		Input,  Home Pin, If high, motor is at home position, ie (stepCount % 3200 = 0)										(low active)
 * 		1		7		Input,  Fault Pin, Pin Interrupt 6, Signals a electrical fault and triggers the interrupt handler STEPPERS_XFault	(low active)
 * 		1		1		Output, Enable Pin, CS- of motor controller																			(low active)
 * 		1		29		Output, Reset Pin, resets motor controller if raised high. Used when waking up motors								(low active)
 * 		1		11		Output, Sleep Pin, sleeps motor if dropped low. Motors are movable if low and use less power						(low active)
 *
 * 		Output pins are initialized low.
 * 		Fault Pin interrupts trigger on falling edge.
 *
 * 	WANRING: uses pre-initialized PINT. Please call PINT_init, before running this.
 *
 * @params	- None
 * @return	- None
 */
void STEPPERS_initializePins() {
	gpio_pin_config_t gpioOUTConfigLOW = {
		.pinDirection = kGPIO_DigitalOutput,
		.outputLogic = 0
	};
	gpio_pin_config_t gpioOUTConfigHIGH = {
		.pinDirection = kGPIO_DigitalOutput,
		.outputLogic = 1
	};
	gpio_pin_config_t gpioINConfig = {
		.pinDirection = kGPIO_DigitalInput
	};
	// PWM outs
    GPIO_PinInit(GPIO, stepperX_pwmOut.port, stepperX_pwmOut.pin, &gpioOUTConfigLOW);			// start low
	GPIO_PinInit(GPIO, stepperY_pwmOut.port, stepperY_pwmOut.pin, &gpioOUTConfigLOW);			// start low

	// Stepper X out
	GPIO_PinInit(GPIO, stepperX_direction.port, stepperX_direction.pin, &gpioOUTConfigLOW);		// start low
	GPIO_PinInit(GPIO, stepperX_enable.port, stepperX_enable.pin, &gpioOUTConfigHIGH);			// active low
	GPIO_PinInit(GPIO, stepperX_reset.port, stepperX_reset.pin, &gpioOUTConfigHIGH);			// active low
	GPIO_PinInit(GPIO, stepperX_sleep.port, stepperX_sleep.pin, &gpioOUTConfigHIGH);			// active low

	// Stepper X in
	GPIO_PinInit(GPIO, stepperX_home.port, stepperX_home.pin, &gpioINConfig);					// inputs (active low)

	// didn't go high
	// Stepper Y out
	GPIO_PinInit(GPIO, stepperY_direction.port, stepperY_direction.pin, &gpioOUTConfigLOW);		// start low
	GPIO_PinInit(GPIO, stepperY_enable.port, stepperY_enable.pin, &gpioOUTConfigHIGH);			// active low
	GPIO_PinInit(GPIO, stepperY_reset.port, stepperY_reset.pin, &gpioOUTConfigHIGH);			// active low
	GPIO_PinInit(GPIO, stepperY_sleep.port, stepperY_sleep.pin, &gpioOUTConfigHIGH);			// active low

	// Stepper Y in
	GPIO_PinInit(GPIO, stepperY_home.port, stepperY_home.pin, &gpioINConfig);					// inputs (active low)

	/* Connect trigger sources to PINT */
	INPUTMUX_Init(INPUTMUX);
	INPUTMUX_AttachSignal(INPUTMUX, kPINT_PinInt5, PINT_PIN_INT5_SRC); // StepperX fault interrupt = pin interrupt 5
	INPUTMUX_AttachSignal(INPUTMUX, kPINT_PinInt6, PINT_PIN_INT6_SRC); // StepperX fault interrupt = pin interrupt 6
	/* Turnoff clock to inputmux to save power. Clock is only needed to make changes */
	INPUTMUX_Deinit(INPUTMUX);

	/* Setup Pin Interrupt 5 for falling edge */
	PINT_PinInterruptConfig(PINT, kPINT_PinInt5, kPINT_PinIntEnableFallEdge, STEPPERS_XFault);
	/* Enable callbacks for PINT5 by Index */
	PINT_EnableCallbackByIndex(PINT, kPINT_PinInt5);

	/* Setup Pin Interrupt 6 for falling edge */
	PINT_PinInterruptConfig(PINT, kPINT_PinInt6, kPINT_PinIntEnableFallEdge, STEPPERS_YFault);
	/* Enable callbacks for PINT6 by Index */
	PINT_EnableCallbackByIndex(PINT, kPINT_PinInt6);
}

/**
 * Initialize both X and Y stepper motors.
 * Creates a generic ctimer_config_t and removes it from memory.
 * Enables both motors (motors will begin consuming power and are no longer manually movable).
 *
 * Run this once before using motors. No need to call again.
 *
 * @params	- None
 * @return	- None
 */
void STEPPERS_initializeMotors() {
	ctimer_config_t config;					// can set prescale if need be (we don't need it for these ranges
	CTIMER_GetDefaultConfig(&config);		// different from MatchConfig
	CTIMER_Init(CTIMER0_X, &config);		// this config handles timer setup
	CTIMER_Init(CTIMER1_Y, &config);

	STEPPERS_writeOutputPin(stepperX_p->pinInfo_arr_p[ENABLE], 0U);
	STEPPERS_writeOutputPin(stepperY_p->pinInfo_arr_p[ENABLE], 0U);
}

/**
 * Sets up the initial state for the motors before any steps have taken place. Sets callback function to STEPPERS_{X or Y}TimerCallback.
 * Sets up match register of CTIMER associated with stepperMotor_s.
 *
 * @params	- stepperMotor_s* motor_p	: a pointer to a stepperMotor_s. This motor's ctimer will be setup by this function.
 * 			- uint32_t steps			: the number of steps on this journey. Whole numbers only. Direction should be handled outside of this function.
 * 			- bool accel				: true -> this trip can be sped up with acceleration, false -> this trip will not use acceleration.
 * @return	- status_t					: always kStatus_Success
 */
status_t STEPPERS_setupMotor(stepperMotor_s* motor_p, uint32_t steps, bool accel) {
	motor_p->matchConfig.enableCounterReset = true;					// reload timer when on match
	motor_p->matchConfig.enableCounterStop  = false;					// don't stop timer on match
	if (motor_p == stepperX_p)										// these are the same currently, but might as well check
		motor_p->matchConfig.matchValue     = STARTSTEPPERSTEPS0;
	else
		motor_p->matchConfig.matchValue     = STARTSTEPPERSTEPS1;
	motor_p->matchConfig.outControl         = kCTIMER_Output_Toggle;	// don't care, don't actually use output, just interrupt
	motor_p->matchConfig.outPinInitState    = false;					// don't care, don't actually use output, just interrupt
	motor_p->matchConfig.enableInterrupt    = true;					// since we use the interrupt, we want it on

	CTIMER_RegisterCallBack(motor_p->timer_p, &(motor_p->matchCallback), kCTIMER_SingleCallback);
	CTIMER_SetupMatch(motor_p->timer_p, motor_p->output, &(motor_p->matchConfig));

	STEPPERS_setMotorStepsPerPhase(motor_p->phaseSteps_p, steps, accel);	// presets all info about journey
	return kStatus_Success;
}

/**
 * Runs STEPPERS_setupMotor and STEPPERS_startMotor for the given motor.
 *
 * @params	- stepperMotor_s* motor_p	: a pointer to a stepperMotor_s. This motor will have its ctimer setup and will be started.
 * 			- uint32_t steps			: the number of steps this motor should travel.
 * 			- bool accel				: true -> this trip can be sped up with acceleration, false -> this trip will not use acceleration.
 * @return	- None
 */
void STEPPERS_driveSteps(stepperMotor_s* motor_p, uint32_t steps, bool accel) { // doesn't set direciton. moves in current direction
	STEPPERS_setupMotor(motor_p, steps, accel);
	STEPPERS_startMotor(motor_p);
}

/**
 * Setups up the Direction pin for given motor based on steps.
 *
 * @params	- stepperMotor_s* motor_p	: a pointer to a stepperMotor_s. This motor will have its direction pin set and ctimer setup and started.
 * 			- int32_t steps				: the number of steps this motor should travel. negative steps will set the direction pin low and be converted to positive
 * 			- bool accel				: true -> this trip can be sped up with acceleration, false -> this trip will not use acceleration.
 * 			- bool block				: true -> this function will block until the trip has ended, false -> this function will return as soon as it can.
 * @return	- status_t					: kStatus_Fail if steps = 0 (ie no need to turn on motors). kStatus_Success otherwise.
 */
status_t STEPPERS_moveRelative(stepperMotor_s* motor_p, int32_t steps, bool accel, bool block) { // move step amount relative to position
	if (steps == 0)
		return kStatus_Fail;
	if (steps > 0)
		motor_p->direction = true;
	else {
		steps *= -1;	// swap so steps is always a positive number before passing as int. 2's compliment + 1 addition is 2 clocks. Multiply is 1-2 clocks, so this is faster, https://developer.arm.com/documentation/ddi0337/e/BABBCJII
		motor_p->direction = false;	// if negative, dir = false
	}
	STEPPERS_writeDirectionPin(motor_p, motor_p->direction);
	STEPPERS_driveSteps(motor_p, (uint32_t) steps, accel);
	if (block)
		while (motor_p->status.running); // block until motor done
	return kStatus_Success;
}

/**
 * Drives motor to location based on grid given by home position of given motor.
 *
 * @params	- stepperMotor_s* motor_p	: a pointer to a stepperMotor_s. This motor will have its direction pin set and ctimer setup and started.
 * 			- int32_t steps				: the step coordinate this motor should travel to. negative steps will set the direction pin low and be converted to positive
 * 			- bool accel				: true -> this trip can be sped up with acceleration, false -> this trip will not use acceleration.
 * 			- bool block				: true -> this function will block until the trip has ended, false -> this function will return as soon as it can.
 * @return	- status_t					: kStatus_Fail if steps = 0 (ie no need to turn on motors). kStatus_Success otherwise.
 */
status_t STEPPERS_moveTo(stepperMotor_s* motor_p, uint32_t target, bool accel, bool block) {
	int32_t relativeSteps = target - motor_p->position;
	return STEPPERS_moveRelative(motor_p, relativeSteps, accel, block);
}

/**
 * Drives both motors relative to their current position.
 *
 * @params	- int32_t stepsX			: the number of steps stepperX should travel. negative steps will set the direction pin low and be converted to positive
 * 			- int32_t stepsY			: the number of steps stepperY should travel. negative steps will set the direction pin low and be converted to positive
 * 			- bool accel				: true -> this trip can be sped up with acceleration, false -> this trip will not use acceleration.
 * 			- bool block				: true -> this function will block until the trip has ended, false -> this function will return as soon as it can.
 * @return	- status_t					: kStatus_Fail if stepsX & stepsY == 0 (ie no need to turn on motors). kStatus_Success otherwise.
 */
status_t STEPPERS_moveBothRelative(int32_t stepsX, int32_t stepsY, bool accel, bool block) {
	if (stepsX == 0 && stepsY == 0)
		return kStatus_Fail;
	else if (stepsX == 0)
		return STEPPERS_moveRelative(stepperY_p, stepsY, accel, block);
	else if (stepsY == 0)
		return STEPPERS_moveRelative(stepperX_p, stepsX, accel, block);
	else {
		if (stepsX > 0)
			stepperX_p->direction = true;
		else {
			stepsX *= -1;	// swap so steps is always a positive number before passing as int
			stepperX_p->direction = false;	// if negative, dir = false
		}
		if (stepsY > 0)
			stepperY_p->direction = true;
		else {
			stepsY *= -1;	// swap so steps is always a positive number before passing as int
			stepperY_p->direction = false;	// if negative, dir = false
		}
		STEPPERS_writeDirectionPin(stepperX_p, stepperX_p->direction);
		STEPPERS_writeDirectionPin(stepperY_p, stepperY_p->direction);
		STEPPERS_driveSteps(stepperX_p, (uint32_t) stepsX, accel);
		STEPPERS_driveSteps(stepperY_p, (uint32_t) stepsY, accel);
		if (block)
			while (stepperX_p->status.running || stepperY_p->status.running); // block until motor done
		return kStatus_Success;
	}
}

/**
 * Drives both motors to location based on grid given by home position of each motor.
 *
 * @params	- int32_t targetX			: the step coordinate stepperX should travel to. negative steps will set the direction pin low and be converted to positive
 * 			- int32_t targetY			: the step coordinate stepperY should travel to. negative steps will set the direction pin low and be converted to positive
 * 			- bool accel				: true -> this trip can be sped up with acceleration, false -> this trip will not use acceleration.
 * 			- bool block				: true -> this function will block until the trip has ended, false -> this function will return as soon as it can.
 * @return	- status_t					: kStatus_Fail if stepsX & stepsY == 0 (ie no need to turn on motors). kStatus_Success otherwise.
 */
status_t STEPPERS_moveBothTo(uint32_t targetX, uint32_t targetY, bool accel, bool block) {
	int32_t relativeStepsX = targetX - stepperX_p->position;
	int32_t relativeStepsY = targetY - stepperY_p->position;
	return STEPPERS_moveBothRelative(relativeStepsX, relativeStepsY, accel, block);
}

/**
 * Setup the phaseSteps struct given based on the steps and whether acceleration should be a factor.
 *
 * @params	- stepperMotorPhaseSteps_s* phaseSteps_p	: a pointer to a stepperMotorPhaseSteps_s. It's values will be cleared and then set.
 * 			- uint32_t steps							: the number of steps on this journey.
 * 			- bool accel								: true -> this trip can be sped up with acceleration, false -> this trip will not use acceleration.
 * @return	- status_t									: always kStatus_Success.
 */
status_t STEPPERS_setMotorStepsPerPhase(stepperMotorPhaseSteps_s* phaseSteps_p, uint32_t steps, bool accel) {
	phaseSteps_p->accelValue = 0.0f;
	phaseSteps_p->accelerating = 0;
	phaseSteps_p->steady = 0;
	phaseSteps_p->slowing = 0;
	phaseSteps_p->startSpeed = 0;	// clear phaseSteps
	if (steps <= 1000 || !accel)	// 5/16 of a revolution = 1000 steps
		phaseSteps_p->startSpeed = steps;	// all steps are no accel at start speed
	else if (steps <= 3200) { // one full revolution = 3200 steps
		phaseSteps_p->accelerating = steps >> 2;				// 1/4 accel. Avoid expensive (2-12 clocks) division with 1 clock bit shift
		phaseSteps_p->slowing = phaseSteps_p->accelerating;		// 1/4 decel
		steps -= phaseSteps_p->accelerating << 1;				// left shift once == multiply by 2 in 1 clock
		phaseSteps_p->steady = steps;							// rest at max speed

		phaseSteps_p->accelValue = ((float) DIFFSTARTTOMAX0) / ((float) phaseSteps_p->accelerating); // avoid using floats till very end
	}
	else {
		phaseSteps_p->accelerating = steps >> 3;						// 1/8 accel. Avoid expensive (2-12 clocks) division with 1 clock bit shift
		phaseSteps_p->slowing = phaseSteps_p->accelerating;				// 1/8 decel
		steps -= phaseSteps_p->accelerating << 1;						// subtracting accelerating and slowing steps from total steps
		phaseSteps_p->steady = steps;									// rest at max speed

		phaseSteps_p->accelValue = ((float) DIFFSTARTTOMAX0) / ((float) phaseSteps_p->accelerating); // accelValue is "amount of change in match value per period"
	}
	return kStatus_Success;
}

/**
 * Starts the given motor's ctimer (begins driving the motor) and set it's initial status.
 *
 * @params	- stepperMotor_s* motor_p	: a pointer to a stepperMotor_s. Its status will be changed and its timer will start.
 * @return	- None
 */
void STEPPERS_startMotor(stepperMotor_s* motor_p) {
	if (motor_p->phaseSteps_p->accelerating != 0)	// all drives must start with accel or startSpeed.
		motor_p->status.accelerating = true;		// if accel steps assigned, then start on accel
	else											// otherwise start startSpeed, if error, then
		motor_p->status.startSpeed = true;			// will produce 1 step (still tracked) and stop following startSpeed interrupt
	CTIMER_StartTimer(motor_p->timer_p);
	motor_p->status.running = true;
}

/**
 * Stops the given motor's ctimer (stops driving the motor) and reset it's status.
 *
 * @params	- stepperMotor_s* motor_p	: a pointer to a stepperMotor_s. Its status will be changed and its timer will stop.
 * @return	- None
 */
void STEPPERS_stopMotor(stepperMotor_s* motor_p) {
	CTIMER_StopTimer(motor_p->timer_p);
	motor_p->status.running = false;
	motor_p->status.startSpeed = false;
	motor_p->status.accelerating = false;
	motor_p->status.steady = false;
	motor_p->status.slowing = false;
}

/**
 * Sets the position of the motor to 0 (ie home = position at 0).
 *
 * @params	- stepperMotor_s* motor_p	: a pointer to a stepperMotor_s. Its status will be changed and its timer will start.
 * @return	- None
 */
void STEPPERS_setHome(stepperMotor_s* motor_p) {
	motor_p->position = 0;
}

stepperMotorMode STEPPERS_switchMode() {
	if (currentMode == PENMODE) {
		STEPPERS_moveBothRelativeAccel(PENIROFFSETX, PENIROFFSETY); // offset for pen
		currentMode = IRMODE;
	}
	else { // IRMODE
		STEPPERS_moveBothRelativeAccel(-PENIROFFSETX, -PENIROFFSETY);
		currentMode = PENMODE;
	}
	return currentMode;
}
stepperMotorMode STEPPERS_getCurrentMode() {
	return currentMode;
}
stepperMotorMode STEPPERS_setCurrentMode(stepperMotorMode mode) {
	currentMode = mode;
}

/**
 * Callback for the X stepperMotor. This is called once per PULSE (ie twice per period). Developers should not be calling this function.
 * To set a custom function, set the stepperMotor_s*->matchCallback to a function of type void (*)(uint32_t).
 *
 * @params	- uint32_t flags	: The ctimer passes these flags based on the interrupt conditions.
 * @return	- None
 */
void STEPPERS_XTimerCallback(uint32_t flags) {
	static bool periodOffsetX = false; // need to skip 1 interrupt per 2 to make pwm
	if (periodOffsetX)
		STEPPERS_generalTimerCallback(flags, stepperX_p);
	else
		STEPPERS_writeOutputPin(stepperX_p->pinInfo_arr_p[PWMOUT], 1U); // write high
	periodOffsetX = !periodOffsetX;
}

/**
 * Callback for the Y stepperMotor. This is called once per PULSE (ie twice per period). Developers should not be calling this function.
 * To set a custom function, set the stepperMotor_s*->matchCallback to a function of type void (*)(uint32_t).
 *
 * @params	- uint32_t flags	: The ctimer passes these flags based on the interrupt conditions.
 * @return	- None
 */
void STEPPERS_YTimerCallback(uint32_t flags) {
	static bool periodOffsetY = false; // need to skip 1 interrupt per 2 to make pwm
	if (periodOffsetY)
		STEPPERS_generalTimerCallback(flags, stepperY_p);
	else
		STEPPERS_writeOutputPin(stepperY_p->pinInfo_arr_p[PWMOUT], 1U); // write high
	periodOffsetY = !periodOffsetY;
}

/**
 * This function is a common resultant callback of the STEPPERS_{X or Y}TimerCallback functions.
 * If the status of the stepperMotor_s is accelerating, steady, or slowing, acceleration is being implemented on the trip.
 * Otherwise, the motor is in noAccel mode, which means startSpeed is true.
 * stepperMotor_s's position is updated and 1 step is decremented from the total journey (accelerating + steady + slowing or startSpeed).
 * Handles both noAccel and accelerating case of driving motors.
 *
 * Developers should not be calling this function.
 *
 * @params	- uint32_t flags			: The ctimer passes these flags based on the interrupt conditions.
 * 			- stepperMotor_s* motor_p	: a pointer to a stepperMotor_s. This motor will have it's status and phaseSteps altered.
 * @return	- None
 */
void STEPPERS_generalTimerCallback(uint32_t flags, stepperMotor_s* motor_p) {
	if (motor_p->status.accelerating) {
		motor_p->position += motor_p->direction ? 1 : -1;
		motor_p->phaseSteps_p->accelerating--;
		motor_p->phaseSteps_p->roundedAccel += motor_p->phaseSteps_p->accelValue; // use roundedAccel as motor specific measure of cumulative match value modification.
		if (motor_p->phaseSteps_p->roundedAccel > 1) { // needed for accel rates < 1 and to handle decimal accel rates. Since accelValue is < 1 when accelerating steps > DIFFMAXSTARTFREQ
			motor_p->matchConfig.matchValue -= (uint32_t) motor_p->phaseSteps_p->roundedAccel;	// decrease matchValue by whole integers
			motor_p->phaseSteps_p->roundedAccel -= (uint32_t) motor_p->phaseSteps_p->roundedAccel; // remove amount applied
		}
		if (motor_p->phaseSteps_p->accelerating == 0) {
			motor_p->status.accelerating = false;
			if (motor_p->phaseSteps_p->steady != 0)
				motor_p->status.steady = true;
			else if (motor_p->phaseSteps_p->slowing != 0)
				motor_p->status.slowing = true;
			else
				STEPPERS_stopMotor(motor_p);
		}
		CTIMER_SetupMatch(motor_p->timer_p, motor_p->output, &(motor_p->matchConfig));
	}
	else if (motor_p->status.steady) { // no acceleration, same as startSpeed but with different conditionals
		motor_p->position += motor_p->direction ? 1 : -1;
		motor_p->phaseSteps_p->steady--;
		if (motor_p->phaseSteps_p->steady == 0) {
			motor_p->status.steady = false;
			if (motor_p->phaseSteps_p->slowing != 0)
				motor_p->status.slowing = true;
			else
				STEPPERS_stopMotor(motor_p);
		}
	}
	else if (motor_p->status.slowing) {
		motor_p->position += motor_p->direction ? 1 : -1;
		motor_p->phaseSteps_p->slowing--;
		motor_p->phaseSteps_p->roundedAccel += motor_p->phaseSteps_p->accelValue;
		if (motor_p->phaseSteps_p->roundedAccel > 1) { // needed for accel rates < 1 and to handle decimal accel rates
			motor_p->matchConfig.matchValue += (uint32_t) motor_p->phaseSteps_p->roundedAccel;
			motor_p->phaseSteps_p->roundedAccel -= (uint32_t) motor_p->phaseSteps_p->roundedAccel; // remove amount applied
		}
		if (motor_p->phaseSteps_p->slowing == 0) {
			motor_p->status.slowing = false;
			STEPPERS_stopMotor(motor_p);
		}
		CTIMER_SetupMatch(motor_p->timer_p, motor_p->output, &(motor_p->matchConfig));
	}
	else if (motor_p->status.startSpeed) { // lowest freq requirements so can be last in conditionals w/ least consequence
		motor_p->position += motor_p->direction ? 1 : -1;
		motor_p->phaseSteps_p->startSpeed--;
		if (motor_p->phaseSteps_p->startSpeed == 0) {
			motor_p->status.startSpeed = false;
			STEPPERS_stopMotor(motor_p);
		}
	}
	else
		STEPPERS_stopMotor(motor_p); // failsafe. If somehow the bools get messed up and one of them isn't one, stop motor
	STEPPERS_writeOutputPin(motor_p->pinInfo_arr_p[PWMOUT], 0U); // clear
	// clearing here means our PWM period starts with a cleared pulse and then a set pulse. This guarentees the motors always clear when done.
}

/**
 * This function is a callback for Pin Interrupt 5 (stepperX's fault pin). Developers should not call this function.
 * This function stops the motor and prints to the debug terminal.
 *
 * @params	- pint_pin_int_t pintr			: an identifier for which pin interrupt is triggering this function call. (always kPINT_PinInt5)
 * 			- uint32_t pmatch_status		: I actually don't know what this is. Should probably print it out and try to figure it out.
 * @return	- None
 */
void STEPPERS_XFault(pint_pin_int_t pintr, uint32_t pmatch_status) {
	PRINTF("\r\n fault pin x raised");
	if (debounceFault(&stepperX_fault)) {
		PRINTF("\r\n motor fault on stepper X detected!!! stopping motor...");
		STEPPERS_stopMotor(stepperX_p);
	}
}

/**
 * This function is a callback for Pin Interrupt 6 (stepperY's fault pin). Developers should not call this function.
 * This function stops the motor and prints to the debug terminal.
 *
 * @params	- pint_pin_int_t pintr			: an identifier for which pin interrupt is triggering this function call. (always kPINT_PinInt6)
 * 			- uint32_t pmatch_status		: I actually don't know what this is. Should probably print it out and try to figure it out.
 * @return	- None
 */
void STEPPERS_YFault(pint_pin_int_t pintr, uint32_t pmatch_status) {
	PRINTF("\r\n fault pin y raised");
	if (debounceFault(&stepperY_fault)) {
		PRINTF("\r\n motor fault on stepper Y detected!!! stopping motor...");
		STEPPERS_stopMotor(stepperY_p);
	}
}

/**
 * Disables the motor and sleeps it. Some small adjustments may be made to motor position before sleeping,
 * as any position that is a microstep will be lost if awoken.
 *
 * @params	- stepperMotor_s* motor_p	: a pointer to a stepperMotor_s. This motor will be disabled and slept.
 * @return	- None
 */
void STEPPERS_sleepMotor(stepperMotor_s* motor_p) {
	uint8_t microStepCount = motor_p->position % 16;
	if (microStepCount != 0)
		STEPPERS_moveRelative(motor_p, -(microStepCount), false, true);
	STEPPERS_writeEnablePin(motor_p, true);	// disable motors
	delay1ms();
	STEPPERS_writeSleepPin(motor_p, false);	// set sleep
	delay1ms();
}

/**
 * Unsleep, reset, and enable the given motor.
 *
 * @params	- stepperMotor_s* motor_p	: a pointer to a stepperMotor_s. This motor will be unslept, reset, and enabled.
 * @return	- None
 */
void STEPPERS_wakeMotor(stepperMotor_s* motor_p) {
	STEPPERS_writeResetPin(motor_p, false);		// reset motors
	delay1ms();									// delay to allow stepperMotorController time to adjust.
	STEPPERS_writeSleepPin(motor_p, true);		// wake motors
	delay1ms();									// not sure how long these delays need to be.
	STEPPERS_writeResetPin(motor_p, true);		// end reset
	delay1ms();									// for now, 1ms = 1 period of 1kHz PWM, so that seems like a good amount
	STEPPERS_writeEnablePin(motor_p, false);	// enable motors
	delay1ms();
}

/**
 * Write to output pin of given pinInformation_s. This should only be used for output pins.
 * Developers should use the helper functions below rather than this function,
 * as this would require knowing the position of elements in the pinInfo_arr_p array.
 *
 * @params	- pinInformation_s* pinInfo_p	: a pointer to a pinInformation_s. This pin's output will be set based on highLow.
 * 			- bool highLow					: true -> write high (1U), false -> write low (0U)
 * @return	- None
 */
void STEPPERS_writeOutputPin(pinInformation_s* pinInfo_p, bool highLow) {
	GPIO_PinWrite(GPIO, pinInfo_p->port, pinInfo_p->pin, highLow);
}

/**
 * Read pin's current value. This can be used on both input and output pins.
 * Developers should use the helper functions below rather than this function,
 * as this would require knowing the position of elements in the pinInfo_arr_p array.
 *
 * @params	- pinInformation_s* pinInfo_p	: a pointer to a pinInformation_s. This pin will be read.
 * @return	- None
 */
bool STEPPERS_readInputPin(pinInformation_s* pinInfo_p) {
	return GPIO_PinRead(GPIO, pinInfo_p->port, pinInfo_p->pin);
}

/**
 * Write to a motor's direction pin.
 *
 * @params	- stepperMotor_s* motor_p	: a pointer to a stepperMotor_s. The direction pin of this motor will be set based on highLow.
 * 			- bool highLow				: true -> write high (1U), false -> write low (0U)
 * @return	- None
 */
void STEPPERS_writeDirectionPin(stepperMotor_s* motor_p, bool highLow) {
	if (motor_p == stepperY_p) // stepperY rotates opposite of StepperX.
		//This makes their "directions", the same in code, but opposite in hardware,
		// as long as this function is the how the direction is changed
		STEPPERS_writeOutputPin(motor_p->pinInfo_arr_p[DIRECTION], !highLow);
	else
		STEPPERS_writeOutputPin(motor_p->pinInfo_arr_p[DIRECTION], highLow);
}

/**
 * Write to a motor's enable pin.
 *
 * @params	- stepperMotor_s* motor_p	: a pointer to a stepperMotor_s. The enable pin of this motor will be set based on highLow.
 * 			- bool highLow				: true -> write high (1U), false -> write low (0U)
 * @return	- None
 */
void STEPPERS_writeEnablePin(stepperMotor_s* motor_p, bool highLow) {
	STEPPERS_writeOutputPin(motor_p->pinInfo_arr_p[ENABLE], highLow);
}

/**
 * Write to a motor's reset pin.
 *
 * @params	- stepperMotor_s* motor_p	: a pointer to a stepperMotor_s. The reset pin of this motor will be set based on highLow.
 * 			- bool highLow				: true -> write high (1U), false -> write low (0U)
 * @return	- None
 */
void STEPPERS_writeResetPin(stepperMotor_s* motor_p, bool highLow) {
	STEPPERS_writeOutputPin(motor_p->pinInfo_arr_p[RESET], highLow);
}

/**
 * Write to a motor's sleep pin.
 *
 * @params	- stepperMotor_s* motor_p	: a pointer to a stepperMotor_s. The sleep pin of this motor will be set based on highLow.
 * 			- bool highLow				: true -> write high (1U), false -> write low (0U)
 * @return	- None
 */
void STEPPERS_writeSleepPin(stepperMotor_s* motor_p, bool highLow) {
	STEPPERS_writeOutputPin(motor_p->pinInfo_arr_p[SLEEP], highLow);
}

/**
 * Read a motor's home pin.
 *
 * @params	- stepperMotor_s* motor_p	: a pointer to a stepperMotor_s. The home pin of this motor will be read.
 * @return	- None
 */
bool STEPPERS_readHomePin(stepperMotor_s* motor_p) {
	return STEPPERS_readInputPin(motor_p->pinInfo_arr_p[HOME]);
}

bool debounceFault(pinInformation_s* pinInfo) {
	bool r = GPIO_PinRead(GPIO, pinInfo->port, pinInfo->pin);
	if (r) {
		delay500us();
		return GPIO_PinRead(GPIO, pinInfo->port, pinInfo->pin);
	}
	return r; // r guaranteed to be false
}
// End Functions
