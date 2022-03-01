/**
 * Title: OvalMate_sevenSegmentLED_module_DevBoard
 * Version: 0.0.1
 * Filename: LPC55S16_Project_main_dev.c
 * Authors: Alex Wissing
 * Purpose of Program: Manage 7-segment LEDs
 * How Program is Run on Target System:
 * Date Started: 2/28/2022
 * Update History:
 *
 */

#include "buttons.h"

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

	/* Initialize PINT */
	PINT_Init(PINT);

	/* Setup Pin Interrupt 0 for rising edge */
	PINT_PinInterruptConfig(PINT, kPINT_PinInt0, kPINT_PinIntEnableRiseEdge, buttonEmergencyCallback);
	/* Enable callbacks for PINT0 by Index */
	PINT_EnableCallbackByIndex(PINT, kPINT_PinInt0);

	/* Setup Pin Interrupt 1 for falling edge */
	PINT_PinInterruptConfig(PINT, kPINT_PinInt1, kPINT_PinIntEnableRiseEdge, bumpRightCallback);
	/* Enable callbacks for PINT1 by Index */
	PINT_EnableCallbackByIndex(PINT, kPINT_PinInt1);

	/* Setup Pin Interrupt 2 for falling edge */
	PINT_PinInterruptConfig(PINT, kPINT_PinInt2, kPINT_PinIntEnableRiseEdge, bumpLeftCallback);
	/* Enable callbacks for PINT2 by Index */
	PINT_EnableCallbackByIndex(PINT, kPINT_PinInt2);

	/* Setup Pin Interrupt 3 for falling edge */
	PINT_PinInterruptConfig(PINT, kPINT_PinInt3, kPINT_PinIntEnableRiseEdge, bumpDownCallback);
	/* Enable callbacks for PINT3 by Index */
	PINT_EnableCallbackByIndex(PINT, kPINT_PinInt3);

	/* Setup Pin Interrupt 4 for falling edge */
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
	//PRINTF("\f\r\nPINT Pin Interrupt %d event detected. EMERGENCY SWITCH", pintr);
}

/*!
 * @brief Call back for PINT Pin interrupt 1.
 */
void bumpRightCallback(pint_pin_int_t pintr, uint32_t pmatch_status)
{
	//PRINTF("\f\r\nPINT Pin Interrupt %d event detected. Right", pintr);
}

/*!
 * @brief Call back for PINT Pin interrupt 2.
 */
void bumpLeftCallback(pint_pin_int_t pintr, uint32_t pmatch_status)
{
	//PRINTF("\f\r\nPINT Pin Interrupt %d event detected. Left", pintr);
}

/*!
 * @brief Call back for PINT Pin interrupt 3.
 */
void bumpUpCallback(pint_pin_int_t pintr, uint32_t pmatch_status)
{
	//PRINTF("\f\r\nPINT Pin Interrupt %d event detected. Up", pintr);
}

/*!
 * @brief Call back for PINT Pin interrupt 4.
 */
void bumpDownCallback(pint_pin_int_t pintr, uint32_t pmatch_status)
{
	//PRINTF("\f\r\nPINT Pin Interrupt %d event detected. Down", pintr);
}