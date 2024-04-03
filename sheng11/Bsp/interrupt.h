#ifndef _INTERRUPT_H_
#define _INTERRUPT_H_
#include "main.h"
#include "stdbool.h"

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim);


struct keys
{
    uchar judge_sta;
    uchar key_sta;
    bool singel_flag;
};



#endif

