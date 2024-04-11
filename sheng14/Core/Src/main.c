/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "tim.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "led.h"
#include "interrupt.h"
#include "stdio.h"
#include "badc.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
void key_proc(void);
void lcd_proc(void);
void led_proc(void);
void pa7_v_change(void);
void PWM_proc(void);
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
extern struct keys key[4];
extern uint frq_pa7;
uint16_t view=0;
bool mode=0;
uint N_change=0;
uint PWM=0;
float VOLT=0;
uint PARA_K=1;
uint PARA_R=1;
bool R_K_choice=0;
uint R_temp=1,K_temp=1;
double pa1_duty=10.0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

	
  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_TIM4_Init();
  MX_ADC2_Init();
  MX_TIM2_Init();
  MX_TIM17_Init();
  /* USER CODE BEGIN 2 */
  LCD_Init();					    	//LCD初始化

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  LED_dis(0x00);				//led初始化
  
  HAL_TIM_Base_Start_IT(&htim4);
  
  LCD_Clear(Black);
  LCD_SetBackColor(Black);
  LCD_SetTextColor(White);
  
  HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_2);		//TIM_PWM开启通道
  
  HAL_TIM_IC_Start_IT(&htim17,TIM_CHANNEL_1); 					//PA7频率测量开启通道
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	  lcd_proc();
	  key_proc();
	  led_proc();
	  pa7_v_change();
	  PWM_proc();
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = RCC_PLLM_DIV3;
  RCC_OscInitStruct.PLL.PLLN = 20;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void key_proc()								//按键控制函数
{
	if(key[0].single_flag==1)
	{
		view++;
		LCD_Clear(Black);
		if(view==3)view=0;
		if(view==1)R_K_choice=0;
		key[0].single_flag=0;
	}
	if(key[1].single_flag==1)
	{
		if(view==1)
		{
			R_K_choice=!R_K_choice;
		}
		if(view==0)
		{
			mode=!mode;
			N_change++;
		}
		
		key[1].single_flag=0;
	}
	if(key[2].single_flag==1)
	{
		if(view==1)
		{
			if(R_K_choice==0)
			{
				R_temp++;
				while(R_temp>10)R_temp=1;
			}
			if(R_K_choice==1)
			{
				K_temp++;
				while(K_temp>10)K_temp=1;
			}
		}
		
		key[2].single_flag=0;
	}
	if(key[3].single_flag==1)
	{
		if(view==1)
		{
			if(R_K_choice==0)
			{
				R_temp--;
				while(R_temp==0)R_temp=10;
			}
			if(R_K_choice==1)
			{
				K_temp--;
				while(K_temp==0)K_temp=10;
			}
		}
		
		key[3].single_flag=0;
	}
}

void lcd_proc()								//界面显示函数
{
	char text[30];
	if(view==0)																//数据显示界面（默认界面）
	{
		sprintf(text,"        DATA         ");								
		LCD_DisplayStringLine(Line1, (uint8_t *)text);
		if(mode==0)
		{
			sprintf(text,"     M=L            ");								
			LCD_DisplayStringLine(Line3, (uint8_t *)text);
		}
		if(mode==1)
		{
			sprintf(text,"     M=H            ");								
			LCD_DisplayStringLine(Line3, (uint8_t *)text);
		}
		sprintf(text,"     P=%.0lf%%             ",pa1_duty);								
		LCD_DisplayStringLine(Line4, (uint8_t *)text);
		sprintf(text,"     V=%.1f             ",VOLT);								
		LCD_DisplayStringLine(Line5, (uint8_t *)text);
		sprintf(text,"     V=%.2f             ",getADC(&hadc2));								
		LCD_DisplayStringLine(Line6, (uint8_t *)text);
	}
	if(view==1)																//参数界面			
	{
		sprintf(text,"        PARA         ");								
		LCD_DisplayStringLine(Line1, (uint8_t *)text);
		sprintf(text,"     R=%d            ",R_temp);								
		LCD_DisplayStringLine(Line3, (uint8_t *)text);
		sprintf(text,"     K=%d            ",K_temp);								
		LCD_DisplayStringLine(Line4, (uint8_t *)text);
	}
	if(view==2)																//统计界面
	{
		sprintf(text,"        RECD         ");								
		LCD_DisplayStringLine(Line1, (uint8_t *)text);
		sprintf(text,"     N=%d              ",N_change);								
		LCD_DisplayStringLine(Line3, (uint8_t *)text);
		sprintf(text,"     MH=             ");								
		LCD_DisplayStringLine(Line4, (uint8_t *)text);
		sprintf(text,"     ML=             ");								
		LCD_DisplayStringLine(Line5, (uint8_t *)text);
	}
}
void led_proc()								//led控制函数
{
	if(view==0)
	{
		LED_dis(0x01);
	}
	else
	{
		LED_dis(0x00);
	}
}
void pa7_v_change()
{
	if(view==0||view==2)
	{
		PARA_K=K_temp;
		PARA_R=R_temp;
	}
	VOLT=frq_pa7*2*3.14*PARA_R/(100*PARA_K);
}
void PWM_proc()
{
	pa1_duty=(getADC(&hadc2)*37.5)-27.5;
	if(getADC(&hadc2)<=1.0)pa1_duty=10;
	if(getADC(&hadc2)>=3.0)pa1_duty=85;
	__HAL_TIM_SetCompare(&htim2,TIM_CHANNEL_2,pa1_duty);
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
