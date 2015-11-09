#include "main.h"

void bluetooth_listener(const uint8_t byte){
	switch(byte){
		//case for left gripper
		
		//case for right gripper
		
		//case for pivoting left
		
		//case for pivoting right
		
		//case for moving forward
		
		//case for moving backward
		
		//case for beginning autozone
		
		//case for raising flag
		
		//case for dropping shuttlecock and hitting with racket

			}
}

//Global variables
int road_pos = 64;
int mean_array[10];
int maf = 64;
int mai;

int main(void){
	init();
	uart_init(COM3, 115200);
	uart_interrupt_init(COM3, &bluetooth_listener);
	for(int i = 0; i < 10; i++){
		mean_array[i] = 64;
	}
	
	//---AUTOZONE (LINE TRACER)---//
	
	//motor_control(1,0, 100);
			//while(1){
			//if(get_ms_ticks() % 50 == 0){
					//tft_prints(0, 0, "       ");
					//tft_prints(0, 0, "%d", get_angle()*10);
					//tft_update();
					
			//}
		//}
		
	
	
	
	
	
	//---MANUALZONE (PICKING UP TUBES)---//
	/*
	while(1){
		uart_interrupt(COM3);		
		}
		*/
		
		
		//---AUTOZONE (LINE TRACER)---//
		
		
		
		
		//---MANUALZONE (RAISE FLAG AND HIT SHUTTLECOCK)---//
		/*
		while(1){
		uart_interrupt(COM3);		
		}
		*/
		
		
	return 0;
}

/*
*
*
*
*
*
*/

/////-----VVVVVVVVVV-----/////
//---AUTOZONE (LINE TRACER)---//

//---!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!IMPORTANT NOTES!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!---//
/*
-    	Consider only the biggest for get_road_pos -Vikram 
-
-				How to break out of while loop when: (A: Auto, M: Manual)
-				--> Reached holder zone (A to M)
-				-->	Start line tracing again	(M to A)
-				--> Reached serving zone	(A to M)
-
-				If time allows (but still really important):
--				--> Find a way to reset the whole int main(void){...}
]-				--> Still need to know how to actually detect which holder tube is correct
-
-				TODO (before Saturday):
-					--> Find the correct magnitude of motor_control() for wheels
-					--> How to use get_angle() to turn (and from get_angle, when to call wheel_speed_on_arc())
]--				--> Using the linear_ccd_buffer1[] list to find out if there is a 90 deg / 135 deg turn
]-]-			IF POSSIBLE:	
-						--> Find correct timing for hitting badminton
]-					--> Set up basic bluetooth functions
-						--> Setting up PS3 driver to use PS3 controller to emulate keyboard (using Scarlet Crush)
-						--> Try to set up second version of code
*/

int get_road_pos(void){
	int sum = 0;
	int count = 0;
	
	//---Clearing TFT---//
	for(int i = 0; i < 128; i++){
		tft_put_pixel(i, linear_ccd_buffer1[i], BLACK);
	}
	/*
	for(int j = 0; j < 160; j++){
		tft_put_pixel(road_pos, j, BLACK);
	}
	*/
	
	//---Updating CCD and getting road_pos---//
	linear_ccd_read();
	
	//---Updating TFT and calculating road_pos---//
	for(int i = 0; i < 128; i++){
		if(linear_ccd_buffer1[i] >= 150){
			tft_put_pixel(i, linear_ccd_buffer1[i], WHITE);
			sum = sum + i;
			count++;
		}
		else{
			tft_put_pixel(i, linear_ccd_buffer1[i], BLUE);
		}
	}
			
	if(count < 5){
		road_pos = 64;
	}
	else{
		road_pos = sum/count;		
	}
					
	//---Printing road_pos as a vertical line---//
	/*for(int j = 0; j < 160; j++){
		tft_put_pixel(road_pos, j, YELLOW);
	}
	*/
	
	return road_pos;
}

int get_moving_average(void){
	int sum = 0;
	
	for(int i = 0; i < 9; i++){
		mean_array[i] = mean_array[i+1];
	}
	mean_array[9] = get_road_pos();
	
	for(int j = 0; j < 10; j++){
		sum += mean_array[j];
	}
	
	maf = sum/10;
	
	return maf;
}

int get_didt(void){
	int didt = 0;
	int i = 0;
	
	while(i<10){
		if(get_ms_ticks()%10 == 0){
			for(i = 0; i < 10; i++){
				if(i == 0){
					mai = get_moving_average();
				}
				else if(i == 9){
					didt = (get_moving_average() - mai)/0.01;
				}
				else{
					get_moving_average();
				}
			}
		}
	}
	
	return didt;
}

int get_angle(void){
	double mperpx = 0.26; //need to find thru test (from 15cm high, can see 26 cm)
	int angle = 0;
	double speed = 0.5; //need to find thru test
	
	angle = atan((-1*mperpx/(speed*64))*get_didt());
	return angle;
}

int wheel_speed_on_arc(){
	double leftspeed = 0.5;
	double rightspeed = 0.5;
	double wheelbase = 0.4;
	
	if(get_angle() > 0){
	rightspeed = leftspeed*(1-wheelbase)/(1+wheelbase);
	}
	
	else if(get_angle() < 0){
	leftspeed = rightspeed*(1-wheelbase)/(1+wheelbase);
	}
	
	else{
		leftspeed = 0.5;
		rightspeed = 0.5;
	}
	
	motor_control(1, 0, 400*leftspeed);
}
