
#include "advancedSteppers.h"

// Variables
uint32_t OVALLINEOFFSET =		((1.0 - (1.0 / (OVALLINESTARTPERCENT * 2.0))) * 1000000.0) / OVALLINESHALF; // Take twice percent off for both ends, get range we'll divide based on one side of oval and scale to nm
// End Variables

/** ballistic behavior. Presumes at center of oval
 * Oval is 3mm x 2mm (width x height). To avoid loss from floating point precision, convert to nanometers and divide oval in half.
 * Half oval = 1.5mm x 1mm = 1500000nm x 1000000nm. Steppers w/ microstepping at 16 = 0.01143mm = 11430nm per step.
 * Half oval = parabola. Rotate 90 degrees for concave downward parabola. Region defined by y > 0 and parabola need to be shaded.
 * width of pen mark + bleed of pen ~= 0.25mm. 1.5mm x 1mm => 263 steps x 175 steps, so 4 lines across width of oval and mirrored on negative side => filled
 * On parabola, equation in mm is y = -1.5 * (x^2) + 1.5. In nm, -1500000 ((x / 1000000)^2) + 1500000.
 *
 * @params	- None
 * @return	- None
 */
void drawOval() {
	int8_t k = OVALLINESHALF;				// start value
	int32_t currX = 0; // presume in center of oval
	int32_t currY = 0; // presume in center of oval
	int8_t kInc = -1;

	SERVO_setPenMode(PENDOWN);
	delay20ms();

	while (k <= OVALLINESHALF) { // negate x result if doing negative side
		int32_t x = kInc == -1 ? -(k * OVALLINEOFFSET + OVALSTARTOFFSET) : k * OVALLINEOFFSET + OVALSTARTOFFSET; // 0.15 mm * k + 0.05 mm = 6 evenly spaced divisions across 0-1 given input in space 0-6
		int32_t relativeX = x - currX;
		uint32_t xPosSteps = nanometersToSteps((uint64_t) (relativeX < 0 ? -relativeX : relativeX)); // negative side first, so

		double scaledX = (double) x / 1000000.0L; // scaled to 1mm (1000000 nm)
		uint32_t y = -1500000 * (scaledX * scaledX) + 1500000; // -1.5mm * x^2 + 1.5mm = equation of parabola
		int32_t relativeY = y - currY;
		uint32_t yPosSteps = nanometersToSteps((uint64_t)relativeY);

		//STEPPERS_moveRelativeNoAccelNoBlock(stepperY_p, relativeX > 0 ? xPosSteps : -xPosSteps);
		//STEPPERS_moveRelativeNoAccelNoBlock(stepperX_p, relativeY > 0 ? -yPosSteps : yPosSteps);
		//while (stepperX_p->status.running || stepperY_p->status.running);
		STEPPERS_moveBothRelativeNoAccelNoBlock(relativeX > 0 ? xPosSteps : -xPosSteps, relativeY > 0 ? -yPosSteps : yPosSteps);
		// the parabola model is a 90 degree rotation of half the oval. So X coords actually correspond to Y's motor

		currX = x;
		currY = y; // in position
		uint32_t lineSteps = nanometersToSteps(y << 1);
		STEPPERS_moveRelativeNoAccel(stepperX_p, lineSteps); // y * 2 == line width
		//while(stepperX_p->status.running);
		currY = -y;
		k += kInc;
		if (k == -1) {
			kInc = 1;
			k++;
		}
		delay1ms();
	}
	SERVO_setPenMode(PENUP);
}

/**
 * 3000 x 250 debugging oval (for moving around a rectangle and finding IR center
 *
 * @params	- None
 * @return	- None
 */
void drawRectangle() {
	SERVO_setPenMode(PENDOWN);
	delay20ms();
	uint32_t i = 0;
	while (i < 250) {
		STEPPERS_moveRelativeAccel(stepperX_p, i % 10 == 0 ? -3000 : 3000);
		STEPPERS_moveRelativeNoAccel(stepperY_p, 5);
		delay1ms();
		i+= 5;
	}
	SERVO_setPenMode(PENUP);
	delay20ms();
}
