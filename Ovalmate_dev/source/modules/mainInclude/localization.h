

#ifndef LOCALIZATION_H
#define LOCALIZATION_H

#include "advancedSteppers.h"
#include "modules/irSensor/irSensor.h"
#include <stdbool.h>

#include <stdio.h>
#include "fsl_debug_console.h"  // this gives access to PRINTF for debugging

extern sample_s pastSamples[100];

status_t findDocumentCorners();
status_t followBlackSquares(stepperMotor_s*);
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

#endif
