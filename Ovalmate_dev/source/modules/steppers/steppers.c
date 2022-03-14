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
 *		Set direction GPIO in moveSteps function. Chanded pinInfo_p member on stepperMotor_s to pinInfo_arr_p
 *		which holds an array of 7 pinInformation_s pointers
 */

#include "steppers.h"

// Variables
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
	.matchCallback = stepperXTimerCallback,
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
	.matchCallback = stepperYTimerCallback,
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
void initializeStepperPins() {
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
	GPIO_PinInit(GPIO, stepperX_home.port, stepperX_reset.pin, &gpioINConfig);					// inputs (active low)

	// Stepper Y out
	GPIO_PinInit(GPIO, stepperY_direction.port, stepperX_direction.pin, &gpioOUTConfigLOW);		// start low
	GPIO_PinInit(GPIO, stepperY_enable.port, stepperX_enable.pin, &gpioOUTConfigHIGH);			// active low
	GPIO_PinInit(GPIO, stepperY_reset.port, stepperX_reset.pin, &gpioOUTConfigHIGH);			// active low
	GPIO_PinInit(GPIO, stepperY_sleep.port, stepperX_sleep.pin, &gpioOUTConfigHIGH);			// active low

	// Stepper Y in
	GPIO_PinInit(GPIO, stepperY_home.port, stepperX_reset.pin, &gpioINConfig);					// inputs (active low)

	/* Connect trigger sources to PINT */
	INPUTMUX_Init(INPUTMUX);
	INPUTMUX_AttachSignal(INPUTMUX, kPINT_PinInt5, PINT_PIN_INT5_SRC); // StepperX fault interrupt = pin interrupt 5
	INPUTMUX_AttachSignal(INPUTMUX, kPINT_PinInt6, PINT_PIN_INT6_SRC); // StepperX fault interrupt = pin interrupt 6
	/* Turnoff clock to inputmux to save power. Clock is only needed to make changes */
	INPUTMUX_Deinit(INPUTMUX);

	//PRINTF("\f\r\nPINT Pin interrupt example\r\n");

	/* Initialize PINT */
	#ifndef PINTINITIALIZED	// this init func appears to reset previous items put upon it,
	#define	PINTINITIALIZED	// should only need to initialize it once. use definition safeguard for pragma once
	PINT_Init(PINT);
	#endif

	/* Setup Pin Interrupt 5 for falling edge */
	PINT_PinInterruptConfig(PINT, kPINT_PinInt5, kPINT_PinIntEnableFallEdge, stepperXFault);
	/* Enable callbacks for PINT5 by Index */
	PINT_EnableCallbackByIndex(PINT, kPINT_PinInt5);

	/* Setup Pin Interrupt 6 for falling edge */
	PINT_PinInterruptConfig(PINT, kPINT_PinInt6, kPINT_PinIntEnableFallEdge, stepperYFault);
	/* Enable callbacks for PINT6 by Index */
	PINT_EnableCallbackByIndex(PINT, kPINT_PinInt6);
}

void initializeStepperMotors() {
	ctimer_config_t config;					// can set prescale if need be (we don't need it for these ranges
	CTIMER_GetDefaultConfig(&config);		// different from MatchConfig
	CTIMER_Init(CTIMER0_X, &config);		// this config handles timer setup
	CTIMER_Init(CTIMER1_Y, &config);

	STEPPERS_writeOutputPin(stepperX_p->pinInfo_arr_p[ENABLE], 0U);
	STEPPERS_writeOutputPin(stepperX_p->pinInfo_arr_p[ENABLE], 0U);
}

status_t setupStepperMotor(stepperMotor_s* motor_p, uint32_t steps) {
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

	setMotorStepsPerPhase(motor_p->phaseSteps_p, steps);	// presets all info about journey
	return kStatus_Success;
}

void driveStepperSteps(stepperMotor_s* motor_p, uint32_t steps) { // doesn't set direciton. moves in current direction
	setupStepperMotor(motor_p, steps);
	startMotor(motor_p);
}

status_t moveSteps(stepperMotor_s* motor_p, int32_t steps) { // move step amount relative to position
	if (steps == 0)
		return kStatus_Fail;
	if (steps > 0)
		motor_p->direction = true;
	else {
		steps *= -1;	// swap so steps is always a positive number before passing as int
		motor_p->direction = false;	// if negative, dir = false
	}
	STEPPERS_writeOutputPin(motor_p->pinInfo_arr_p[DIRECITON], motor_p->direction);
	driveStepperSteps(motor_p, (int32_t) steps);
	return kStatus_Success;
}

status_t moveTo(stepperMotor_s* motor_p, uint32_t target) {
	int32_t relativeSteps = target - motor_p->position;
	return moveSteps(motor_p, relativeSteps);
}

status_t setMotorStepsPerPhase(stepperMotorPhaseSteps_s* phaseSteps_p, uint32_t steps) {
	phaseSteps_p->accelValue = 0.0f;
	phaseSteps_p->accelerating = 0;
	phaseSteps_p->steady = 0;
	phaseSteps_p->slowing = 0;
	phaseSteps_p->startSpeed = 0;	// clear phaseSteps
	if (steps <= 3200)	// one full revolution
		phaseSteps_p->startSpeed = steps;	// all steps are no accel at start speed
	else if (steps <= 9600) { // 3 full revolutions
		phaseSteps_p->accelerating = steps / 4;					// 1/4 accel
		phaseSteps_p->slowing = phaseSteps_p->accelerating;		// 1/4 decel
		steps -= phaseSteps_p->accelerating << 1;				// left shift once == multiply by 2 in 1 clock
		phaseSteps_p->steady = steps;							// rest at max speed

		phaseSteps_p->accelValue = ((float) DIFFSTARTTOMAX0) / ((float) phaseSteps_p->accelerating);
	}
	else {
		phaseSteps_p->accelerating = steps / 10;						// 1/10 accel
		phaseSteps_p->slowing = phaseSteps_p->accelerating;				// 1/10 decel
		steps -= phaseSteps_p->accelerating + phaseSteps_p->slowing;	// addition faster when not by power of 2
		phaseSteps_p->steady = steps;									// rest at max speed

		phaseSteps_p->accelValue = ((float) DIFFSTARTTOMAX0) / ((float) phaseSteps_p->accelerating);
	}
	return kStatus_Success;
}



void startMotor(stepperMotor_s* motor_p) {
	if (motor_p->phaseSteps_p->accelerating != 0)	// all drives must start with accel or startSpeed.
		motor_p->status.accelerating = true;		// if accel steps assigned, then start on accel
	else											// otherwise start startSpeed, if error, then
		motor_p->status.startSpeed = true;			// will produce 1 step (still tracked) and stop following startSpeed interrupt
	CTIMER_StartTimer(motor_p->timer_p);
	motor_p->status.running = true;
}
void stopMotor(stepperMotor_s* motor_p) {
	CTIMER_StopTimer(motor_p->timer_p);
	motor_p->status.running = false;
	motor_p->status.startSpeed = false;
	motor_p->status.accelerating = false;
	motor_p->status.steady = false;
	motor_p->status.slowing = false;
}

void setHome(stepperMotor_s* motor_p) {
	motor_p->position = 0;
}

void stepperXTimerCallback(uint32_t flags) {
	static bool periodOffsetX = false; // need to skip 1 interrupt per 2 to make pwm
	if (periodOffsetX)
		stepperGeneralTimerCallback(flags, stepperX_p);
	else
		STEPPERS_writeOutputPin(stepperX_p->pinInfo_arr_p[PWMOUT], 1U); // write high
	periodOffsetX = !periodOffsetX;
}
void stepperYTimerCallback(uint32_t flags) {
	static bool periodOffsetY = false; // need to skip 1 interrupt per 2 to make pwm
	if (periodOffsetY)
		stepperGeneralTimerCallback(flags, stepperY_p);
	else
		STEPPERS_writeOutputPin(stepperY_p->pinInfo_arr_p[PWMOUT], 1U); // write high
	periodOffsetY = !periodOffsetY;
}
/**
 * designed with 2 modes in mind, either staying to one freq or
 * following a pattern of accelerating (+ velocity), sitting at max speed, and accelerating (- velocity)
 */
void stepperGeneralTimerCallback(uint32_t flags, stepperMotor_s* motor_p) {
	// handle based on bools, send output, update bools
	//static uint32_t count = 0;
	//count++;
	if (motor_p->status.accelerating) {
		motor_p->position += motor_p->direction ? 1 : -1;
		motor_p->phaseSteps_p->accelerating--;
		motor_p->phaseSteps_p->roundedAccel += motor_p->phaseSteps_p->accelValue;
		if (motor_p->phaseSteps_p->roundedAccel > 1) { // needed for accel rates < 1 and to handle decimal accel rates
			motor_p->matchConfig.matchValue -= (uint32_t) motor_p->phaseSteps_p->roundedAccel;
			motor_p->phaseSteps_p->roundedAccel -= (uint32_t) motor_p->phaseSteps_p->roundedAccel; // remove amount applied
		}
		//PRINTF("\r\n\r\n status.accel: %d, status.steady: %d, status.running: %d", status.accelerating, status.steady, status.running);
		//PRINTF("\r\n phase.accel: %d, phase.steady: %d, phase.slowing: %d", phaseSteps.accelerating, phaseSteps.steady, phaseSteps.slowing);
		//PRINTF("\r\n accumulativeAccel: %2.2f, phaseSteps.accelVal: %.4f", accumulativeAccel, phaseSteps.accelValue);
		if (motor_p->phaseSteps_p->accelerating == 0) {
			//PRINTF("\r\n\r\n hit ZERO!!!! \r\n\r\n");
			motor_p->status.accelerating = false;
			if (motor_p->phaseSteps_p->steady != 0)
				motor_p->status.steady = true;
			else if (motor_p->phaseSteps_p->slowing != 0)
				motor_p->status.slowing = true;
			else
				stopMotor(motor_p);
		}
		CTIMER_SetupMatch(motor_p->timer_p, motor_p->output, &(motor_p->matchConfig));
	}
	else if (motor_p->status.steady) {
		motor_p->position += motor_p->direction ? 1 : -1;
		motor_p->phaseSteps_p->steady--;
		if (motor_p->phaseSteps_p->steady == 0) {
			motor_p->status.steady = false;
			if (motor_p->phaseSteps_p->slowing != 0)
				motor_p->status.slowing = true;
			else
				stopMotor(motor_p);
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
			stopMotor(motor_p);
			//PRINTF("step Count : %d", count);
			//count = 0;
		}
		CTIMER_SetupMatch(motor_p->timer_p, motor_p->output, &(motor_p->matchConfig));
	}
	else if (motor_p->status.startSpeed) { // lowest freq requirements so can be last in conditionals w/ least consequence
		motor_p->position += motor_p->direction ? 1 : -1;
		motor_p->phaseSteps_p->startSpeed--;
		if (motor_p->phaseSteps_p->startSpeed == 0) {
			motor_p->status.startSpeed = false;
			stopMotor(motor_p);
		}
	}
	else {
		stopMotor(motor_p); // failsafe. If somehow the bools get messed up and one of them isn't one, stop motor
	}
	STEPPERS_writeOutputPin(motor_p->pinInfo_arr_p[PWMOUT], 0U); // clear
	// clearing here means our PWM period starts with a cleared pulse and then a set pulse. This guarentees the motors always clear when done.
}

void stepperXFault(pint_pin_int_t pintr, uint32_t pmatch_status) {
	PRINTF("\r\n motor fault on stepper X detected!!! stopping motor...");
	stopMotor(stepperX_p);
}

void stepperYFault(pint_pin_int_t pintr, uint32_t pmatch_status) {
	PRINTF("\r\n motor fault on stepper Y detected!!! stopping motor...");
	stopMotor(stepperY_p);
}

void STEPPERS_writeOutputPin(pinInformation_s* pinInfo_p, bool highLow) {
	GPIO_PinWrite(GPIO, pinInfo_p->port, pinInfo_p->pin, highLow);
}
bool STEPPERS_readInputPin(pinInformation_s* pinInfo_p) {
	return GPIO_PinRead(GPIO, pinInfo_p->port, pinInfo_p->pin);
}

// End Functions
