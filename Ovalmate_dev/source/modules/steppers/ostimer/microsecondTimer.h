/*
 * Copyright (c) 2016, Freescale Semiconductor, Inc.
 * Copyright 2016-2020 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "fsl_ctimer.h"
/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define USTIMER_CTIMER				CTIMER0								/* Timer 0 */
#define USTIMER_MATCH_OUT			kCTIMER_Match_0						/* Match output 0 */
#define USTIMER_TIMERCLK_FREQ		CLOCK_GetCTimerClkFreq(0U)			// frequency of clk driving CTIMER
#define USTIMER_FREQ				200000U								// frequency of match occuring
#define USTIMER_MATCHVALUE			USTIMER_CLK_FREQ / USTIMER_FREQ		// match value for timer

/*******************************************************************************
 * Variables
 ******************************************************************************/
static ctimer_match_config_t matchConfig;
extern uint64_t usCount;

/*******************************************************************************
 * Code
 ******************************************************************************/

void USTIMER_FiveUSCallback(uint32_t flags)
{
    usCount += 5;		// this interrupt triggers every 5 microseconds.
    					// a cpu at 150MHz will be interrupted every 750 clocks here.
}

/*!
 * @brief Main function
 */
void initializeUSTIMER(void) {
    ctimer_config_t config;
    usCount = 0; 				// set microsecond count

    // get default config and run it // prescale default = 1
    CTIMER_GetDefaultConfig(&config);
    CTIMER_Init(USTIMER_CTIMER, &config);

    // set match value and start timer
	matchConfig.enableCounterReset = true;
	matchConfig.enableCounterStop  = false;
	matchConfig.matchValue         = USTIMER_MATCHVALUE;
	//matchConfig.outControl         = kCTIMER_Output_Toggle; // dont care
	//matchConfig.outPinInitState    = false; // dont care
	matchConfig.enableInterrupt    = true;


	CTIMER_RegisterCallBack(USTIMER_CTIMER, USTIMER_FiveUSCallback, kCTIMER_SingleCallback);
	CTIMER_SetupMatch(USTIMER_CTIMER, USTIMER_MATCH_OUT, &matchConfig);
	CTIMER_StartTimer(USTIMER_CTIMER);
}
