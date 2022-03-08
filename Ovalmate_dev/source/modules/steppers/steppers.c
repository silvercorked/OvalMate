

// Variables
stepperMotor_t mX = {

	.pinInfo = { // pin 81
		.port = 0,
		.pin = 2
	}
};
stepperMotor_t* motorX = &mX;
stepperMotor_t mY = {

	.pinInfo = { // pin 83
		.port = 0,
		.pin = 3
	}
};
stepperMotor_t* motorY = &mY;
gpio_pin_config_t stepperMotorGPIOConfig = { .pinDirection = kGPIO_DigitalOutput, .outputLogic = 0};
// End Variables

// Functions
void initializeStepperPins() {
	GPIO_PinInit(GPIO, motorX->pinInfo.port, motorX->pinInfo.pin, &stepperMotorGPIOConfig);
	GPIO_PinInit(GPIO, motorY->pinInfo.port, motorY->pinInfo.pin, &stepperMotorGPIOConfig);
}
void clearTarget(whichMotor, uint32_t); // set target to coord & clear pin
void setPosition(whichMotor, uint32_t);
uint32_t distanceFromTarget(whichMotor);
void computeNewSpeed(whichMotor);
status_t checkForStep(whichMotor);
void step(whichMotor);
status_t setMaxSpeed(whichMotor, float);
status_t setAcceleration(whichMotor, float);
status_t setSpeed(whichMotor, float);
status_t setPin(whichMotor which) {
	stepperMotor_t* m = (stepperMotor_t*) which;
	GPIO_PinWrite(GPIO, m->pinInfo.port, m->pinInfo.pin, 1);
}
status_t clearPin(whichMotor) {
	stepperMotor_t* m = (stepperMotor_t*) which;
	GPIO_PinWrite(GPIO, m->pinInfo.port, m->pinInfo.pin, 0);
}
status_t togglePin(whichMotor) {
	stepperMotor_t* m = (stepperMotor_t*) which;
	GPIO_PortToggle(GPIO, m->pinInfo.port, (1 << m->pinInfo.pin));
}
