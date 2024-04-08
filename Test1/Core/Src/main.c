/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
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

#include "interrupt.h"
#include "led.h"
#include "badc.h"
#include "i2c_hal.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

extern struct keys key[4];
uint16_t view=0;
uint pa6_duty=10;
uint16_t pa7_duty=10;
unsigned char eep_temp=0;
bool eep_flag;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void key_proc(void);
void disp_proc(void);
void eep_w(void);
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
  MX_TIM3_Init();
  MX_TIM16_Init();
  MX_TIM17_Init();
  MX_ADC1_Init();
  MX_ADC2_Init();
  /* USER CODE BEGIN 2 */
	
	LED_dis(0X00);
	LCD_Init();
	
	I2CInit();										//I2C初始化
	eeprom_write(1,0x00);
	
	
	LCD_Clear(Black);
    LCD_SetBackColor(Black);
    LCD_SetTextColor(White);
	
	HAL_TIM_Base_Start_IT(&htim3);
	
	HAL_TIM_PWM_Start(&htim16,TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim17,TIM_CHANNEL_1);		//PWM开启通道



	
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	if(eep_flag==1)
	{
//		eep_w();
		eeprom_write(1,111);
		HAL_Delay(500);
		eep_flag=0;
		if (eeprom_read(1) == 111) {
			LED_dis(0x01);
		}
		eep_temp=eeprom_read(1);
	}
//	(eeprom_read(1)<<8)+
	key_proc();
	disp_proc();

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
void key_proc()
{
	if(key[0].single_flag==1)
	{
		LCD_Clear(Black);
		view=!view;
		key[0].single_flag=0;
	}
	if(key[1].single_flag==1)
	{
		pa6_duty+=10;
		if(pa6_duty>=100)pa6_duty=10;
		__HAL_TIM_SetCompare(&htim16,TIM_CHANNEL_1,pa6_duty);
		key[1].single_flag=0;
	}
	if(key[2].single_flag==1)
	{
		pa7_duty+=10;
		if(pa7_duty>=100)pa7_duty=10;
		__HAL_TIM_SetCompare(&htim17,TIM_CHANNEL_1,pa7_duty);
		key[2].single_flag=0;
	}
	if(key[3].single_flag==1)
	{
		LCD_Clear(Black);
		eep_flag=1;
		key[3].single_flag=0;
	}
}   
void disp_proc()
{
	char text[30];
	if(view==0)
	{	
		sprintf(text,"       Data");
		LCD_DisplayStringLine(Line1, (unsigned char *)text);
		sprintf(text,"       V1:%.2f",getADC(&hadc1));
		LCD_DisplayStringLine(Line3, (unsigned char *)text);
		sprintf(text,"       V2:%.2f",getADC(&hadc2));
		LCD_DisplayStringLine(Line4, (unsigned char *)text);
	}
	else if(view==1)
	{
		sprintf(text,"       Para");
		LCD_DisplayStringLine(Line0, (unsigned char *)text);
		sprintf(text,"    PA6:%d",pa6_duty);
		LCD_DisplayStringLine(Line2, (unsigned char *)text);
		sprintf(text,"    PA7:%d",pa7_duty);
		LCD_DisplayStringLine(Line4, (unsigned char *)text);
		
		sprintf(text,"    eep_pa6:%d",eep_temp);
		LCD_DisplayStringLine(Line6, (unsigned char *)text);

	}
	
}
void eep_w()
{
	uchar pa6_h=pa6_duty>>8;
	uchar pa6_l=pa6_duty&0xff;
	eeprom_write(1,pa6_h);
	HAL_Delay(10);
	eeprom_write(2,pa6_l);
	HAL_Delay(10);
	
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
