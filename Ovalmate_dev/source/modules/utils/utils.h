/**
 * Title: OvalMate_utils_module_DevBoard
 * Version: 0.0.1
 * Filename: utils.h
 * Authors: Alex Wissing
 * Purpose of Program: Convert between the 3 major coordinate systems (ie, stepperMotor steps, black dots, and real-world measurements)
 * How Program is Run on Target System:
 * Date Started: 3/14/2022
 * Update History:
 *	- 3/14/2022
 *		Created conversions file.
 */

#ifndef UTILS_H
#define UTILS_H

// Includes
#include <stdint.h>
// End Includes

// Definitions
#define STEPSIZE 			1143U	// 0.01143 millimeters.
// to avoid floating point percision error, we can store this in integers.
// This value is 1.143 x 10 ^-6 meters
// This value is 11430 nanometers
#define STEPSIZENANOMETERS	11430U
// End Definitions

// Prototypes
uint64_t stepsToNanometers(uint32_t);
double nanometersToMillimeters(uint64_t);
double nanometersToMicrometers(uint64_t);
uint32_t nanometersToSteps(uint64_t);
uint64_t millimetersToNanometers(double);
uint32_t millimetersToSteps(double);
// blackDotsToNanometers
// blackDotsToSteps
// End Prototypes

#endif
