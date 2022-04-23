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

volatile uint32_t points[BLACKDOTS_MAXROWS];
volatile bool jobReady = false;

/**
 * returns the row at the specified index.
 *
 * @params	- uint8_t	row				: the index of the desired row.
 * @return	- uint32_t					: the row.
 */
uint32_t BLACKDOTS_getRow(uint8_t y) {
	if (y >= BLACKDOTS_MAXROWS) return 0;
	return points[y];
}

/**
 * returns the col value at the specified row.
 *
 * @params	- uint32_t	row				: the row.
 * @params	- uint8_t	x				: the column index.
 * @return	- bool						: the column value.
 */
bool BLACKDOTS_getCol(uint32_t row, uint8_t x) {
	if (x >= BLACKDOTS_MAXCOLS) return false;
	return (row & (1 << x)) != 0;
}

/**
 * sets the value to 1 at corresponding column and row coordinate.
 *
 * @params	- uint8_t	x				: the column index.
 * @params	- uint8_t	y				: the row index.
 * @return	- None
 */
void BLACKDOTS_setPoint(uint8_t x, uint8_t y) {
	if (x >= BLACKDOTS_MAXCOLS || y >= BLACKDOTS_MAXROWS) return;
	points[y] |= 1U << x;
}

/**
 * clears the value to 0 at corresponding column and row coordinate.
 *
 * @params	- uint8_t	x				: the column index.
 * @params	- uint8_t	y				: the row index.
 * @return	- None
 */
void BLACKDOTS_clearPoint(uint8_t x, uint8_t y) {
	if (x >= BLACKDOTS_MAXCOLS || y >= BLACKDOTS_MAXROWS) return;
	points[y] &= ~(1U << x);
}

/**
 * changes the value to a given state at corresponding column and row coordinate.
 *
 * @params	- uint8_t	x				: the column index.
 * @params	- uint8_t	y				: the row index.
 * @params	- bool		highLow			: the desired state of the given coordinate.
 * @return	- None
 */
void BLACKDOTS_changePoint(uint8_t x, uint8_t y, bool highLow) {
	if (x >= BLACKDOTS_MAXCOLS || y >= BLACKDOTS_MAXROWS) return;
	points[y] = (points[y] & ~(1U << x)) | (highLow << x);
}

/**
 * toggles the value at corresponding column and row coordinate.
 *
 * @params	- uint8_t	x				: the column index.
 * @params	- uint8_t	y				: the row index.
 * @return	- None
 */
void BLACKDOTS_togglePoint(uint8_t x, uint8_t y) {
	if (x >= BLACKDOTS_MAXCOLS || y >= BLACKDOTS_MAXROWS) return;
	points[y] ^= 1U << x;
}

/**
 * gets the value at a given column and row.
 *
 * @params	- uint8_t	x				: the column index.
 * @params	- uint8_t	y				: the row index.
 * @return	- bool						: the state of that coordinate.
 */
bool BLACKDOTS_getPoint(uint8_t x, uint8_t y) {
	if (x >= BLACKDOTS_MAXCOLS || y >= BLACKDOTS_MAXROWS) return false;
	return BLACKDOTS_getCol(BLACKDOTS_getRow(y), x);
}

/**
 * populates a blackDotCoordinate_s* with information relating to a given point.
 *
 * @params	- uint8_t				x				: the column index.
 * @params	- uint8_t				y				: the row index.
 * @params	- blackDotcoordinate_s*	dot				: an empty blackDotCoordinate_s* ready to be set with relevant information.
 * @return	- None
 */
void BLACKDOTS_getPointAsCoordinate(uint8_t x, uint8_t y, blackDotCoordinate_s* dot) {
	dot->row = y;
	dot->col = x;
	dot->value = BLACKDOTS_getPoint(x, y);
}

/**
 * Searches through the BLACKDOTS structure for set black dots (ie value = 1).
 * If it finds them, it populates the given pointer, otherwise it returns it with invalid information.
 *
 * @params	- blackDotCoordinate_s* 	dot		: the black dot ready to be filled with the next blackDot in the structure.
 * @return	- None
 */
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
	for (uint8_t x = 0; x < BLACKDOTS_MAXCOLS; x++) {
		for (uint8_t y = 0; y < BLACKDOTS_MAXROWS; y++) {
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
