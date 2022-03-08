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
 *		Setup helper functions using multilpe features from different modules
 *		Created macro functions for motor driving needs
 */

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
#include "modules/steppers/steppers.h" // stepper via SCTIMER 2 in 16-bit mode
// End Module Includes

// all variables used will start with the function callback's name in which they are used
// Variables

// End Variables

// all functions will start with the void pointer they are meant to be assigned to.
// Prototypes
status_t configure(void);
void buttonCallback_stopMotors(pint_pin_int_t, uint32_t);	// buttonCallbacks are of this form
//void motorCallback_scheduleNextJob(stepperMotor_t*);		// motorCallbacks are of this form
//void motorCallback_printCurrSteps(stepperMotor_t*);
//void stopMotorIfJobComplete(stepperMotor_t*);
void driveStepperPWM(whichMotor, uint32_t, uint32_t, bool);
// End Prototypes
