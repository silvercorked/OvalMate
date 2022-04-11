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

volatile uint32_t points[50];

uint32_t BLACKDOTS_getRow(uint8_t y) {
	return points[y];
}
bool BLACKDOTS_getCol(uint32_t row, uint8_t x) {
	return (row & (1 << x)) != 0;
}
void BLACKDOTS_setPoint(uint8_t x, uint8_t y) {
	points[y] |= 1U << x;
}
void BLACKDOTS_clearPoint(uint8_t x, uint8_t y) {
	points[y] &= ~(1U << x);
}
void BLACKDOTS_changePoint(uint8_t x, uint8_t y, bool highLow) {
	points[y] = (points[y] & ~(1U << x)) | (highLow << x);
}
void BLACKDOTS_togglePoint(uint8_t x, uint8_t y) {
	points[y] ^= 1U << x;
}
bool BLACKDOTS_getPoint(uint8_t x, uint8_t y) {
	return BLACKDOTS_getCol(BLACKDOTS_getRow(y), x);
}
void BLACKDOTS_getPointAsCoordinate(uint8_t x, uint8_t y, blackDotCoordinate_s* dot) {
	dot->row = y;
	dot->col = x;
	dot->value = BLACKDOTS_getPoint(x, y);
}
void BLACKDOTS_getNext(blackDotCoordinate_s* dot) {
	/*// this version does each dot in a row then moves to next col
	dot->row = -1;
	dot->col = -1;
	dot->value = false;
	bool found = false;
	for (uint8_t y = 0; y < 50; y++) {
		uint32_t row = BLACKDOTS_getRow(y);
		if (row != 0) {
			for (uint8_t x = 0; x < 24; x++) {
				if (BLACKDOTS_getCol(row, x)) {
					dot->row = y;
					dot->col = x;
					dot->value = true;
					found = true;
					break;
				}
			}
			if (found)
				break;
		} // row == 0, ie empty
	}
	*/
	// this version does each dot in a col then moves to next row
	dot->row = -1;
	dot->col = -1;
	dot->value = false;
	bool found = false;
	for (uint8_t x = 0; x < 24; x++) {
		for (uint8_t y = 0; y < 50; y++) {
			if (BLACKDOTS_getPoint(x, y)) {
				dot->row = y;
				dot->col = x;
				dot->value = true;
				found = true;
				break;
			}
		}
		if (found)
			break;
	}
	/* // this version is the same as the previous but backwards
	dot->row = -1;
	dot->col = -1;
	dot->value = false;
	bool found = false;
	for (int8_t x = 24; x >= 0; x--) {
		for (int8_t y = 50; y >= 0; y--) {
			if (BLACKDOTS_getPoint(x, y)) {
				dot->row = y;
				dot->col = x;
				dot->value = true;
				found = true;
				break;
			}
		}
		if (found)
			break;
	}
	*/
	/* // this version is the same as the first but backwards
	dot->row = -1;
	dot->col = -1;
	dot->value = false;
	bool found = false;
	for (int8_t y = 50; y >= 0; y--) {
		for (int8_t x = 24; x >= 0; x--) {
			if (BLACKDOTS_getPoint(x, y)) {
				dot->row = y;
				dot->col = x;
				dot->value = true;
				found = true;
				break;
			}
		}
		if (found)
			break;
	}
	*/
}
