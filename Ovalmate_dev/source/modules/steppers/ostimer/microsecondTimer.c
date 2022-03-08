
/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "microsecondTimer.h"

/*******************************************************************************
 * Variables
 ******************************************************************************/
static ctimer_match_config_t matchConfig;
uint64_t usCount;

/*******************************************************************************
 * Code
 ******************************************************************************/

void USTIMER_fiveUSCallback(uint32_t flags) {
    usCount += 5;		// this interrupt triggers every 5 microseconds.
    					// a cpu at 150MHz will be interrupted every 750 clocks here.
}

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


	CTIMER_RegisterCallBack(USTIMER_CTIMER, USTIMER_fiveUSCallback, kCTIMER_SingleCallback);
	CTIMER_SetupMatch(USTIMER_CTIMER, USTIMER_MATCH_OUT, &matchConfig);
	CTIMER_StartTimer(USTIMER_CTIMER);
}
