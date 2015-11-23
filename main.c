/*
	Pang's code for group 8
	Uploaded to github as branch "encode"
	CHANGE TO YOUR OWN COM PORT B4 PORTING IT TO MAINBOARD
	20/11/2015
*/
#include "main.h"
#include <stdbool.h>
#include <math.h>

#define MOTOR_KP 80 //about 8 peaks per ms

bool seq = true;
double motorError = 0;
int leftPower = 200;

void motor_straight(int time);
void motor_straightTimed(int time);
void update_dist();

int main()
{
	tft_init(0, BLACK, WHITE, GREEN);
	ticks_init();
	LED_INIT();
	motor_init();
	encoder_init();
	motor_control(1, 1, 0);
	motor_control(2, 1, 0);	
	
	while(1) {
		if (get_ms_ticks() % 1000 == 0) {
			LED_ON(GPIOB, GPIO_Pin_4);
		}
		else if (get_ms_ticks() % 500 == 0) {
			LED_OFF(GPIOB, GPIO_Pin_4);
		}
				
		if (get_ms_ticks() % 100 == 0) {
			update_dist();
		}
		
		if (seq) {
			motor_straightTimed(1500);
			seq = false;
		}
	}	
	return 0;
}

void motor_straight(int time){
	int start = get_real_ticks();
	while (get_real_ticks() - start < time) {
		motor_control(1, 1, 200);
		motor_control(2, 1, 200);
		if ((get_real_ticks() - start) % 100 == 0) {
			update_dist();
		}
	}
	motor_control(1, 1, 0);
	motor_control(2, 1, 0);
	return;
}

void motor_straightTimed(int time){
	clearAll();
	int start = get_real_ticks();
	
	while (get_real_ticks() - start < time) {
		motor_control(1, 1, 200);
		motor_control(2, 1, leftPower);
		if ((get_real_ticks() - start) % 100 == 0) {
			motorError = getLeftMotorDist() - getRightMotorDist();
			leftPower -= motorError / MOTOR_KP;
			clearAll();
			update_dist();
		}
	}
	motor_control(1, 1, 0);
	motor_control(2, 1, 0);
	return;
}

void update_dist() {
	tft_clear();
	tft_prints(0, 0, "R Dist: %d", getRightMotorDist());
	tft_prints(0, 1, "L Dist: %d", getLeftMotorDist());
	tft_prints(0, 3, "L PWM:  %d", leftPower);
	//tft_prints(0, 5, "
	tft_update();
}