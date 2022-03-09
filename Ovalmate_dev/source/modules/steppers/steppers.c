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
 */

#include "steppers.h"

// Variables
pinInformation_s stepperX_pinInfo = { .port = 0, .pin = 2 };
pinInformation_s stepperY_pinInfo = { .port = 0, .pin = 3 };
gpio_pin_config_t gpioConfig = {
	.pinDirection = kGPIO_DigitalOutput,
	.outputLogic = 0
};
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
	.pinInfo_p = &stepperX_pinInfo,
	.phaseSteps_p = &stepperX_phaseSteps,
	.matchCallback = &stepperXTimerCallback,
	.output = CTIMER0_MATCH0_CHANNEL,
	.matchConfig = { },
	.status = { }, // defaults are false
	// .position = 0
	// .direction = false
};
stepperMotor_s stepperY = {
	.timer_p = CTIMER1_Y,
	.pinInfo_p = &stepperY_pinInfo,
	.phaseSteps_p = &stepperY_phaseSteps,
	.matchCallback = &stepperYTimerCallback,
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
void initializeStepperOutputPins() {
    GPIO_PinInit(GPIO, stepperX_pinInfo.port, stepperX_pinInfo.pin, &gpioConfig);
	GPIO_PinInit(GPIO, stepperY_pinInfo.port, stepperY_pinInfo.pin, &gpioConfig);
}
void writeStepperOutputPin(pinInformation_s* pInfo_p, bool highLow) {
	GPIO_PinWrite(GPIO, pInfo_p->port, pInfo_p->pin, highLow);
}

void initializeStepperMotors() {
	ctimer_config_t config;					// can set prescale if need be (we don't need it for these ranges
	CTIMER_GetDefaultConfig(&config);		// different from MatchConfig
	CTIMER_Init(CTIMER0_X, &config);		// this config handles timer setup
	CTIMER_Init(CTIMER1_Y, &config);
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

	CTIMER_RegisterCallBack(motor_p->timer_p, motor_p->matchCallback, kCTIMER_SingleCallback);
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
	else
		motor_p->direction = false;	// if negative, dir = false
	driveStepperSteps(motor_p, steps);
	return kStatus_Success;
}

status_t moveTo(stepperMotor_s* motor_p, uint32_t target) {
	int32_t relativeSteps = target - motor_p->position;
	return moveSteps(motor_p, relativeSteps);
}

status_t setMotorStepsPerPhase(stepperMotorPhaseSteps_s* phaseSteps_p, uint32_t steps) {
	if (steps <= 100)
		phaseSteps_p->startSpeed = steps;	// all steps are no accel at start speed
	else if (steps <= 1600) { // one full revolution
		phaseSteps_p->accelerating = steps / 4;					// 1/4 accel
		phaseSteps_p->slowing = phaseSteps_p->accelerating;		// 1/4 decel
		steps -= phaseSteps_p->accelerating << 1;				// left shift once == multiply by 2 in 1 clock
		phaseSteps_p->steady = steps;							// rest at max speed
	}
	else {
		phaseSteps_p->accelerating = steps / 10;						// 1/10 accel
		phaseSteps_p->slowing = phaseSteps_p->accelerating;				// 1/10 decel
		steps -= phaseSteps_p->accelerating + phaseSteps_p->slowing;	// addition faster when not by power of 2
		phaseSteps_p->steady = steps;									// rest at max speed
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
		writeStepperOutputPin(stepperX_p->pinInfo_p, 1U); // write high
	periodOffsetX = !periodOffsetX;
}
void stepperYTimerCallback(uint32_t flags) {
	static bool periodOffsetY = false; // need to skip 1 interrupt per 2 to make pwm
	if (periodOffsetY)
		stepperGeneralTimerCallback(flags, stepperY_p);
	else
		writeStepperOutputPin(stepperY_p->pinInfo_p, 1U); // write high
	periodOffsetY = !periodOffsetY;
}
/**
 * designed with 2 modes in mind, either staying to one freq or
 * following a pattern of accelerating (+ velocity), sitting at max speed, and accelerating (- velocity)
 */
void stepperGeneralTimerCallback(uint32_t flags, stepperMotor_s* motor_p) {
	// handle based on bools, send output, update bools
	stepperMotorStatus_s status = motor_p->status;
	stepperMotorPhaseSteps_s phaseSteps = *(motor_p->phaseSteps_p);

	if (status.accelerating) {
		motor_p->position += motor_p->direction ? 1 : -1;
		phaseSteps.accelerating--;
		if (phaseSteps.accelerating == 0) {
			status.accelerating = false;
			if (phaseSteps.steady != 0)
				status.steady = true;
			else if (phaseSteps.slowing != 0)
				status.slowing = true;
			else
				stopMotor(motor_p);
		}
	}
	else if (status.steady) {
		motor_p->position += motor_p->direction ? 1 : -1;
		phaseSteps.steady--;
		if (phaseSteps.steady == 0) {
			status.steady = false;
			if (phaseSteps.slowing != 0)
				status.slowing = true;
			else
				stopMotor(motor_p);
		}
	}
	else if (status.slowing) {
		motor_p->position += motor_p->direction ? 1 : -1;
		phaseSteps.slowing--;
		if (phaseSteps.steady == 0) {
			status.steady = false;
			stopMotor(motor_p);
		}
	}
	else if (status.startSpeed) { // lowest freq requirements so can be last in conditionals w/ least consequence
		motor_p->position += motor_p->direction ? 1 : -1;
		phaseSteps.startSpeed--;
		if (phaseSteps.startSpeed == 0) {
			status.startSpeed = false;
			stopMotor(motor_p);
		}
	}
	else {
		stopMotor(motor_p); // failsafe. If somehow the bools get messed up and one of them isn't one, stop motor
	}
	writeStepperOutputPin(motor_p->pinInfo_p, 0U); // clear
	// clearing here means our PWM period starts with a cleared pulse and then a set pulse. This guarentees the motors always clear when done.
}
// End Functions
