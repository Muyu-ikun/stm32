#include "badc.h"

double getADC(ADC_HandleTypeDef *pin)
{
	uint32_t adc;
    HAL_ADC_Start (pin);
	adc = HAL_ADC_GetValue(pin);
	return adc*3.3/4027;
}


