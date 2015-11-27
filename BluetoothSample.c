#include "main.h"

void bluetooth_listener(const uint8_t byte){
	LED_ON(GPIOA, GPIO_Pin_15);
	LED_ON(GPIOB, GPIO_Pin_3);
	LED_ON(GPIOB, GPIO_Pin_4);	
	
	switch(byte){
		case '1':
			LED_ON(GPIOA, GPIO_Pin_15);
			LED_OFF(GPIOB, GPIO_Pin_3);
			LED_OFF(GPIOB, GPIO_Pin_4);
			uart_tx_byte(COM3, '1');
			break;
		
		case '2':
			LED_OFF(GPIOA, GPIO_Pin_15);
			LED_ON(GPIOB, GPIO_Pin_3);
			LED_OFF(GPIOB, GPIO_Pin_4);
			uart_tx_byte(COM3, '2');
			break;
		
		case '3':
			LED_OFF(GPIOA, GPIO_Pin_15);
			LED_OFF(GPIOB, GPIO_Pin_3);
			LED_ON(GPIOB, GPIO_Pin_4);
			uart_tx_byte(COM3, '3');
			break;
		
		case '0':
			LED_OFF(GPIOA, GPIO_Pin_15);
			LED_OFF(GPIOB, GPIO_Pin_3);
			LED_OFF(GPIOB, GPIO_Pin_4);
		  uart_tx_byte(COM3, '0');
			break;
		}
}

int main(void)
{
	LED_INIT();
	ticks_init();
	uart_init(COM3, 115200);
	uart_interrupt_init(COM3, &bluetooth_listener);
	
	while(1){
		uart_interrupt(COM3);		
		}
}