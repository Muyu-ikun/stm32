#ifndef __LED_H__
#define __LED_H__

#include "main.h"
#include "gpio.h"

void LED_dis(uint8_t led_flag);
void LED_on(uint8_t i);
void LED_off(uint8_t i);
void LED_tog(uint8_t i);
#endif


