#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_tim.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_exti.h"

void encoder_init();
void rightMotorDecode();
void leftMotorDecode();
int getLeftMotorDist();
int getRightMotorDist();

void EXTI9_5_IRQHandler();
void EXTI15_10_IRQHandler();