#include "led.h"
#include "main.h"

uchar led_temp=0xff;

void LED_dis(uint8_t led_flag)
{	
	led_flag=~led_flag;
	HAL_GPIO_WritePin(GPIOC,GPIO_PIN_All,GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOC,led_flag<<8,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_2,GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_2,GPIO_PIN_RESET);
	
}
void LED_on(uint8_t i)
{
	i=~i;
	led_temp&=i;
	LED_dis(led_temp);
}
void LED_off(uint8_t i)
{
	i=i;
	led_temp|=i;
	LED_dis(led_temp);
}
void LED_tog(uint8_t i)
{
	led_temp^=i;
	LED_dis(led_temp);
}


