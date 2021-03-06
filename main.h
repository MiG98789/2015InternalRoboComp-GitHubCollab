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

#endif /* __MAIN_H */

//---MAIN---//

int main(void);
void primitive_stabiliser(void);
void all_init(void);
int get_road_pos(void);
int dynamic_threshold(void);

int threshold = 150;
int road_pos = 64;
int mean_array[10];
int posflag = 0;
int lcount = 0;
int mcount = 0;
int rcount = 0;

//---BLUETOOTH---//
void bluetooth_listener(const uint8_t byte);

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

int uselgrip = 0;
int usergrip = 0;
int flagraise = 0;
int racketswing = 0;
double leftspeedratio = 0.8;
double rightspeedratio = 0.6;
int leftdirection = 1;
int rightdirection = 1;
int motormag = 125;
int autoormanual = 0;
int blackflag = 0;
int whiteflag = 0;

//PID
double Kp = 4;
double Kd = 2;
int ccd_error_f=0;
int ccd_error_i=0;
double ccd_rate;
int initial_ccd[128];
int final_ccd[128];
int final_road_pos;
double pid;

double PID(void);
int pid_stabiliser();



//---VVVVVVV---JUNK FOR NOW---VVVVVVV---//

//---ANGLE---//
int maf = 64;
int mai;
int didt = 0;
double angle = 0;
int ccd_refined[128];
int inter_ccd[16];

int get_moving_average(void);
int get_didt(void);
int get_angle(void);
int wheel_speed_on_arc(void);

//---AREA---//
int area(void);
int stabiliser(void);

double Larea = 0;
double Marea = 0;
double Rarea = 0;
int areaflag = 0;

//---DECASTELJAU ALGORITHM---//
u32 deCasteljau(u32 args[], double t);

//---RAMER-DOUGLAS-PEUCKER ALGORITHM---//
u32 DouglasPeucker(u32 args[], int startIndex, int lastIndex, double epsilon);