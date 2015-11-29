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
#define MARGIN					500
#define R_PWM						200

// One tile = 42000 pulses
// 90 degrees = 30000 pulses

//bool seq = true;
int seqPos = 0;
double motorError = 0;
int leftPower = R_PWM;
int motorErrorRecord = 0;

int leftMotorTarget = 0;
int rightMotorTarget = 0;

int leftMotorDiff = 0;
int rightMotorDiff = 0;
double speed;

int count = 0;

//int leftSeq[10] = {42000, 30000, 42000};
//int rightSeq[10] = {42000, -30000, 42000};
int leftSeq[10] = {41000, 30000, 41000, 15000, 58200, 15000, 41000, -30000};
int rightSeq[10] = {41000, -30000, 41000, -15000, 58200, -15000, 41000, 30000};

void motor_moveDist(int targetLeftDist, int targetRightDist, int targetLeftDiff, int targetRightDiff);
void update_dist();
void pause();

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
		
		leftMotorDiff = (leftMotorTarget - getLeftMotorDist());
		rightMotorDiff = (rightMotorTarget - getRightMotorDist());
		if ((abs(leftMotorDiff) > MARGIN) || (abs(rightMotorDiff) > MARGIN)) {
			motor_moveDist(leftMotorTarget, rightMotorTarget, leftMotorDiff, rightMotorDiff);			
		}
		
		if (seqPos < 10) {
			pause();
			if ((abs(leftMotorDiff) <= MARGIN) && (abs(rightMotorDiff) <= MARGIN)) {
				leftMotorTarget += leftSeq[seqPos];
				rightMotorTarget += rightSeq[seqPos];
				seqPos++;
			}
		}
	}	
	return 0;
}

void motor_moveDist(int targetLeftDist, int targetRightDist, int targetLeftDiff, int targetRightDiff){	
	int start = get_real_ticks();
	int leftTempDist = getLeftMotorDist();
	int rightTempDist = getRightMotorDist();
	double leftRatio = abs(targetLeftDiff) / fmax(abs(targetLeftDiff), abs(targetRightDiff));
	double rightRatio = abs(targetRightDiff) / fmax(abs(targetLeftDiff), abs(targetRightDiff));
	speed = 1;
	
	while (1) {
		targetLeftDiff = targetLeftDist - getLeftMotorDist();
		targetRightDiff = targetRightDist - getRightMotorDist();
		
		if ((abs(targetLeftDiff) <= MARGIN) && (abs(targetRightDiff) <= MARGIN)) {
			break;
		}	
		
		motor_control(1, (targetRightDiff >= 0), R_PWM*rightRatio*speed);
		motor_control(2, (targetLeftDiff >= 0), leftPower*leftRatio*speed);
				
		if (((get_real_ticks() - start) % MOTOR_INTERVAL == 0) && (leftRatio*rightRatio)) {			
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
			update_dist();
		}
	}
	motor_control(1, 1, 0);
	motor_control(2, 1, 0);
	count++;
	return;
}

void update_dist() {
	tft_clear();
	tft_prints(0, 0, "R Dist: %d", getRightMotorDist());
	tft_prints(0, 1, "L Dist: %d", getLeftMotorDist());
	tft_prints(0, 2, "R Tar:  %d", rightMotorTarget);
	tft_prints(0, 3, "L Tar:  %d", leftMotorTarget);
	tft_prints(0, 9, "COUNT:  %d", count);		
	tft_update();
}

void pause() {
	int pause_start = get_real_ticks();
	while ((get_real_ticks() - pause_start) < 50) { 
		if (get_ms_ticks() % 50 == 0) {
			LED_ON(GPIOB, GPIO_Pin_4);
		}
		else if (get_ms_ticks() % 25 == 0) {
			LED_OFF(GPIOB, GPIO_Pin_4);
		}
	}
	return;
}