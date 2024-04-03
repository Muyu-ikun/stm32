#ifndef __INTERRUPT_H__
#define __INTERRUPT_H__

#include "main.h"
#include "stdbool.h"
struct keys
{
	uint8_t judge_sta;
	uint16_t key_time;
	bool key_sta;
	bool single_flag;
	bool long_flag;
};

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim);
#endif

