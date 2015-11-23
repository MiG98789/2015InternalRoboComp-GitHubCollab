/*
	Pang's code for group 8
	Uploaded to github as branch "encode"
	CHANGE TO YOUR OWN COM PORT B4 PORTING IT TO MAINBOARD
	20/11/2015
*/
#include "main.h"
#include <stdbool.h>
#include <math.h>

#define MOTOR_KP 80

bool seq = true;
double motorError = 0;
int leftPower = 200;

void motor_straight(int time);

int main()
{
	tft_init(0, BLACK, WHITE, GREEN);
	ticks_init();
	LED_INIT();
	motor_init();
	encoder_init();
	
	while(1) {
		if (get_ms_ticks() % 1000 == 0) {
			LED_ON(GPIOB, GPIO_Pin_4);
		}
		else if (get_ms_ticks() % 500 == 0) {
			LED_OFF(GPIOB, GPIO_Pin_4);
		}
		
		
		if (get_ms_ticks() % 100 == 0) {
			tft_clear();
			tft_prints(0, 0, "%d", getRightMotorDist());
			tft_prints(0, 1, "%d", getLeftMotorDist());
			tft_update();
		}
		
		if (seq) {
			motor_straight(500);
			seq = false;
		}		
	}	
	return 0;
}

void motor_straight(int time){
	int start = get_ms_ticks();
	while (get_ms_ticks() - start < time) {
		motor_control(1, 1, 200);
		motor_control(2, 1, 200);
	}
	motor_control(1, 1, 0);
	motor_control(2, 1, 0);
	return;
}

void motor_straightTimed(int time){
	int start = get_ms_ticks();
	clearAll();
	
	while (get_ms_ticks() - start < time) {
		motor_control(1, 1, leftPower);
		motor_control(2, 1, 200);
		if ((get_ms_ticks() - start) % 100 == 0) {
			motorError = getLeftMotorDist() - getRightMotorDist(); //about 8 peaks per ms
			leftPower -= motorError / MOTOR_KP;
			clearAll();
		}
	}
	motor_control(1, 1, 0);
	motor_control(2, 1, 0);
	return;
}