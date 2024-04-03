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
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "led.h"
#include "interrupt.h"
#include "stdio.h"
#include "lcd.h"
#include "string.h"
#include "calculation.h"
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
extern char rx_data[30];
extern uint8_t rx_dat;
extern uchar rx_pointer;
bool view=0;

uint Cnum=0;
uint Vnum=0;

extern char car_type[5];
extern char car_data[5];
extern char car_time[13];
extern struct cars car[];
extern struct cars Car;
double Cprice=3.50;
double Vprice=2.00;
bool led1_flag=0;
bool error_flag=0;
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
void uart_rx_proc(void);
void error_proc(void);

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
  MX_TIM17_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */

	LED_Dsp(0x00);			//LED初始化
	LCD_Init();				//LCD初始化
	
	LCD_Clear(Black);		
    LCD_SetBackColor(Black);
    LCD_SetTextColor(White);
	
	HAL_UART_Receive_IT(&huart1,&rx_dat,1);   //UART1初始化
	
	HAL_TIM_Base_Start_IT(&htim3);			//打开tim3
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  lcd_proc();
	  key_proc();
	  led_proc();
	  if(rx_pointer!=0)
	  {
		  int temp=rx_pointer;
		  HAL_Delay(1);
		  if(temp==rx_pointer)uart_rx_proc();
	  }
	  error_proc();
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
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
	  char temp[20];							//串口发送测试
	  sprintf(temp,"CNBR: %d\r\n",Cnum);
	  HAL_UART_Transmit(&huart1,(uint8_t*)temp,strlen(temp),50);
		key[0].singel_flag=0;
	}
	if(key[1].singel_flag==1)
	{
		if(view!=0)
		{
			Cprice+=0.5;
			Vprice+=0.5;			
		}
		key[1].singel_flag=0;
	}
	if(key[2].singel_flag==1)
	{
		if(view!=0)
		{
			Cprice-=0.5;
			Vprice-=0.5;
			
			if(Vprice<0)
			{
				Cprice=0;
				Vprice=0;
			}
		}
		key[2].singel_flag=0;
	}
	if(key[3].singel_flag==1)
	{
		led1_flag=!led1_flag;
		if(led1_flag==1)
		HAL_TIM_PWM_Start(&htim17,TIM_CHANNEL_1);
		else if(led1_flag==0)
		HAL_TIM_PWM_Stop(&htim17,TIM_CHANNEL_1);
		HAL_GPIO_WritePin(GPIOA,GPIO_PIN_7,GPIO_PIN_RESET);
		key[3].singel_flag=0;
	}
}
void lcd_proc(void)
{
	char text[30];
	if(view==0)						//车位显示界面
	{
		sprintf(text,"       Data");
	LCD_DisplayStringLine(Line1, (unsigned char *)text);
		sprintf(text,"   CNBR:%d",Cnum);
	LCD_DisplayStringLine(Line3, (unsigned char *)text);
		sprintf(text,"   VNBR:%d",Vnum);
	LCD_DisplayStringLine(Line5, (unsigned char *)text);
		sprintf(text,"   IDLE:%d",8-Cnum-Vnum);
	LCD_DisplayStringLine(Line7, (unsigned char *)text);
	}
	else if(view==1)				//费率界面
	{
		sprintf(text,"       Para");
	LCD_DisplayStringLine(Line1, (unsigned char *)text);
		sprintf(text,"   CNBR:%.2lf",Cprice);
	LCD_DisplayStringLine(Line3, (unsigned char *)text);
		sprintf(text,"   VNBR:%.2lf",Vprice);
	LCD_DisplayStringLine(Line5, (unsigned char *)text);
		sprintf(text,"   car:%s",car[0].car_data);
	LCD_DisplayStringLine(Line7, (unsigned char *)text);
	}
	
}

void led_proc(void)
{
	  if(8-Cnum-Vnum>0&&led1_flag)
		  LED_Dsp(0x03);
	  else if(8-Cnum-Vnum>0)
		  LED_Dsp(0x02);
	  else if(led1_flag)
		  LED_Dsp(0x01);
	  else if(8-Cnum-Vnum<=0&&led1_flag==0)
		  LED_Dsp(0x00);
}

void outCar()
{

	if(strcmp(Car.car_type,"CNBR")==0)
	{
		Cnum--;
	}
	else if(strcmp(Car.car_type,"VNBR")==0)
	{
		Vnum--;
	}
	
	
}

void inCar()
{
	for(int i=0;i<8;i++)
	{
		if(car[i].car_data[0] == '\0')
		{
			car[i]=Car;
			if(strcmp(car[i].car_type,"VNBR")==0)
			{
				Vnum++;
			}else if(strcmp(car[i].car_type,"CNBR")==0)
			{
				Cnum++;
			}
			break;
		}
	}
}

void uart_rx_proc(void)
{
	if(rx_pointer>0)
	{
		if(rx_pointer==22)
		{
			sscanf(rx_data,"%4s:%4s:%12s:",Car.car_type,Car.car_data,Car.car_time);	
			if(strcmp(Car.car_type,"CNBR")==0||strcmp(Car.car_type,"VNBR")==0)
			{
				for(int i=0;i<8;i++)
				{
					if(strcmp(Car.car_data,car[i].car_data)==0)
					{
						if(strcmp(Car.car_type,car[i].car_type)==0)			//车库中已有需要出库
						{
							outCar();
							memset(&car[i],0,sizeof(car[i]));
							break;
						}
						else
						{
							error_flag=1;
						}
					}
					else if(strcmp(Car.car_data,car[i].car_data)!=0&&Cnum+Vnum<8)		//车库中没有需要入库
					{
						inCar();
						break;
					}
				}
			}
			else
			{
				error_flag=1;
			}
		}
		else
		{
			error_flag=1;
		}
		rx_pointer=0;
		memset(rx_data,0,30);
	}
}
void error_proc()
{
	if(error_flag==1)
	{
		char temp[20];							//串口发送错误
		sprintf(temp,"Error\r\n");
		HAL_UART_Transmit(&huart1,(uint8_t*)temp,strlen(temp),50);	
	}
	error_flag=0;
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
