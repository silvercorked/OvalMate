/**
 * Title: OvalMate_sevenSegmentLED_module_DevBoard
 * Version: 0.0.1
 * Filename: LPC55S16_Project_main_dev.c
 * Authors: Alex Wissing
 * Purpose of Program: Manage 7-segment LEDs
 * How Program is Run on Target System:
 * Date Started: 2/28/2022
 * Update History:
 *
 */

// INTERRUPT INCLUDES
#include "fsl_pint.h"
#include "fsl_inputmux.h"

// Interrupt Global Definitions
#define PINT_PIN_INT1_SRC			kINPUTMUX_GpioPort1Pin25ToPintsel // right
#define PINT_PIN_INT2_SRC			kINPUTMUX_GpioPort1Pin26ToPintsel // left
#define PINT_PIN_INT3_SRC			kINPUTMUX_GpioPort1Pin27ToPintsel // down
#define PINT_PIN_INT4_SRC			kINPUTMUX_GpioPort0Pin9ToPintsel  // up
#define PINT_PIN_INT0_SRC			kINPUTMUX_GpioPort0Pin0ToPintsel  // emergency
// End Interrupt Global Definitions

// Interrupt Prototypes
void buttonEmergencyCallback(pint_pin_int_t pintr, uint32_t pmatch_status);
void bumpRightCallback(pint_pin_int_t pintr, uint32_t pmatch_status);
void bumpLeftCallback(pint_pin_int_t pintr, uint32_t pmatch_status);
void bumpUpCallback(pint_pin_int_t pintr, uint32_t pmatch_status);
void bumpDownCallback(pint_pin_int_t pintr, uint32_t pmatch_status);
void assignPinsToInterrupts(void);
// End Interrupt Prototypes