

#ifndef ADVANCEDSTEPPERS_H
#define ADVANCEDSTEPPERS_H

// Includes
#include "modules/irSensor/irSensor.h"
#include "modules/servo/servo.h"
#include "modules/steppers/steppers.h"
#include "modules/utils/utils.h"
// End Includes

// Definitions
#define OVALLINES				12
#define OVALLINESHALF			(OVALLINES / 2)
#define OVALLINESTARTPERCENT	5				// percentage of start and end that will be ignored in considering range
#define OVALSTARTOFFSET			OVALLINESTARTPERCENT * 10000U
// #define OVALLINEOFFSET		((1.0 - (1.0 / (OVALLINESTARTPERCENT * 2.0))) * 1000000.0) / OVALLINESHALF // not used as the floating point from the preprocessor doesnt seem to operate the same as in code
// End Definitions

// Structs
typedef struct {
	uint32_t x;
	uint32_t y;
} point_s;
typedef struct {
	uint32_t x;
	uint32_t y;
	uint32_t value;
} sample_s;
// End Structs

// Prototypes
void drawOval(void);
void drawRectangle(void);
// End Prototypes

#endif
