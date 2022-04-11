/**
 * Title: OvalMate_stepper_module_DevBoard
 * Version: 0.0.1
 * Filename: LPC55S16_Project_main_dev.c
 * Authors: Alex Wissing
 * Purpose of Program: Find Pen location and deduce location of document artifacts
 * How Program is Run on Target System:
 * Date Started: 3/26/2022
 * Update History:
 *	- 3/26/2022
 *		Setup localization with findHome and pollADC
 *	- 3/27/2022
 *		Added polling over an ADC range (replace only part of the 100 sample range with new range.
 *		Added functions for finding different patterns in polledSample range
 *	- 3/28/2022
 *		Setup functions for finding rectangles and getting a center point given the 4 corners of a rectangle
 *	- 3/29/2022
 *		Created functions to check if we are above certain IR values which represent white paper, black paper, and the mounting brackets
 *	- 3/30/2022
 *		Editing findDocumentCorners to locate the top left and top right rectangles, then verify with the bottom left rectangle
 */

#ifndef LOCALIZATION_H
#define LOCALIZATION_H

// Includes
#include "advancedSteppers.h"
#include "modules/irSensor/irSensor.h"
#include <stdbool.h>

#include <stdio.h>
#include "fsl_debug_console.h"  // this gives access to PRINTF for debugging
// End Includes

// Definitions
#define SCALEFACTOR				1.31612903226L
// 1.31612903226 = 255 / 193.75 (255 distance form top row to bottom row (measured from center)) (193.75 = distance between col0 center and col23 center)
// 1.31443298969 = 255 / 194 (255 = distance from center of rectangle on top to rectangle on bottom) (194 = distance from topLeft rectangle to 24th column rectangle on top)
// 1.38586956522 = 255 / 184 (255 = distance from center of rectangle on top to rectangle on bottom) (184 = distance from top 1st column rectangle to 24th column rectangle on top)
#define ISBLACKTHRESHOLD		30000U
#define ISWHITEUPPERTHRESHOLD	20000U
#define ISWHITELOWERTHRESHOLD	4000U
// End Definitions

// Variables
extern sample_s pastSamples[100];
extern point_s topLeft;
extern point_s topRight;
extern point_s bottomLeft;
extern bool buttonCallback_stepperMotorX_stopped;
extern bool buttonCallback_stepperMotorY_stopped;
// End variables

// Prototypes
status_t findDocumentCorners();
status_t followBlackSquares(stepperMotor_s*);
status_t findHome(void);
status_t findRectangleCorners(point_s*, uint8_t);
status_t findRectangleCorner(sample_s*, bool, bool, bool, bool, bool);
status_t getCenterFromRectCorners(const point_s[], uint8_t, point_s*);
void pollADC(stepperMotor_s*, int32_t);
void pollADCRange(stepperMotor_s* motor_p, int32_t steps, uint8_t start, uint8_t end);
uint8_t locateBetterCenter(stepperMotor_s* motor_p, uint8_t start, uint8_t end);
uint8_t findPeak(uint8_t start, uint8_t end);
uint8_t findRiseStart(uint8_t start);
uint8_t findFallEnd(uint8_t start);
bool aboveAlignmentBracket();
bool aboveBlack();
bool aboveWhite();
bool isAlignmentBracket(uint32_t);
bool isBlack(uint32_t);
bool isWhite(uint32_t);
void buttonCallback_stopMotors(pint_pin_int_t, uint32_t);	// buttonCallbacks are of this form
void buttonCallback_stopRelaventMotor(pint_pin_int_t, uint32_t);
// End Prototypes

#endif
