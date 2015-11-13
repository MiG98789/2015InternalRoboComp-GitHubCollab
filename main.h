#ifndef __MAIN_H
#define __MAIN_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include "stm32f10x.h"
#include "ticks.h"
#include "LEDs.h"
#include "tft_display.h"
#include "adc.h"
#include "linear_ccd.h"
#include "motor.h"
#include "GPIO_switch.h"
#include "button.h"
#include "uart.h"
#include "servo_tutorial.h"
#include "pneumatic_control.h"

#define MAX_CCRn			1000
#define MIN_CCRn			450
#define GetCCRn(deg)	(((float)deg / 180) * 600 + MIN_CCRn)
	
#define forward 'w'
#define backward 's'
#define pivotleft 'a'
#define pivotright 'd'
#define stop 32
#define lgrip 'o'
#define rgrip 'p'
#define beginautozone 'b'
#define raiseflag 'f'
#define hit 'h'

#endif /* __MAIN_H */

//Global variables
int road_pos = 64;
int mean_array[10];
int maf = 64;
int mai;
int didt = 0;
double angle = 0;
double leftspeedratio = 0.8;
double rightspeedratio = 0.6;
int motormag = 200;
int uselgrip = 0;
int usergrip = 0;
int flagraise = 0;
int autoormanual = 0; //0 for auto, 1 for manual
int racketswing = 0;

void bluetooth_listener(const uint8_t byte);
int main(void);
int get_road_pos(void);
int get_moving_average(void);
int get_didt(void);
int get_angle(void);
int wheel_speed_on_arc(void);
int stabiliser(void);
int autozone(void);
int manualzone(void);