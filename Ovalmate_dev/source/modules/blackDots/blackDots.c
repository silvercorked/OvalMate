/**
 * Title: OvalMate_blackDots_module_DevBoard
 * Version: 0.0.1
 * Filename: blackDots.c
 * Authors: Alex Wissing
 * Purpose of Program: Manage the "blackDots" data structure, which is a specialized array of integers coded to represent a 2d matrix
 * How Program is Run on Target System:
 * Date Started: 3/19/2022
 * Update History:
 *	- 3/19/2022:
 *		Setup basic functions, which give a nod towards what the data structure truely is.
 *		getRow, getCol, setPoint, clearPoint, changePoint, togglePoint, and getPoint were setup.
 */

#include "blackDots.h"

volatile uint32_t points[70];

uint32_t BLACKDOTS_getRow(uint8_t x) {
	return points[x];
}
bool BLACKDOTS_getCol(uint32_t row, uint8_t y) {
	return ((bool) row & (1 << y));
}
void BLACKDOTS_setPoint(uint8_t x, uint8_t y) {
	points[x] |= 1U << y;
}
void BLACKDOTS_clearPoint(uint8_t x, uint8_t y) {
	points[x] &= ~(1U << y);
}
void BLACKDOTS_changePoint(uint8_t x, uint8_t y, bool highLow) {
	points[x] = (points[x] & ~(1U << y)) | (highLow << y);
}
void BLACKDOTS_togglePoint(uint8_t x, uint8_t y) {
	points[x] ^= 1U << y;
}
bool BLACKDOTS_getPoint(uint8_t x, uint8_t y) {
	return BLACKDOTS_getCol(BLACKDOTS_getRow(x), y);
}
