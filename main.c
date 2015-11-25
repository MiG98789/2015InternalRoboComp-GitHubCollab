/*
	Pang's code for group 8
	Uploaded to github as branch "encode"
	CHANGE TO YOUR OWN COM PORT B4 PORTING IT TO MAINBOARD
	20/11/2015
*/
#include "main.h"
#include <stdbool.h>
#include <math.h>

#define MOTOR_KP				20 //about 4 peaks per ms
#define MOTOR_INTERVAL	50 //interval of close loop control in ms
#define TARGET_MARGIN		5

bool seq = true;
double motorError = 0;
int leftPower = 100;
int motorErrorRecord = 0;

int leftMotorTarget = 0;
int rightMotorTarget = 0;

int leftMotorDiff = 0;
int rightMotorDiff = 0;
double speed;

void motor_moveDist(int targetLeftDist, int targetRightDist, int targetLeftDiff, int targetRightDiff);
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
			tft_clear();
			update_dist();
		}
		
		if (seq) {
			leftMotorTarget += 100000;
			rightMotorTarget += 100000;
			seq = false;
		}
		
		leftMotorDiff = (leftMotorTarget - getLeftMotorDist());
		rightMotorDiff = (rightMotorTarget - getRightMotorDist());
		if ((abs(leftMotorDiff) > 3000) || (abs(rightMotorDiff) > 3000)) {
			motor_moveDist(leftMotorTarget, rightMotorTarget, leftMotorDiff, rightMotorDiff);			
		}
	}	
	return 0;
}

void motor_moveDist(int targetLeftDist, int targetRightDist, int targetLeftDiff, int targetRightDiff){	
	int start = get_real_ticks();
	int leftTempDist = getLeftMotorDist();
	int rightTempDist = getRightMotorDist();
	double leftRatio = targetLeftDiff / fmax(targetLeftDiff, targetRightDiff);
	double rightRatio = targetRightDiff / fmax(targetLeftDiff, targetRightDiff);
	speed = 1;
	
	while (1) {
		tft_clear();
		targetLeftDiff = targetLeftDist - getLeftMotorDist();
		targetRightDiff = targetRightDist - getRightMotorDist();
		
		if ((abs(targetLeftDiff) <= 3000) || (abs(targetRightDiff) <= 3000)) {
			break;
		}	
		
		motor_control(1, (targetRightDiff >= 0), 100*rightRatio*speed);
		motor_control(2, (targetLeftDiff >= 0), leftPower*leftRatio*speed);
				
		if (((get_real_ticks() - start) % MOTOR_INTERVAL == 0) && (leftRatio*rightRatio)) {
			tft_prints(0, 4, "R TarD: %d", targetRightDiff);
			tft_prints(0, 5, "L TarD: %d", targetLeftDiff);	
			tft_prints(0, 6, "L PWM:  %d", leftPower);
			
			if ((targetRightDiff >= 0) == (targetLeftDiff >= 0))
				motorError = (getLeftMotorDist() - leftTempDist)/leftRatio - (getRightMotorDist() - rightTempDist)/rightRatio;
			else
				motorError = (getLeftMotorDist() - leftTempDist)/leftRatio + (getRightMotorDist() - rightTempDist)/rightRatio;
			
			if (targetLeftDiff >= 0)
				leftPower -= (motorError / speed / MOTOR_KP);
			else
				leftPower += (motorError / speed / MOTOR_KP);
			
			leftTempDist = getLeftMotorDist();
			rightTempDist = getRightMotorDist();
		}
		update_dist();
	}
	motor_control(1, 1, 0);
	motor_control(2, 1, 0);
	return;
}

void update_dist() {
	tft_prints(0, 0, "R Dist: %d", getRightMotorDist());
	tft_prints(0, 1, "L Dist: %d", getLeftMotorDist());
	tft_prints(0, 2, "R Tar:  %d", rightMotorTarget);
	tft_prints(0, 3, "L Tar:  %d", leftMotorTarget);	
	tft_prints(0, 7, "Speed:  %.2f", speed);	
	tft_update();
}