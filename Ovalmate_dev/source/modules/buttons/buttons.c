/**
 * Title: OvalMate_buttons_module_DevBoard
 * Version: 0.0.1
 * Filename: buttons.c
 * Authors: Alex Wissing
 * Purpose of Program: Initialize button interrupts
 * How Program is Run on Target System:
 * Date Started: 2/28/2022
 * Update History:
 *	- 3/3/2022:
 *		Added settable function callbacks for button interrupts
 *	- 3/14/2022:
 *		Reformatted function names to better represent module approach
 *	- 3/19/2022:
 *		Added comments to all functions in preparation for Software Design Review
 */

#include "buttons.h"

// Variables
void (*emergencyBumpCallback)(pint_pin_int_t, uint32_t);
void (*rightBumpCallback)(pint_pin_int_t, uint32_t);
void (*leftBumpCallback)(pint_pin_int_t, uint32_t);
void (*upBumpCallback)(pint_pin_int_t, uint32_t);
void (*downBumpCallback)(pint_pin_int_t, uint32_t);
pinInformation_s up = { .port = 0, .pin = 9 }; // demo green led port 1 pin 7
pinInformation_s right = { .port = 1, .pin = 25 };
pinInformation_s left = { .port = 1, .pin = 26 };
pinInformation_s down = { .port = 1, .pin = 27 };
gpio_pin_config_t bumpConfig = { .pinDirection = kGPIO_DigitalInput };
// End Variables

// Functions
/**
 * Initialize the pins used by the bump switch interrupts.
 * The bump switches utilize pin interrupts 0-4. Each are triggered on rising edge.
 * Run this once before for the bump switch interrupts to work.
 *
 * @params	- None
 * @return	- None
 */
void BUTTONS_assignPinsToInterrupts() {

	GPIO_PinInit(GPIO, up.port, up.pin, &bumpConfig);
	GPIO_PinInit(GPIO, right.port, right.pin, &bumpConfig);
	GPIO_PinInit(GPIO, left.port, left.pin, &bumpConfig);
	GPIO_PinInit(GPIO, down.port, down.pin, &bumpConfig);

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
	PINT_PinInterruptConfig(PINT, kPINT_PinInt0, kPINT_PinIntEnableRiseEdge, BUTTONS_buttonEmergencyCallback);
	/* Enable callbacks for PINT0 by Index */
	PINT_EnableCallbackByIndex(PINT, kPINT_PinInt0);

	/* Setup Pin Interrupt 1 for rising edge */
	PINT_PinInterruptConfig(PINT, kPINT_PinInt1, kPINT_PinIntEnableRiseEdge, BUTTONS_bumpRightCallback);
	/* Enable callbacks for PINT1 by Index */
	PINT_EnableCallbackByIndex(PINT, kPINT_PinInt1);

	/* Setup Pin Interrupt 2 for rising edge */
	PINT_PinInterruptConfig(PINT, kPINT_PinInt2, kPINT_PinIntEnableRiseEdge, BUTTONS_bumpLeftCallback);
	/* Enable callbacks for PINT2 by Index */
	PINT_EnableCallbackByIndex(PINT, kPINT_PinInt2);

	/* Setup Pin Interrupt 3 for rising edge */
	PINT_PinInterruptConfig(PINT, kPINT_PinInt3, kPINT_PinIntEnableRiseEdge, BUTTONS_bumpDownCallback);
	/* Enable callbacks for PINT3 by Index */
	PINT_EnableCallbackByIndex(PINT, kPINT_PinInt3);

	/* Setup Pin Interrupt 4 for rising edge */
	PINT_PinInterruptConfig(PINT, kPINT_PinInt4, kPINT_PinIntEnableRiseEdge, BUTTONS_bumpUpCallback);
	/* Enable callbacks for PINT4 by Index */
	PINT_EnableCallbackByIndex(PINT, kPINT_PinInt4);

	//PRINTF("\r\nPINT Pin Interrupt events are configured\r\n");
	//PRINTF("\r\nPress corresponding switches to generate events\r\n");
}

/**
 * This callback is used to signal the emergency button has been pressed.
 * A custom callback can be called alongside the default behavior by setting the emergencyBumpCallback variable.
 *
 * @params	- pint_pin_int_t pintr			: an identifier for which pin interrupt is triggering this function call. (always kPINT_PinInt0)
 * 			- uint32_t pmatch_status		: I actually don't know what this is. Should probably print it out and try to figure it out.
 * @return	- None
 */
void BUTTONS_buttonEmergencyCallback(pint_pin_int_t pintr, uint32_t pmatch_status) {
	PRINTF("\r\n\r\n emergency button hit");

	if (emergencyBumpCallback != NULL)
		emergencyBumpCallback(pintr, pmatch_status);
}

/**
 * This callback is used to signal the right button has been pressed.
 * A custom callback can be called alongside the default behavior by setting the rightBumpCallback variable.
 *
 * @params	- pint_pin_int_t pintr			: an identifier for which pin interrupt is triggering this function call. (always kPINT_PinInt1)
 * 			- uint32_t pmatch_status		: I actually don't know what this is. Should probably print it out and try to figure it out.
 * @return	- None
 */
void BUTTONS_bumpRightCallback(pint_pin_int_t pintr, uint32_t pmatch_status) {
	PRINTF("\r\n\r\n right button hit");
	if (rightBumpCallback != NULL && BUTTONS_debounceButton(&right)) {
		PRINTF("button right is a valid trigger \r\n");
		rightBumpCallback(pintr, pmatch_status);
	}
}

/**
 * This callback is used to signal the left button has been pressed.
 * A custom callback can be called alongside the default behavior by setting the leftBumpCallback variable.
 *
 * @params	- pint_pin_int_t pintr			: an identifier for which pin interrupt is triggering this function call. (always kPINT_PinInt2)
 * 			- uint32_t pmatch_status		: I actually don't know what this is. Should probably print it out and try to figure it out.
 * @return	- None
 */
void BUTTONS_bumpLeftCallback(pint_pin_int_t pintr, uint32_t pmatch_status) {
	PRINTF("\r\n\r\n left button hit");
	if (leftBumpCallback != NULL && BUTTONS_debounceButton(&left)) {
		PRINTF("button left is a valid trigger \r\n");
		leftBumpCallback(pintr, pmatch_status);
	}
}

/**
 * This callback is used to signal the up button has been pressed.
 * A custom callback can be called alongside the default behavior by setting the upBumpCallback variable.
 *
 * @params	- pint_pin_int_t pintr			: an identifier for which pin interrupt is triggering this function call. (always kPINT_PinInt3)
 * 			- uint32_t pmatch_status		: I actually don't know what this is. Should probably print it out and try to figure it out.
 * @return	- None
 */
void BUTTONS_bumpUpCallback(pint_pin_int_t pintr, uint32_t pmatch_status) {
	PRINTF("\r\n\r\n up button hit");
	if (upBumpCallback != NULL && BUTTONS_debounceButton(&up)) {
		PRINTF("button up is a valid trigger \r\n");
		upBumpCallback(pintr, pmatch_status);
	}
}

/**
 * This callback is used to signal the down button has been pressed.
 * A custom callback can be called alongside the default behavior by setting the downBumpCallback variable.
 *
 * @params	- pint_pin_int_t pintr			: an identifier for which pin interrupt is triggering this function call. (always kPINT_PinInt4)
 * 			- uint32_t pmatch_status		: I actually don't know what this is. Should probably print it out and try to figure it out.
 * @return	- None
 */
void BUTTONS_bumpDownCallback(pint_pin_int_t pintr, uint32_t pmatch_status) {
	PRINTF("\r\n down button hit \r\n");
	if (downBumpCallback != NULL && BUTTONS_debounceButton(&down)) {
		PRINTF("button down is a valid trigger \r\n");
		downBumpCallback(pintr, pmatch_status);
	}
}

bool BUTTONS_debounceButton(pinInformation_s* pinInfo) {
	bool r = GPIO_PinRead(GPIO, pinInfo->port, pinInfo->pin);
	if (r) {
		delay500us();
		return GPIO_PinRead(GPIO, pinInfo->port, pinInfo->pin);
	}
	return r; // r guaranteed to be false
}
// End Functions
