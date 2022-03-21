

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
status_t getCenterFromRectCorners(const point_s*, uint8_t, point_s*);
void pollADC(stepperMotor_s*, int32_t);
bool isAlignmentBracket();
bool isBlack();
bool isWhite();

#endif
