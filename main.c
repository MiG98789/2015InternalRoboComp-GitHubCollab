#include "main.h"

int road_pos = 64;
int mean_array[10];
int ma = 64;

int main(void){
	tft_init(0, BLACK, WHITE, WHITE);
	linear_ccd_init();
	adc_init();
	ticks_init();
	
	for(int i = 0; i < 10; i++){
		mean_array[i] = 64;
	}

	
		while(1){
			if(get_ms_ticks() % 50 == 0){
					tft_prints(0, 0, "       ");
					tft_prints(0, 0, "%d", get_road_pos());
					tft_update();
			}
		}
		
		
return 0;
}

int get_road_pos(void){
	int sum = 0;
	int count = 0;
	
	//---Clearing TFT---//
	for(int i = 0; i < 128; i++){
		tft_put_pixel(i, linear_ccd_buffer1[i], BLACK);
	}
	for(int j = 0; j < 160; j++){
		tft_put_pixel(road_pos, j, BLACK);
	}
	
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
					
	//---Printing road_pos---//
	for(int j = 0; j < 160; j++){
		tft_put_pixel(road_pos, j, WHITE);
	}
	
	return road_pos;
}

//---!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!WORK HERE!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!--//
/*
-    Consider only the biggest 
-
-
-
-
-
-
-
--
]-
-
-
-
-
-
]--
]-]-
-
]-
-
*/

int get_moving_average(void){
	int first_val = mean_array[0];
	
	for(int i = 0; i < 9; i++){
		mean_array[i] = mean_array[i+1];
	}
	
	mean_array[9] = get_road_pos();
	
	ma += (mean_array[9] - first_val)/10;
	
	return ma;
}


//---!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!WORK HERE!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!--//
/*
-
-
-
-
-
-
-
-
--
]-
-
-
-
-
-
]--
]-]-
-
]-
-
*/



int get_didt(void){
	double didt = 0;
	
	didt = (ma-get_moving_average())/0.01;
		
	return didt;
}

int check_angle(void){
	double mperpx = 0; //need to find thru test
	double angle = 0;
	double speed = 0; //need to find thru test
	
	angle = atan((-1*mperpx/(speed*64))*get_didt());
	return angle;
}

int wheel_speed_on_arc(){
	double leftspeed = 0;
	double rightspeed = 0;
	double wheelbase = 0.4;
	
	//if turning right
	//rightspeed = leftspeed*(1-wheelbase)/(1+wheelbase);
	
	//else
	// leftspeed = rightspeed*(1-wheelbase)/(1+wheelbase);
}
