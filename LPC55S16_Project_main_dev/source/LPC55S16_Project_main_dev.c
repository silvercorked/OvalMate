/*
 * Title: OvalMate_DevBoard
 * Version: 0.0.1
 * Filename: LPC55S16_Project_main_dev.c
 * Authors: Alex Wissing
 * Purpose of Program: Operate a LPC55S16-EVK devlopment board
 * 		with the needed capabilities for the OvalMate project
 * 		(USB device, PWM to 2 steppers and a servo motor, Bump switch interrupts,
 * 		LED handling for error codes and status codes via 7-segment display).
 * How Program is Run on Target System:
 * Date Started: 2/05/2022
 * Update History:
 *
 */

/*
 * Copyright 2016-2022 NXP
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of NXP Semiconductor, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/* next tasks:
 * learn how to stop and track pwm sctimer
 * setup ctimer for servo
 * test bump switch interrupts
 * setup git repo
 *
 */

// CORE INCLUDES
#include <stdio.h>
#include "board.h"
#include "clock_config.h"
#include "fsl_debug_console.h"
#include "LPC55S16.h"
#include "peripherals.h"
#include "pin_mux.h"

// INTERRUPT INCLUDES
#include "fsl_pint.h"
#include "fsl_common.h"
#include "fsl_inputmux.h"
#include "fsl_power.h"

// GPIO INCLUDES (for 7Seg)
#include "fsl_gpio.h"

// ADC INCLUDES
#include "fsl_lpadc.h"

// PWM INCLUDES
#include "fsl_sctimer.h"

// CTIMER INCLUDES
#include <stdbool.h>
#include "fsl_ctimer.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

// Interrupt Global Definitions
#define PINT_PIN_INT1_SRC			kINPUTMUX_GpioPort1Pin25ToPintsel // right
#define PINT_PIN_INT2_SRC			kINPUTMUX_GpioPort1Pin26ToPintsel // left
#define PINT_PIN_INT3_SRC			kINPUTMUX_GpioPort1Pin27ToPintsel // down
#define PINT_PIN_INT4_SRC			kINPUTMUX_GpioPort0Pin9ToPintsel  // up
#define PINT_PIN_INT0_SRC			kINPUTMUX_GpioPort0Pin0ToPintsel  // emergency
// #define DEMO_SEC_PINT_PIN_INT0_SRC kINPUTMUX_GpioPort0Pin5ToPintSecsel
// End Interrupt Global Definitions

// 7Seg Global Definitions
#define APP_BOARD_TEST_LED_PORT 1U
#define APP_BOARD_TEST_LED_PIN  7U
#define APP_SW_PORT             BOARD_SW1_GPIO_PORT
#define APP_SW_PIN              BOARD_SW1_GPIO_PIN
// End 7Seg Global Definitions

// Stepper PWM Global Definitions
#define SCTIMER_CLK_FREQ        CLOCK_GetFreq(kCLOCK_BusClk)
#define X_SCTIMER_OUT			kSCTIMER_Out_0
#define Y_SCTIMER_OUT			kSCTIMER_Out_1
#define SETUP_STEPPER_X			0
#define SETUP_STEPPER_Y			1
#define SETUP_STEPPER_BOTH		2
// End Stepper PWM Global Definitions

// ADC Definitions
#define DEMO_LPADC_BASE						ADC0
#define DEMO_LPADC_USER_CHANNEL				0U
#define DEMO_LPADC_USER_CMDID				1U /* CMD1 */
#define DEMO_LPADC_VREF_SOURCE				kLPADC_ReferenceVoltageAlt1	// 1 is internal VDD as ref, 2 is external ref
											// https://community.nxp.com/t5/LPC-Microcontrollers/LPC55S69-adc-reference-voltages/m-p/984941
#define DEMO_LPADC_DO_OFFSET_CALIBRATION	true
#define DEMO_LPADC_USE_HIGH_RESOLUTION		true
// End ADC Definitions

// CTIMER Definitions
#define CTIMER          CTIMER3         /* Timer 3 */
#define CTIMER_MAT_OUT  kCTIMER_Match_2 /* Match output 2 */
#define CTIMER_CLK_FREQ CLOCK_GetCTimerClkFreq(3U) // 3U for clock 3
							// 1MHz => 50Hz = 20000 counts => 20ms
#define PENDOWN			10U	// 20% duty cycle = 2ms pulse = 90 degree
#define PENUP			7U	// 14% duty cylce = 1.5ms pulse = 0 degree
							// 10% duty cycle = 1ms pulse = -90 degree
							// https://content.instructables.com/ORIG/FA2/O1SS/J7ARLNBW/FA2O1SSJ7ARLNBW.pdf
#define SERVOFREQUENCYPWM 50U
#ifndef CTIMER_MAT_PWM_PERIOD_CHANNEL
#define CTIMER_MAT_PWM_PERIOD_CHANNEL kCTIMER_Match_3 // what's this for?
#endif
// End CTIMER Definitions

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

// 7Seg Prototypes
void set7Seg(
		bool legA,
		bool legB,
		bool legC,
		bool legD,
		bool legE,
		bool legF,
		bool legG
);
void initialize7SegLegs(void);
// End 7Seg Prototypes

// Interrupt Prototypes
void buttonEmergencyCallback(pint_pin_int_t pintr, uint32_t pmatch_status);
void bumpRightCallback(pint_pin_int_t pintr, uint32_t pmatch_status);
void bumpLeftCallback(pint_pin_int_t pintr, uint32_t pmatch_status);
void bumpUpCallback(pint_pin_int_t pintr, uint32_t pmatch_status);
void bumpDownCallback(pint_pin_int_t pintr, uint32_t pmatch_status);
void assignPinsToInterrupts(void);
// End Interrupt Prototypes

// ADC Prototypes
int getADCValue(void);
void initializeADC(void);
// End ADC Prototypes

// Stepper PWM Prototypes
void startStepperPWM(void);
status_t setupStepperPWM(int which);
void initializeStepperPWM(void);
// End Stepper PWM Prototypes

// CTIMER Prototypes
void startServoPWM(void);
void stopServoPWM(void);
void setupServoPWM(void);
status_t CTIMER_getPWMPeriodValue(
	uint32_t pwmFreqHz,
	uint8_t dutyCyclePercent,
	uint32_t timerClockHz
);
status_t CTIMER_updatePWMPulsePeriodValue(uint8_t dutyCyclePercent);
void updateServoPWMDutyCycle(uint8_t dutyCycle);
void initializeServoPWM(void);
// End CTIMER Prototypes

void delay(void)
{
    volatile uint32_t i = 0U;
    for (i = 0U; i < 20000000U; ++i)
    {
        __asm("NOP"); /* delay */
    }
}

/*******************************************************************************
 * Structs
 ******************************************************************************/

// 7Seg Structs
typedef struct __pin_information {
	int pin;
	int port;
} pinInformation;
// End 7Seg Structs

/*******************************************************************************
 * Global Variables
 ******************************************************************************/

// 7Seg Global Variables
pinInformation legA = { .port = 0, .pin = 4 }; // demo green led port 1 pin 7
pinInformation legB = { .port = 0, .pin = 5 };
pinInformation legC = { .port = 0, .pin = 6 };
pinInformation legD = { .port = 0, .pin = 8 };
pinInformation legE = { .port = 0, .pin = 13 };
pinInformation legF = { .port = 0, .pin = 14 };
pinInformation legG = { .port = 0, .pin = 18 };
gpio_pin_config_t sevenSegConfig = { .pinDirection = kGPIO_DigitalOutput, .outputLogic = 0};
// End 7Seg Global Variables

// Stepper PWM Global Variables
sctimer_config_t sctimerInfo;
sctimer_pwm_signal_param_t pwmParam;
uint32_t event;
uint32_t sctimerClock;
// End Stepper PWM Global Variables

// ADC Global Variables
#if (defined(DEMO_LPADC_USE_HIGH_RESOLUTION) && DEMO_LPADC_USE_HIGH_RESOLUTION)
const uint32_t g_LpadcFullRange   = 65536U;
const uint32_t g_LpadcResultShift = 0U;
#else
const uint32_t g_LpadcFullRange   = 4096U;
const uint32_t g_LpadcResultShift = 3U;
#endif /* DEMO_LPADC_USE_HIGH_RESOLUTION */

lpadc_config_t mLpadcConfigStruct;
lpadc_conv_trigger_config_t mLpadcTriggerConfigStruct;
lpadc_conv_command_config_t mLpadcCommandConfigStruct;
lpadc_conv_result_t mLpadcResultConfigStruct;
// End ADC Global Variables

// CTIMER Global Variables
volatile uint32_t g_pwmPeriod   = 0U;
volatile uint32_t g_pulsePeriod = 0U;
ctimer_config_t ctimer_config;
uint32_t ctimer_srcClock_Hz;
uint32_t ctimer_timerClock;
// End CTIMER Global Variables

/*
 * @brief   Application entry point.
 */
int main(void) {

	/* Init board hardware. */
	BOARD_InitBootPins();
	BOARD_InitBootClocks();
	BOARD_InitBootPeripherals();
#ifndef BOARD_INIT_DEBUG_CONSOLE_PERIPHERAL
	/* Init FSL debug console. */
	BOARD_InitDebugConsole();
#endif

	PRINTF("Hello World\n");

	initialize7SegLegs();
	assignPinsToInterrupts();
	initializeADC();
	initializeStepperPWM();
	setupStepperPWM(SETUP_STEPPER_BOTH);
	startStepperPWM();
	initializeServoPWM();
	setupServoPWM();
	startServoPWM();

	set7Seg(0, 0, 1, 0, 1, 0, 1);

	/* Force the counter to be placed into memory. */
	volatile static int i = 0 ;
	/* Enter an infinite loop, just incrementing a counter. */
	while(1) {
		i++ ;
		/* 'Dummy' NOP to allow source level single stepping of
			tight while() loop */
		delay();
		if (i % 2 == 1) {
			updateServoPWMDutyCycle(PENDOWN);
			PRINTF("\r\nPENDOWN");
			set7Seg(0, 0, 1, 0, 1, 0, 1);
		}
		else {
			updateServoPWMDutyCycle(PENUP);
			PRINTF("\r\nPENUP");
			set7Seg(1, 1, 0, 1, 0, 1, 0);
		}
		__asm volatile ("nop");
	}
	return 0;
}

/*!
 * @brief Call back for PINT Pin interrupt 0.
 */
void buttonEmergencyCallback(pint_pin_int_t pintr, uint32_t pmatch_status)
{
	PRINTF("\f\r\nPINT Pin Interrupt %d event detected. EMERGENCY SWITCH", pintr);
}

/*!
 * @brief Call back for PINT Pin interrupt 1.
 */
void bumpRightCallback(pint_pin_int_t pintr, uint32_t pmatch_status)
{
	PRINTF("\f\r\nPINT Pin Interrupt %d event detected. Right", pintr);
}

/*!
 * @brief Call back for PINT Pin interrupt 2.
 */
void bumpLeftCallback(pint_pin_int_t pintr, uint32_t pmatch_status)
{
	PRINTF("\f\r\nPINT Pin Interrupt %d event detected. Left", pintr);
}

/*!
 * @brief Call back for PINT Pin interrupt 3.
 */
void bumpUpCallback(pint_pin_int_t pintr, uint32_t pmatch_status)
{
	PRINTF("\f\r\nPINT Pin Interrupt %d event detected. Up", pintr);
}

/*!
 * @brief Call back for PINT Pin interrupt 4.
 */
void bumpDownCallback(pint_pin_int_t pintr, uint32_t pmatch_status)
{
	PRINTF("\f\r\nPINT Pin Interrupt %d event detected. Down", pintr);
}

void set7Seg(bool a, bool b, bool c, bool d, bool e, bool f, bool g) {
	GPIO_PinWrite(GPIO, legA.port, legA.pin, a);
	GPIO_PinWrite(GPIO, legB.port, legB.pin, b);
	GPIO_PinWrite(GPIO, legC.port, legC.pin, c);
	GPIO_PinWrite(GPIO, legD.port, legD.pin, d);
	GPIO_PinWrite(GPIO, legE.port, legE.pin, e);
	GPIO_PinWrite(GPIO, legF.port, legF.pin, f);
	GPIO_PinWrite(GPIO, legG.port, legG.pin, g);
}

void initialize7SegLegs() {
    /* Print a note to terminal. */
    PRINTF("\r\n GPIO Driver 7Seg\r\n");

    /* Init output LED GPIO. */
    //GPIO_PortInit(GPIO, legA.port);
    GPIO_PinInit(GPIO, legA.port, legA.pin, &sevenSegConfig);
    //GPIO_PortInit(GPIO, legB.port);
	GPIO_PinInit(GPIO, legB.port, legB.pin, &sevenSegConfig);
	//GPIO_PortInit(GPIO, legC.port);
	GPIO_PinInit(GPIO, legC.port, legC.pin, &sevenSegConfig);
	//GPIO_PortInit(GPIO, legD.port);
	GPIO_PinInit(GPIO, legD.port, legD.pin, &sevenSegConfig);
	//GPIO_PortInit(GPIO, legE.port);
	GPIO_PinInit(GPIO, legE.port, legE.pin, &sevenSegConfig);
	//GPIO_PortInit(GPIO, legF.port);
	GPIO_PinInit(GPIO, legF.port, legF.pin, &sevenSegConfig);
	//GPIO_PortInit(GPIO, legG.port);
	GPIO_PinInit(GPIO, legG.port, legG.pin, &sevenSegConfig);
}

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

	PRINTF("\f\r\nPINT Pin interrupt example\r\n");

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

	PRINTF("\r\nPINT Pin Interrupt events are configured\r\n");
	PRINTF("\r\nPress corresponding switches to generate events\r\n");
}

int getADCValue() {
	PRINTF("ADC Full Range: %d\r\n", g_LpadcFullRange);
#if defined(FSL_FEATURE_LPADC_HAS_CMDL_CSCALE) && FSL_FEATURE_LPADC_HAS_CMDL_CSCALE
	if (kLPADC_SampleFullScale == mLpadcCommandConfigStruct.sampleScaleMode)
	{
		PRINTF("Full channel scale (Factor of 1).\r\n");
	}
	else if (kLPADC_SamplePartScale == mLpadcCommandConfigStruct.sampleScaleMode)
	{
		PRINTF("Divided input voltage signal. (Factor of 30/64).\r\n");
	}
#endif

	PRINTF("Please press any key to get user channel's ADC value.\r\n");
	while (1)
	{
		GETCHAR();
		LPADC_DoSoftwareTrigger(DEMO_LPADC_BASE, 1U); /* 1U is trigger0 mask. */
#if (defined(FSL_FEATURE_LPADC_FIFO_COUNT) && (FSL_FEATURE_LPADC_FIFO_COUNT == 2U))
		while (!LPADC_GetConvResult(DEMO_LPADC_BASE, &mLpadcResultConfigStruct, 0U))
#else
		while (!LPADC_GetConvResult(DEMO_LPADC_BASE, &mLpadcResultConfigStruct))
#endif /* FSL_FEATURE_LPADC_FIFO_COUNT */
		{
		}

		PRINTF("ADC value: %d\r\n", ((mLpadcResultConfigStruct.convValue) >> g_LpadcResultShift));
		return ((mLpadcResultConfigStruct.convValue) >> g_LpadcResultShift);
	}
}

void startServoPWM() {
    CTIMER_StartTimer(CTIMER);
}

void stopServoPWM() {
	CTIMER_StopTimer(CTIMER);
}

void setupServoPWM() {
    /* Get the PWM period match value and pulse width match value of 20Khz PWM signal with 20% dutycycle */
    CTIMER_getPWMPeriodValue(SERVOFREQUENCYPWM, PENUP, ctimer_timerClock);
    CTIMER_SetupPwmPeriod(CTIMER, CTIMER_MAT_PWM_PERIOD_CHANNEL, CTIMER_MAT_OUT, g_pwmPeriod, g_pulsePeriod, false);
    PRINTF("\n starting servo pwm with period channel %d, output channel %d, period %d, pulsePeriod %d"
		, CTIMER_MAT_PWM_PERIOD_CHANNEL
		, CTIMER_MAT_OUT
		, g_pwmPeriod
		, g_pulsePeriod
	);
}

void updateServoPWMDutyCycle(uint8_t dutyCycle) {
	CTIMER_updatePWMPulsePeriodValue(dutyCycle);
	CTIMER_UpdatePwmPulsePeriod(CTIMER, CTIMER_MAT_OUT, g_pulsePeriod);
}

void initializeServoPWM() {
    /* CTimer3 counter uses the AHB clock, some CTimer1 modules use the Aysnc clock */
    ctimer_srcClock_Hz = CTIMER_CLK_FREQ;

    PRINTF("CTimer example to generate a PWM signal\r\n");

    CTIMER_GetDefaultConfig(&ctimer_config);
    ctimer_timerClock = ctimer_srcClock_Hz / (ctimer_config.prescale + 1);
    PRINTF("setting ctimer_timerClock to %d given that ctimer_srcClock_Hz is %d and prescale is %d"
    		, ctimer_timerClock
			, ctimer_srcClock_Hz
			, ctimer_config.prescale
	);
    CTIMER_Init(CTIMER, &ctimer_config);
}

status_t CTIMER_getPWMPeriodValue(uint32_t pwmFreqHz, uint8_t dutyCyclePercent, uint32_t timerClock_Hz) {
    /* Calculate PWM period match value */
    g_pwmPeriod = (timerClock_Hz / pwmFreqHz) - 1U;
    /* Calculate pulse width match value */
    g_pulsePeriod = (g_pwmPeriod + 1U) * (100 - dutyCyclePercent) / 100;

    return kStatus_Success;
}

status_t CTIMER_updatePWMPulsePeriodValue(uint8_t dutyCyclePercent)
{
    /* Calculate pulse width match value */
    g_pulsePeriod = (g_pwmPeriod + 1U) * (100 - dutyCyclePercent) / 100;

    return kStatus_Success;
}

void startStepperPWM() {
    /* Start the 32-bit unify timer */
    SCTIMER_StartTimer(SCT0, kSCTIMER_Counter_U);
}

status_t setupStepperPWM(int which) {
	/* Configure first PWM with frequency 2kHZ from first output */
	if (which == 0 || which == 2) {
		pwmParam.output           = X_SCTIMER_OUT;
		pwmParam.level            = kSCTIMER_HighTrue;
		pwmParam.dutyCyclePercent = 50;
		if (SCTIMER_SetupPwm(SCT0, &pwmParam, kSCTIMER_CenterAlignedPwm, 2000U, sctimerClock, &event) == kStatus_Fail)
		{
			return kStatus_Fail;
		}
	}

	/* Configure second PWM with different duty cycle but same frequency as before */
	if (which == 1 || which == 2) {
		pwmParam.output           = Y_SCTIMER_OUT;
		pwmParam.level            = kSCTIMER_HighTrue;
		pwmParam.dutyCyclePercent = 50;
		if (SCTIMER_SetupPwm(SCT0, &pwmParam, kSCTIMER_CenterAlignedPwm, 2000U, sctimerClock, &event) == kStatus_Fail)
		{
			return kStatus_Fail;
		}
	}
	return kStatus_Success;
}

void initializeStepperPWM() {
	sctimerClock = SCTIMER_CLK_FREQ;

	/* Print a note to terminal */
	PRINTF("\r\nSCTimer example to output 2 center-aligned PWM signals @ %d Hz\r\n", sctimerClock);
	PRINTF("\r\nProbe the signal using an oscilloscope\r\n");

	SCTIMER_GetDefaultConfig(&sctimerInfo);

	/* Initialize SCTimer module */
	SCTIMER_Init(SCT0, &sctimerInfo);
}

void initializeADC() {
	PRINTF("LPADC Polling Example\r\n");

	LPADC_GetDefaultConfig(&mLpadcConfigStruct);
	mLpadcConfigStruct.enableAnalogPreliminary = true;
#if defined(DEMO_LPADC_VREF_SOURCE)
	mLpadcConfigStruct.referenceVoltageSource = DEMO_LPADC_VREF_SOURCE;
#endif /* DEMO_LPADC_VREF_SOURCE */
#if defined(FSL_FEATURE_LPADC_HAS_CTRL_CAL_AVGS) && FSL_FEATURE_LPADC_HAS_CTRL_CAL_AVGS
	mLpadcConfigStruct.conversionAverageMode = kLPADC_ConversionAverage128;
#endif /* FSL_FEATURE_LPADC_HAS_CTRL_CAL_AVGS */
	LPADC_Init(DEMO_LPADC_BASE, &mLpadcConfigStruct);

#if defined(FSL_FEATURE_LPADC_HAS_CTRL_CALOFS) && FSL_FEATURE_LPADC_HAS_CTRL_CALOFS
#if defined(FSL_FEATURE_LPADC_HAS_OFSTRIM) && FSL_FEATURE_LPADC_HAS_OFSTRIM
	/* Request offset calibration. */
#if defined(DEMO_LPADC_DO_OFFSET_CALIBRATION) && DEMO_LPADC_DO_OFFSET_CALIBRATION
	LPADC_DoOffsetCalibration(DEMO_LPADC_BASE);
#else
	LPADC_SetOffsetValue(DEMO_LPADC_BASE, DEMO_LPADC_OFFSET_VALUE_A, DEMO_LPADC_OFFSET_VALUE_B);
#endif /* DEMO_LPADC_DO_OFFSET_CALIBRATION */
#endif /* FSL_FEATURE_LPADC_HAS_OFSTRIM */
	/* Request gain calibration. */
	LPADC_DoAutoCalibration(DEMO_LPADC_BASE);
#endif /* FSL_FEATURE_LPADC_HAS_CTRL_CALOFS */

#if (defined(FSL_FEATURE_LPADC_HAS_CFG_CALOFS) && FSL_FEATURE_LPADC_HAS_CFG_CALOFS)
	/* Do auto calibration. */
	LPADC_DoAutoCalibration(DEMO_LPADC_BASE);
#endif /* FSL_FEATURE_LPADC_HAS_CFG_CALOFS */

	/* Set conversion CMD configuration. */
	LPADC_GetDefaultConvCommandConfig(&mLpadcCommandConfigStruct);
	mLpadcCommandConfigStruct.channelNumber = DEMO_LPADC_USER_CHANNEL;
#if defined(DEMO_LPADC_USE_HIGH_RESOLUTION) && DEMO_LPADC_USE_HIGH_RESOLUTION
	mLpadcCommandConfigStruct.conversionResolutionMode = kLPADC_ConversionResolutionHigh;
#endif /* DEMO_LPADC_USE_HIGH_RESOLUTION */
	LPADC_SetConvCommandConfig(DEMO_LPADC_BASE, DEMO_LPADC_USER_CMDID, &mLpadcCommandConfigStruct);

	/* Set trigger configuration. */
	LPADC_GetDefaultConvTriggerConfig(&mLpadcTriggerConfigStruct);
	mLpadcTriggerConfigStruct.targetCommandId       = DEMO_LPADC_USER_CMDID;
	mLpadcTriggerConfigStruct.enableHardwareTrigger = false;
	LPADC_SetConvTriggerConfig(DEMO_LPADC_BASE, 0U, &mLpadcTriggerConfigStruct); /* Configurate the trigger0. */
}
