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
#include <modules/utils/conversion.h>
// End Includes

// Functions
/**
 * Converts a number of steps to nanometers.
 *
 * @params  - uint32_t steps	: the number of steps
 * @return  - uint64_t 			: the number of nanometers that the given steps represent
 */
uint64_t stepsToNanometers(uint32_t steps) {
	return ((uint64_t) steps) * STEPSIZENANOMETERS;
}

/**
 * Converts a number of nanometers to millimeters.
 *
 * @params  - uint64_t nanometers	: the number of nanometers
 * @return  - double				: the number of millimeters that the given nanometers represent
 */
double nanometersToMillimeters(uint64_t nanometers) {
	return ((double) nanometers) / 1000000; // (10^-6)
}

/**
 * Converts a number of nanometers to micrometers.
 *
 * @params  - uint64_t nanometers	: the number of nanometers
 * @return  - double				: the number of micrometers that the given nanometers represent
 */
double nanometersToMicrometers(uint64_t nanometers) {
	return ((double) nanometers) / 1000; // (10^-3)
}

/**
 * Converts a number of nanometers to steps.
 *
 * @params  - uint64_t nanometers	: the number of nanometers
 * @return  - uint32_t 				: the number of steps that the given nanometers represent
 */
uint32_t nanometersToSteps(uint64_t nanometers) {
	return (uint32_t) nanometers / STEPSIZENANOMETERS; // forced rounded down
}

/**
 * Converts a number of millimeters to nanometers.
 *
 * @params  - double millimeters	: the number of millimeters
 * @return  - uint64_t				: the number of nanometers that the given millimeters represent
 */
uint64_t millimetersToNanometers(double millimeters) {
	return (uint64_t) (millimeters * 1000000); // (10^6)
}

/**
 * Converts a number of millimeters to steps.
 *
 * @params  - double millimeters	: the number of millimeters
 * @return  - uint32_t				: the number of steps that the given millimeters represent
 */
uint32_t millimetersToSteps(double millimeters) {
	return nanometersToSteps(millimetersToNanometers(millimeters));
}
// End Functions
