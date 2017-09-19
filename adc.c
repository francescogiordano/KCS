#include "adc.h"

#include "delay.h"
#include "nrf_gpio.h"

void InitAdc(uint8_t adcPin) {
	uint32_t pin = 0;

	if (adcPin == 2) {
		pin = ADC_CONFIG_PSEL_AnalogInput3;
	}
	else {
		pin = ADC_CONFIG_PSEL_Disabled;
	}

	//Disable interrupt ADC
	NRF_ADC->INTENSET = (ADC_INTENSET_END_Disabled << ADC_INTENSET_END_Pos);

	//Config ADC
	NRF_ADC->CONFIG = (ADC_CONFIG_EXTREFSEL_None						<< ADC_CONFIG_EXTREFSEL_Pos)	// Bits 17..16 : ADC external reference pin selection.
					| (ADC_CONFIG_PSEL_AnalogInput3						<< ADC_CONFIG_PSEL_Pos)			// Use pin PO.02/AN3 as analog input.
					| (ADC_CONFIG_REFSEL_VBG							<< ADC_CONFIG_REFSEL_Pos)		// Use internal 1.2V bandgap voltage as reference for conversion.
					| (ADC_CONFIG_INPSEL_AnalogInputOneThirdPrescaling	<< ADC_CONFIG_INPSEL_Pos)		// Analog input specified by PSEL with 1/3 prescaling used as input for the conversion.
					| (ADC_CONFIG_RES_8bit								<< ADC_CONFIG_RES_Pos);			//	8bit ADC resolution
}
void EnableAdc() {
	//Enable ADC
	NRF_ADC->ENABLE = ADC_ENABLE_ENABLE_Enabled;
}
void DisableAdc() {
	//Disable ADC
	NRF_ADC->ENABLE = ADC_ENABLE_ENABLE_Disabled;
}
uint16_t GetAdcValue() {
	uint32_t result = 0x02;

	//NRF_ADC->POWER = ADC_POWER_POWER_Enabled;

	//Enable ADC
	//NRF_ADC->ENABLE = ADC_ENABLE_ENABLE_Enabled;
	EnableAdc();

	delay_ms(50);

	// start ADC conversion
	NRF_ADC->TASKS_START = 1;

	// wait for conversion to end
	while (!NRF_ADC->EVENTS_END){}

	NRF_ADC->EVENTS_END = 0;

	//Save your ADC result
	result = NRF_ADC->RESULT;

	//Use the STOP task to save current. Workaround for PAN_028 rev1.1 anomaly 1.
	NRF_ADC->TASKS_STOP = 1;

	//Disable ADC
	//NRF_ADC->ENABLE = ADC_ENABLE_ENABLE_Disabled;
	DisableAdc();

	return result;
}
