/**
 * Title: OvalMate_irSensor_module_DevBoard
 * Version: 0.0.1
 * Filename: LPC55S16_Project_main_dev.c
 * Authors: Alex Wissing
 * Purpose of Program: Manage IR sensor
 * How Program is Run on Target System:
 * Date Started: 2/28/2022
 * Update History:
 *	- 2/28/2022
 *		Moved this into separate "modules"
 *	- 3/7/2022:
 *		Changed ADC pin. ADC0_8 is the negative side of channel 0 of the ADC. It won't work, but it's positive terminal can.
 *	- 3/8/2022:
 *		Added definition safeguards to avoid extra imports or duplicate declarations
 *	- 3/14/2022:
 *		Reformatted function names to better fit with modular approach
 */

#ifndef IRSENSOR_H
#define IRSENSOR_H

// Includes
#include "fsl_lpadc.h"
#include "fsl_power.h"
#include "fsl_anactrl.h"
// End Includes

// Definitions
#define LPADC_BASE						ADC0
#define LPADC_USER_CHANNEL				0U
#define LPADC_USER_CMDID				1U /* CMD1 */
#define LPADC_VREF_SOURCE				kLPADC_ReferenceVoltageAlt2	// 1 is internal VDD as ref, 2 is external ref
										// https://community.nxp.com/t5/LPC-Microcontrollers/LPC55S69-adc-reference-voltages/m-p/984941
#define LPADC_DO_OFFSET_CALIBRATION		true
#define LPADC_USE_HIGH_RESOLUTION		true
// End Definitions

// Extern Variables
extern const uint32_t g_LpadcFullRange;
extern const uint32_t g_LpadcResultShift;
extern lpadc_config_t mLpadcConfigStruct;
extern lpadc_conv_trigger_config_t mLpadcTriggerConfigStruct;
extern lpadc_conv_command_config_t mLpadcCommandConfigStruct;
extern lpadc_conv_result_t mLpadcResultConfigStruct;
// End Extern Variables

// Prototypes
uint32_t IRSENSOR_getADCValue(void);
void IRSENSOR_initializeADC(void);
double IRSENSOR_readAvgADC(uint32_t);
// End Prototypes

#endif
