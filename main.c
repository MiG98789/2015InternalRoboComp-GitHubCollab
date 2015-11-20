/*
	Pang's code for group 8
	Uploaded to github as branch "encode"
	CHANGE TO YOUR OWN COM PORT B4 PORTING IT TO MAINBOARD
	20/11/2015
*/
#include "main.h"

void motor_straight(int time);

int main()
{
	tft_init(0, BLACK, WHITE, GREEN);
	ticks_init();
	LED_INIT();
	motor_init();
	encoder_init();
	
	//bool seq = true;
	int ldist;
	int rdist;
	double ltor = 1;
	
	while(1) {
		if (get_ms_ticks() % 1000 == 0) {
			LED_ON(GPIOB, GPIO_Pin_4);
		}
		else if (get_ms_ticks() % 500 == 0) {
			LED_OFF(GPIOB, GPIO_Pin_4);
		}
		//motor_control(1, 1, 100);
		//motor_control(2, 1, 100);
		
		
		if (get_ms_ticks() % 100 == 0) {
			tft_clear();
			tft_prints(0, 0, "%d", getRightMotorDist());
			tft_prints(0, 1, "%d", getLeftMotorDist());
			tft_update();
		}
		/*
		if (get_ms_ticks() % ENCODER_PERIOD == 0) {
			int leftwheel = getCount() - ldist;
			int rightwheel = getCount() - rdist;
			ltor = ldist / rdist;
			motor_control(1, 1, ltor*100);
		}
		
		if (seq) {
			motor_straight(1000);
			seq = false;
		}
		*/
		
	}
	
	return 0;
}

void motor_straight(int time){
	int start = get_ms_ticks();
	while (get_ms_ticks() - start < time) {
		motor_control(1, 1, 50);
		motor_control(2, 1, 50);
	}
	motor_control(1, 1, 0);
	motor_control(2, 1, 0);
	return;
}