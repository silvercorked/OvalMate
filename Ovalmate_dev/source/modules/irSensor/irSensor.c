/**
 * Title: OvalMate_ir sensor_module_DevBoard
 * Version: 0.0.1
 * Filename: LPC55S16_Project_main_dev.c
 * Authors: Alex Wissing
 * Purpose of Program: Manage IR sensor
 * How Program is Run on Target System:
 * Date Started: 2/28/2022
 * Update History:
 *	- 2/28/2022
 *		Moved this into separate "modules"
 *	- 3/9/2022:
 *		Removed while loop and commented out code from SDK example
 *	- 3/14/2022:
 *		Reformatted function names to better fit with modular approach
 *	- 3/17/2022:
 *		Fixed IR sensor and even found out how to run it through the correct pin (ADC0_8 ie ADC0_0 side b)
 *	- 3/19/2022:
 *		Added comments to all functions in preparation for Software Design Review
 */

#include "irSensor.h"

// Variables
#if (defined(LPADC_USE_HIGH_RESOLUTION) && LPADC_USE_HIGH_RESOLUTION)
const uint32_t g_LpadcFullRange   = 65536U;
const uint32_t g_LpadcResultShift = 0U;
#else
const uint32_t g_LpadcFullRange   = 4096U;
const uint32_t g_LpadcResultShift = 3U;
#endif /* LPADC_USE_HIGH_RESOLUTION */
lpadc_config_t mLpadcConfigStruct;
lpadc_conv_trigger_config_t mLpadcTriggerConfigStruct;
lpadc_conv_command_config_t mLpadcCommandConfigStruct;
lpadc_conv_result_t mLpadcResultConfigStruct;
// End Variables

/**
 * Causes the ADC channel (specified by definitions) to do a software trigger to capture ADC input voltage.
 *
 * @params	- None
 * @return	- uint32_t	: ADC input voltage. Mapped from 0V-3.3V -> 0-(2^16) (ADC is 16 bits)
 */
uint32_t IRSENSOR_getADCValue() {
	LPADC_DoSoftwareTrigger(LPADC_BASE, 1U); /* 1U is trigger0 mask. */
	#if (defined(FSL_FEATURE_LPADC_FIFO_COUNT) && (FSL_FEATURE_LPADC_FIFO_COUNT == 2U))
	while (!LPADC_GetConvResult(LPADC_BASE, &mLpadcResultConfigStruct, 0U))
	#else
	while (!LPADC_GetConvResult(DEMO_LPADC_BASE, &mLpadcResultConfigStruct))
	#endif /* FSL_FEATURE_LPADC_FIFO_COUNT */
	{
	}
	//PRINTF("ADC value: %d\r\n", ((mLpadcResultConfigStruct.convValue) >> g_LpadcResultShift));
	return ((mLpadcResultConfigStruct.convValue) >> g_LpadcResultShift);
}

/**
 * reads and averages the ADC output a given number of times.
 *
 * @params	- uint32_t samples	: number of times the ADC should be polled.
 * @return	- double			: ADC input voltage averaged over a period (samples / 48MHz). 48MHz is the frequency of the ADC.
 */
double IRSENSOR_readAvgADC(uint32_t samples) {
	uint32_t sum = 0;
	for (uint32_t i = 0; i < samples; i++)
		sum += IRSENSOR_getADCValue();
	return sum / ((double) samples);	 // returns double
}

/**
 * Initialize the ADC & setup IRSensor's ADC channel.
 *
 * Run this once before using IRSensor. No need to call again.
 *
 * @params	- None
 * @return	- None
 */
void IRSENSOR_initializeADC() {
    /* Disable LDOGPADC power down */
    POWER_DisablePD(kPDRUNCFG_PD_LDOGPADC);

    ANACTRL_Init(ANACTRL);
    ANACTRL_EnableVref1V(ANACTRL, true);

    //PRINTF("LPADC Polling Example\r\n");

    LPADC_GetDefaultConfig(&mLpadcConfigStruct);
    mLpadcConfigStruct.enableAnalogPreliminary = true;
#if defined(LPADC_VREF_SOURCE)
    mLpadcConfigStruct.referenceVoltageSource = LPADC_VREF_SOURCE;
#endif /* DEMO_LPADC_VREF_SOURCE */
#if defined(FSL_FEATURE_LPADC_HAS_CTRL_CAL_AVGS) && FSL_FEATURE_LPADC_HAS_CTRL_CAL_AVGS
    mLpadcConfigStruct.conversionAverageMode = kLPADC_ConversionAverage128;
#endif /* FSL_FEATURE_LPADC_HAS_CTRL_CAL_AVGS */
    LPADC_Init(LPADC_BASE, &mLpadcConfigStruct);

#if defined(FSL_FEATURE_LPADC_HAS_CTRL_CALOFS) && FSL_FEATURE_LPADC_HAS_CTRL_CALOFS
#if defined(FSL_FEATURE_LPADC_HAS_OFSTRIM) && FSL_FEATURE_LPADC_HAS_OFSTRIM
    /* Request offset calibration. */
#if defined(LPADC_DO_OFFSET_CALIBRATION) && LPADC_DO_OFFSET_CALIBRATION
    LPADC_DoOffsetCalibration(LPADC_BASE);
#else
    LPADC_SetOffsetValue(LPADC_BASE, LPADC_OFFSET_VALUE_A, LPADC_OFFSET_VALUE_B);
#endif /* DEMO_LPADC_DO_OFFSET_CALIBRATION */
#endif /* FSL_FEATURE_LPADC_HAS_OFSTRIM */
    /* Request gain calibration. */
    LPADC_DoAutoCalibration(LPADC_BASE);
#endif /* FSL_FEATURE_LPADC_HAS_CTRL_CALOFS */

#if (defined(FSL_FEATURE_LPADC_HAS_CFG_CALOFS) && FSL_FEATURE_LPADC_HAS_CFG_CALOFS)
    /* Do auto calibration. */
    LPADC_DoAutoCalibration(LPADC_BASE);
#endif /* FSL_FEATURE_LPADC_HAS_CFG_CALOFS */

    /* Set conversion CMD configuration. */
    LPADC_GetDefaultConvCommandConfig(&mLpadcCommandConfigStruct);
    mLpadcCommandConfigStruct.channelNumber = LPADC_USER_CHANNEL;
    mLpadcCommandConfigStruct.sampleChannelMode = kLPADC_SampleChannelSingleEndSideB;
#if defined(LPADC_USE_HIGH_RESOLUTION) && LPADC_USE_HIGH_RESOLUTION
    mLpadcCommandConfigStruct.conversionResolutionMode = kLPADC_ConversionResolutionHigh;
#endif /* DEMO_LPADC_USE_HIGH_RESOLUTION */
    LPADC_SetConvCommandConfig(LPADC_BASE, LPADC_USER_CMDID, &mLpadcCommandConfigStruct);

    /* Set trigger configuration. */
    LPADC_GetDefaultConvTriggerConfig(&mLpadcTriggerConfigStruct);
    mLpadcTriggerConfigStruct.targetCommandId       = LPADC_USER_CMDID;
    mLpadcTriggerConfigStruct.enableHardwareTrigger = false;
    LPADC_SetConvTriggerConfig(LPADC_BASE, 0U, &mLpadcTriggerConfigStruct); /* Configure the trigger0. */

/*
    PRINTF("ADC Full Range: %d\r\n", g_LpadcFullRange);
#if defined(FSL_FEATURE_LPADC_HAS_CMDL_CSCALE) && FSL_FEATURE_LPADC_HAS_CMDL_CSCALE
    if (kLPADC_SampleFullScale == mLpadcCommandConfigStruct.sampleScaleMode)
    '
    {
        PRINTF("Full channel scale (Factor of 1).\r\n");
    }
    else if (kLPADC_SamplePartScale == mLpadcCommandConfigStruct.sampleScaleMode)
    {
        PRINTF("Divided input voltage signal. (Factor of 30/64).\r\n");
    }
#endif
*/
}
