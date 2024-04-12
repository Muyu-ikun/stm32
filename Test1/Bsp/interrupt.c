#include "interrupt.h"
#include "usart.h"

struct keys key[4]={0,0,0,0};
uint led_time=0;
bool led7_flag=0;

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim->Instance==TIM4)
	{
		if(led_time++==100)
		{
			led7_flag=1;
			led_time=0;
		}
		key[0].key_sta=HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_0);
		key[1].key_sta=HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_1);
		key[2].key_sta=HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_2);
		key[3].key_sta=HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0);
		
		for(int i=0;i<4;i++)
		{
			switch(key[i].judge_sta)
			{
				case 0:
				{
					if(key[i].key_sta==0) 
					{
						key[i].judge_sta=1;
						key[i].key_time=0;
					}
				}
				break;
				case 1:
				{
					if(key[i].key_sta==0) 
					{
						key[i].judge_sta=2;
						
					}
					else key[i].judge_sta=0;
					
				}
				break;
				case 2:
				{
					if(key[i].key_sta==1)				//ÅÐ¶Ïµ¯Æð
					{
						key[i].judge_sta=0;
						key[i].single_flag=1;
						if(key[i].key_time<200)
						{
							key[i].long_flag=0;
							key[i].key_time=0;
						}
					else if(key[i].key_time>200)
					{
						key[i].long_flag=1;
					}
					}
					else
					{
						key[i].key_time++;
//						if(key[i].key_time>200)
//						{
//							key[i].long_flag=1;
//							key[i].key_time=0;
//						}
					}
				}
				break;
				
			}
		}
	}
}

uint ccrl_val1=0;
uint ccrl_val2=0;
uint frq1=0;
uint frq2=0;
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
	if(htim->Instance==TIM2)
	{
		ccrl_val1=HAL_TIM_ReadCapturedValue(htim,TIM_CHANNEL_1);
		__HAL_TIM_SetCounter(htim,0);
		frq1=(80000000/80)/ccrl_val1;
		HAL_TIM_IC_Start(htim,TIM_CHANNEL_1);
	}
	if(htim->Instance==TIM3)
	{
		ccrl_val2=HAL_TIM_ReadCapturedValue(htim,TIM_CHANNEL_1);
		__HAL_TIM_SetCounter(htim,0);
		frq2=(80000000/80)/ccrl_val2;
		HAL_TIM_IC_Start(htim,TIM_CHANNEL_1);
	}
}

char rx_data[30];
uint8_t rx_dat;
uchar rx_pointer;

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *hurat)
{
	rx_data[rx_pointer++]=rx_dat;
	HAL_UART_Receive_IT(&huart1,&rx_dat,1);
}



