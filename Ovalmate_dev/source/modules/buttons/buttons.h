/**
 * Title: OvalMate_button_module_DevBoard
 * Version: 0.0.1
 * Filename: LPC55S16_Project_main_dev.c
 * Authors: Alex Wissing
 * Purpose of Program: Initialize button interrupts
 * How Program is Run on Target System:
 * Date Started: 2/28/2022
 * Update History:
 *	- 3/3/2022:
 *		Added variables for assignable function callbacks for button interrupts
 *	- 3/8/2022:
 *		Added definition safeguards to avoid duplicate declarations
 *	- 3/14/2022:
 *		Reformatted function names to better represent module approach
 */

#ifndef BUTTONS_H
#define BUTTONS_H

#include <stdio.h>
#include "fsl_debug_console.h"  // this gives access to PRINTF for debugging

// Includes
#include "fsl_pint.h"
#include "fsl_inputmux.h"
// End Includes

// Definitions
#define PINT_PIN_INT1_SRC			kINPUTMUX_GpioPort1Pin25ToPintsel // right
#define PINT_PIN_INT2_SRC			kINPUTMUX_GpioPort1Pin26ToPintsel // left
#define PINT_PIN_INT3_SRC			kINPUTMUX_GpioPort1Pin27ToPintsel // down
#define PINT_PIN_INT4_SRC			kINPUTMUX_GpioPort0Pin9ToPintsel  // up
#define PINT_PIN_INT0_SRC			kINPUTMUX_GpioPort0Pin0ToPintsel  // emergency
// End Definitions

// Variables
extern void (*emergencyBumpCallback)(pint_pin_int_t, uint32_t);
extern void (*rightBumpCallback)(pint_pin_int_t, uint32_t);
extern void (*leftBumpCallback)(pint_pin_int_t, uint32_t);
extern void (*upBumpCallback)(pint_pin_int_t, uint32_t);
extern void (*downBumpCallback)(pint_pin_int_t, uint32_t);
// End Variables

// Prototypes
void BUTTONS_buttonEmergencyCallback(pint_pin_int_t pintr, uint32_t pmatch_status);
void BUTTONS_bumpRightCallback(pint_pin_int_t pintr, uint32_t pmatch_status);
void BUTTONS_bumpLeftCallback(pint_pin_int_t pintr, uint32_t pmatch_status);
void BUTTONS_bumpUpCallback(pint_pin_int_t pintr, uint32_t pmatch_status);
void BUTTONS_bumpDownCallback(pint_pin_int_t pintr, uint32_t pmatch_status);
void BUTTONS_assignPinsToInterrupts(void);
// End Prototypes

#endif
