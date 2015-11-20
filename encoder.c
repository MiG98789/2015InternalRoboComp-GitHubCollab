#include "encoder.h"

static int8_t states[] = {0,-1,1,0,1,0,0,-1,-1,0,0,1,0,1,-1,0};
uint8_t rightCurrentState = 0x00;
uint8_t leftCurrentState = 0x00;
uint8_t rightTransition = 0x00;
uint8_t leftTransition = 0x00;
int rightPosition = 0; 
int leftPosition = 0;

void encoder_init(void) {
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_StructInit(&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource9);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource10);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource11);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource12);
	
	EXTI_InitTypeDef EXTI_InitStructure;
	EXTI_InitStructure.EXTI_Line = EXTI_Line9 | EXTI_Line10 | EXTI_Line11 | EXTI_Line12;	
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
	EXTI_Init(&EXTI_InitStructure);
	
	NVIC_InitTypeDef NVIC_InitStructure_95;
	NVIC_InitStructure_95.NVIC_IRQChannel = EXTI9_5_IRQn;
	NVIC_InitStructure_95.NVIC_IRQChannelPreemptionPriority = 0x00;
	NVIC_InitStructure_95.NVIC_IRQChannelSubPriority = 0x01;
	NVIC_InitStructure_95.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure_95);
	
	NVIC_InitTypeDef NVIC_InitStructure_1510;
	NVIC_InitStructure_1510.NVIC_IRQChannel = EXTI15_10_IRQn;
	NVIC_InitStructure_1510.NVIC_IRQChannelPreemptionPriority = 0x00;
	NVIC_InitStructure_1510.NVIC_IRQChannelSubPriority = 0x01;
	NVIC_InitStructure_1510.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure_1510);
}

void rightMotorDecode() {
	rightCurrentState = GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_10) << 1;
	rightCurrentState |= GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_12);
	rightTransition = ((rightTransition << 2) + rightCurrentState) & 0x0F;
	rightPosition += states[rightTransition];
}

void leftMotorDecode() {
	leftCurrentState = GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_9) << 1;
	leftCurrentState |= GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_11);
	leftTransition = ((leftTransition << 2) + leftCurrentState) & 0x0F;
	leftPosition -= states[leftTransition];
}

void EXTI9_5_IRQHandler(void) {
	if (EXTI_GetITStatus(EXTI_Line9) != RESET) {
		leftMotorDecode();		
		EXTI_ClearITPendingBit(EXTI_Line9);
	}
}

void EXTI15_10_IRQHandler(void) {
	if (EXTI_GetITStatus(EXTI_Line10) != RESET) {
		rightMotorDecode();		
		EXTI_ClearITPendingBit(EXTI_Line10);
	}
	if (EXTI_GetITStatus(EXTI_Line11) != RESET) {
		leftMotorDecode();		
		EXTI_ClearITPendingBit(EXTI_Line11);
	}
	if (EXTI_GetITStatus(EXTI_Line12) != RESET) {
		rightMotorDecode();		
		EXTI_ClearITPendingBit(EXTI_Line12);
	}
}

int getRightMotorDist() {
	return rightPosition;
}

int getLeftMotorDist() {
	return leftPosition;
}