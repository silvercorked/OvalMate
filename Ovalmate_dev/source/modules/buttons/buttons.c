/**
 * Title: OvalMate_buttons_module_DevBoard
 * Version: 0.0.1
 * Filename: LPC55S16_Project_main_dev.c
 * Authors: Alex Wissing
 * Purpose of Program: Initialize button interrupts
 * How Program is Run on Target System:
 * Date Started: 2/28/2022
 * Update History:
 *	- 3/3/2022:
 *		Added settable function callbacks for button interrupts
 */

#include "buttons.h"

void (*emergencyBumpCallback)(pint_pin_int_t, uint32_t);
void (*rightBumpCallback)(pint_pin_int_t, uint32_t);
void (*leftBumpCallback)(pint_pin_int_t, uint32_t);
void (*upBumpCallback)(pint_pin_int_t, uint32_t);
void (*downBumpCallback)(pint_pin_int_t, uint32_t);

void assignPinsToInterrupts() {
	/* Connect trigger sources to PINT */
	INPUTMUX_Init(INPUTMUX);
	INPUTMUX_AttachSignal(INPUTMUX, kPINT_PinInt1, PINT_PIN_INT1_SRC); // right bump switch
	INPUTMUX_AttachSignal(INPUTMUX, kPINT_PinInt2, PINT_PIN_INT2_SRC); // left bump switch
	INPUTMUX_AttachSignal(INPUTMUX, kPINT_PinInt3, PINT_PIN_INT3_SRC); // down bump switch
	INPUTMUX_AttachSignal(INPUTMUX, kPINT_PinInt4, PINT_PIN_INT4_SRC); // up bump switch
	INPUTMUX_AttachSignal(INPUTMUX, kPINT_PinInt0, PINT_PIN_INT0_SRC); // emergency bump switch
	/* Turnoff clock to inputmux to save power. Clock is only needed to make changes */
	INPUTMUX_Deinit(INPUTMUX);

	//PRINTF("\f\r\nPINT Pin interrupt example\r\n");

	/* Setup Pin Interrupt 0 for rising edge */
	PINT_PinInterruptConfig(PINT, kPINT_PinInt0, kPINT_PinIntEnableRiseEdge, buttonEmergencyCallback);
	/* Enable callbacks for PINT0 by Index */
	PINT_EnableCallbackByIndex(PINT, kPINT_PinInt0);

	/* Setup Pin Interrupt 1 for rising edge */
	PINT_PinInterruptConfig(PINT, kPINT_PinInt1, kPINT_PinIntEnableRiseEdge, bumpRightCallback);
	/* Enable callbacks for PINT1 by Index */
	PINT_EnableCallbackByIndex(PINT, kPINT_PinInt1);

	/* Setup Pin Interrupt 2 for rising edge */
	PINT_PinInterruptConfig(PINT, kPINT_PinInt2, kPINT_PinIntEnableRiseEdge, bumpLeftCallback);
	/* Enable callbacks for PINT2 by Index */
	PINT_EnableCallbackByIndex(PINT, kPINT_PinInt2);

	/* Setup Pin Interrupt 3 for rising edge */
	PINT_PinInterruptConfig(PINT, kPINT_PinInt3, kPINT_PinIntEnableRiseEdge, bumpDownCallback);
	/* Enable callbacks for PINT3 by Index */
	PINT_EnableCallbackByIndex(PINT, kPINT_PinInt3);

	/* Setup Pin Interrupt 4 for rising edge */
	PINT_PinInterruptConfig(PINT, kPINT_PinInt4, kPINT_PinIntEnableRiseEdge, bumpUpCallback);
	/* Enable callbacks for PINT4 by Index */
	PINT_EnableCallbackByIndex(PINT, kPINT_PinInt4);

	//PRINTF("\r\nPINT Pin Interrupt events are configured\r\n");
	//PRINTF("\r\nPress corresponding switches to generate events\r\n");
}

/*!
 * @brief Call back for PINT Pin interrupt 0.
 */
void buttonEmergencyCallback(pint_pin_int_t pintr, uint32_t pmatch_status)
{
	PRINTF("\r\n\r\n emergency button hit");
	if (emergencyBumpCallback != NULL)
		emergencyBumpCallback(pintr, pmatch_status);
}

/*!
 * @brief Call back for PINT Pin interrupt 1.
 */
void bumpRightCallback(pint_pin_int_t pintr, uint32_t pmatch_status)
{
	PRINTF("\r\n\r\n right button hit");
	if (rightBumpCallback != NULL)
		rightBumpCallback(pintr, pmatch_status);
}

/*!
 * @brief Call back for PINT Pin interrupt 2.
 */
void bumpLeftCallback(pint_pin_int_t pintr, uint32_t pmatch_status)
{
	PRINTF("\r\n\r\n left button hit");
	if (leftBumpCallback != NULL)
		leftBumpCallback(pintr, pmatch_status);
}

/*!
 * @brief Call back for PINT Pin interrupt 3.
 */
void bumpUpCallback(pint_pin_int_t pintr, uint32_t pmatch_status)
{
	PRINTF("\r\n\r\n up button hit");
	if (upBumpCallback != NULL)
		upBumpCallback(pintr, pmatch_status);
}

/*!
 * @brief Call back for PINT Pin interrupt 4.
 */
void bumpDownCallback(pint_pin_int_t pintr, uint32_t pmatch_status)
{
	PRINTF("\r\n\r\n down button hit");
	if (downBumpCallback != NULL)
		downBumpCallback(pintr, pmatch_status);
}
