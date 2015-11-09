#include "main.h"

void bluetooth_listener(const uint8_t byte){
	LED_ON(GPIOA, GPIO_Pin_15);
	LED_ON(GPIOB, GPIO_Pin_3);
	LED_ON(GPIOB, GPIO_Pin_4);	
	
	switch(byte){
		
			}
}

//int main(void)
//{
//	LED_INIT();
//	ticks_init();
//	uart_init(COM3, 115200);
//	uart_interrupt_init(COM3, &bluetooth_listener);
//	
//	while(1){
//		uart_interrupt(COM3);		
//		}
//}

int road_pos = 64;
int mean_array[10];
int maf = 64;
int mai;

int main(void){
	init();
	
	for(int i = 0; i < 10; i++){
		mean_array[i] = 64;
	}

	motor_control(1,0,200);
	
		//while(1){
			//if(get_ms_ticks() % 50 == 0){
					//tft_prints(0, 0, "       ");
					//tft_prints(0, 0, "%d", get_angle()*10);
					//tft_update();
					
			//}
		//}
		
		
return 0;
}

/////-----VVVVVVVVVV-----/////
//---SEE OTHER FUNCTIONS---//


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
	/*for(int j = 0; j < 160; j++){
		tft_put_pixel(road_pos, j, YELLOW);
	}
	*/
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
	mai = maf;
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
	
	didt = (get_moving_average() - mai)/0.01;
		
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
	
	motor_control(1, 0, 100*leftspeed);
}
