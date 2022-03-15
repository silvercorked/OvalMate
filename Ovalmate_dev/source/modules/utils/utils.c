
#include "utils.h"

#define STEPSIZE 			1143U	// 0.01143 millimeters.
// to avoid floating point percision error, we can store this in integers.
// This value is 1.143 x 10 ^-6 meters
// This value is 11430 nanometers
#define STEPSIZENANOMETERS	11430U

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
