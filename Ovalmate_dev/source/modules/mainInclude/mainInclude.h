/**
 * Title: OvalMate_button_module_DevBoard
 * Version: 0.0.1
 * Filename: LPC55S16_Project_main_dev.c
 * Authors: Alex Wissing
 * Purpose of Program: Initialize button interrupts
 * How Program is Run on Target System:
 * Date Started: 3/2/2022
 * Update History:
 *	- 3/3/2022:
 *		Setup helper functions using multiple features from different modules
 *		Created macro functions for motor driving needs
 *	- 3/6/2022:
 *		Removed some helper functions as systems are changing soon. These extra functions require functions that may not be defined
 *		once changes go through. Commenting out for now
 *	- 3/9/2022:
 *		Added definition safeguard to avoid duplicate declarations
 */

#ifndef MAININCLUDE_H
#define MAININCLUDE_H

// Core Includes
#include <stdio.h>
#include "fsl_debug_console.h"  // this gives access to PRINTF for debugging
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
// #include "clock_config.h" // not used atm, but commonly used
// #include "LPC55S16.h" // somehow not used rn

// Module Includes
#include "modules/sevenSegmentLED/sevenSegmentLED.h" // 7 segment display
#include "modules/irSensor/irSensor.h" // irSensor via ADC
#include "modules/servo/servo.h" // servo via CTIMER
#include "modules/buttons/buttons.h" // button interrupts via pin_mux
// ^ consumes pin interrupts 0-4
#include "modules/steppers/steppers.h" // stepper via SCTIMER 2 in 16-bit mode
// ^ consumes pin interrupts 5 & 6
// End Module Includes

// all variables used will start with the function callback's name in which they are used
// Variables
extern bool buttonCallback_stepperMotorX_stopped;
extern bool buttonCallback_stepperMotorY_stopped;
// End Variables

// all functions will start with the void pointer they are meant to be assigned to.
// Prototypes
status_t configure(void);
void buttonCallback_stopMotors(pint_pin_int_t, uint32_t);	// buttonCallbacks are of this form
void buttonCallback_stopRelaventMotor(pint_pin_int_t, uint32_t);
void findHome(void);
void pollADC(stepperMotor_s*, uint32_t);
//void motorCallback_scheduleNextJob(stepperMotor_t*);		// motorCallbacks are of this form
//void motorCallback_printCurrSteps(stepperMotor_t*);
//void stopMotorIfJobComplete(stepperMotor_t*);
//void driveStepperPWM(whichMotor, uint32_t, uint32_t, bool);
// End Prototypes

#endif
