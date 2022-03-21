/**
 * Title: OvalMate_utils_module_DevBoard
 * Version: 0.0.1
 * Filename: utils.c
 * Authors: Alex Wissing
 * Purpose of Program: Convert between the 3 major coordinate systems (ie, stepperMotor steps, black dots, and real-world measurements)
 * How Program is Run on Target System:
 * Date Started: 3/14/2022
 * Update History:
 *	- 3/14/2022
 *		Created conversions file.
 */

// Includes
#include "utils.h"
// End Includes

// Functions
uint64_t stepsToNanometers(uint32_t steps) {
	return ((uint64_t) steps) * STEPSIZENANOMETERS;
}

// double for more precision on decimal
double nanometersToMillimeters(uint64_t nanometers) {
	return ((double) nanometers) / 1000000; // (10^-6)
}

// double for more precision on decimal
double nanometersToMicrometers(uint64_t nanometers) {
	return ((double) nanometers) / 1000; // (10^-3)
}

uint32_t nanometersToSteps(uint64_t nanometers) {
	return (uint32_t) nanometers / STEPSIZENANOMETERS; // forced rounded down
}

uint64_t millimetersToNanometers(double millimeters) {
	return (uint64_t) (millimeters * 1000000); // (10^6)
}

uint32_t millimetersToSteps(double millimeters) {
	return nanometersToSteps(millimetersToNanometers(millimeters));
}
// End Functions
