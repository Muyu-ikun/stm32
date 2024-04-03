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
#include "lcd.h"
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

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
extern struct keys key[];
bool view=0;

extern uint time_count;
double Vmax=3.0;
double Vmin=1.0;
double Vmax_temp=3.0;
double Vmin_temp=1.0;

uint time_start_flag=1;
uint time_stop_flag=1;

bool led1_flag=0;
bool led2_flag=0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void key_proc(void);
void lcd_proc(void);
void led_proc(void); 
void para_judge(void);
void adc_timer_proc(void);

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
  MX_TIM17_Init();
  MX_ADC2_Init();
  MX_TIM6_Init();
  /* USER CODE BEGIN 2 */

	LED_Dsp(0x00);                //led初始化
	LCD_Init();				//LCD初始化
	
	LCD_Clear(Black);		
    LCD_SetBackColor(Black);
    LCD_SetTextColor(White);

	HAL_TIM_Base_Start_IT(&htim4);			//打开tim4
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	  lcd_proc();
	  key_proc();
	  led_proc();
	  adc_timer_proc();
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
void key_proc(void)
{
	if(key[0].singel_flag==1)
	{
		view=!view;
		LCD_Clear(Black);
		key[0].singel_flag=0;
	}
	if(key[1].singel_flag==1)
	{
		if(view==1)
		{
			Vmax_temp+=0.1;
			if(Vmax_temp>3.3)Vmax_temp=0.0;
		}
		key[1].singel_flag=0;
	}
	if(key[2].singel_flag==1)
	{
		if(view==1)
		{
			Vmin_temp+=0.1;
			if(Vmin_temp>3.3)Vmin_temp=0.0;
		}
		key[2].singel_flag=0;
	}

}

void adc_timer_proc()
{
	double Vadc=getADC(&hadc2);
	if(Vadc<Vmin)
	{
		time_start_flag=0;
	}
	
	if(Vadc>Vmin&&Vadc<Vmax)
	{
		if(time_start_flag==0)				//开始计时
		{
		LCD_Clear(Black);
		time_count=0;
		HAL_TIM_Base_Start_IT(&htim6);
		led1_flag=1;
		time_start_flag=1;
		time_stop_flag=0;
		}
	}
	if(Vadc>Vmax)							//结束计时
	{
		if(time_stop_flag==0)
		{
		HAL_TIM_Base_Stop_IT(&htim6);
		led1_flag=0;
		time_stop_flag=1;
		}
	}


}

void led_proc(void)
{
	  if(led2_flag&&led1_flag)
		  LED_Dsp(0x03);
	  else if(led2_flag)
		  LED_Dsp(0x02);
	  else if(led1_flag)
		  LED_Dsp(0x01);
	  else if(led2_flag==0&&led1_flag==0)
		  LED_Dsp(0x00);
}

void para_judge()
{
	if(Vmax_temp-Vmin_temp>=1.0)
	{
		Vmax=Vmax_temp;
		Vmin=Vmin_temp;
		led2_flag=0;
	}
	else
	{
		led2_flag=1;
	}
}
void lcd_proc(void)
{

	char text[30];
	if(view==0)												//默认数据界面
	{
		para_judge();										//判断参数是否符合条件
		
		sprintf(text,"       Data");
	LCD_DisplayStringLine(Line0, (unsigned char *)text);
		sprintf(text," V:%.1lf",getADC(&hadc2));
	LCD_DisplayStringLine(Line2, (unsigned char *)text);
		sprintf(text," T:%d",time_count);
	LCD_DisplayStringLine(Line3, (unsigned char *)text);
		sprintf(text," vmax:%.1lf",Vmax);
	LCD_DisplayStringLine(Line4, (unsigned char *)text);
		sprintf(text," vmin:%.1lf",Vmin);
	LCD_DisplayStringLine(Line5, (unsigned char *)text);
	}
	else if(view==1)										//参数界面
	{

		sprintf(text,"       Para");
	LCD_DisplayStringLine(Line0, (unsigned char *)text);
		sprintf(text," Vmax:%.1lf",Vmax_temp);
	LCD_DisplayStringLine(Line2, (unsigned char *)text);
		sprintf(text," Vmin:%.1lf",Vmin_temp);
	LCD_DisplayStringLine(Line3, (unsigned char *)text);
	}
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
