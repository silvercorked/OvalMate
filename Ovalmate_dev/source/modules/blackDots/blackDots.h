/**
 * Title: OvalMate_blackDots_module_DevBoard
 * Version: 0.0.1
 * Filename: blackDots.h
 * Authors: Alex Wissing
 * Purpose of Program: Manage the "blackDots" data structure, which is a specialized array of integers coded to represent a 2d matrix
 * How Program is Run on Target System:
 * Date Started: 3/19/2022
 * Update History:
 *	- 3/19/2022:
 *		Setup basic functions, which give a nod towards what the data structure truely is.
 *		getRow, getCol, setPoint, clearPoint, changePoint, togglePoint, and getPoint were setup.
 */

#ifndef BLACKDOTS_H
#define BLACKDOTS_H

// Includes
#include <stdint.h>
#include <stdbool.h>
// End Includes

// Extern Variables
extern volatile uint32_t points[70];
// End Extern Variables

// Prototypes
uint32_t BLACKDOTS_getRow(uint8_t);
bool BLACKDOTS_getCol(uint32_t, uint8_t);
void BLACKDOTS_setPoint(uint8_t, uint8_t);
void BLACKDOTS_clearPoint(uint8_t, uint8_t);
void BLACKDOTS_changePoint(uint8_t, uint8_t, bool);
void BLACKDOTS_togglePoint(uint8_t, uint8_t);
bool BLACKDOTS_getPoint(uint8_t, uint8_t);
// End Prototypes

#endif
