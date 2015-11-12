//---!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!IMPORTANT NOTES!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!---//
/*
-    	Consider only the biggest for get_road_pos -Vikram 
-
-			Moved some declarations into main.h so that order of declaration of functions and variables
-			will not matter
-
-				How to break out of while loop when: (A: Auto, M: Manual)
-					--> Reached holder zone (A to M)
-					(X) -->	Start line tracing again	(M to A)
-					--> Reached serving zone	(A to M)
-
-				If time allows (but still really important):
-					--> Find a way to reset the whole int main(void){...}
-					(X) --> Still need to know how to actually detect which holder tube is correct
-							(X) SUGGESTED BY HARDWARE:
-								(X) --> Either (1) pass current through gripper and automatically grip if detected current
-								(X) --> Or (2) light up LED if current is detected
-
-				TODO (before Saturday):
-					--> Find the correct magnitude of motor_control() for wheels
-					--> How to use get_angle() to turn (and from get_angle, when to call wheel_speed_on_arc())
--				--> Using the linear_ccd_buffer1[] list to find out if there is a 90 deg / 135 deg turn
-					IF POSSIBLE:	
-						--> Find correct timing for hitting badminton
-						(X) --> Set up basic bluetooth functions
-						--> Setting up PS3 driver to use PS3 controller to emulate keyboard (using Scarlet Crush)
-						--> Try to set up second version of code
-
-				(X) IMPORTANT POINT TO SHARE WITH HARDWARE SECTOR AS WELL:
-					(X) --> When testing the motor driver, motor_control(0, 0, 100); was used. However, for the first
-							(X) argument, only 1 or 2 can be input. If 0 is input, it does not really do anything.
-							--> GPIO pin starts at high voltage (because in GPIO_switch.c, GPIO mode is IPU
*/

#include "main.h"

int main(void){
	motor_init();
	pneumatic_init();
	LED_INIT();
	linear_ccd_init();
	tft_init(0, BLACK, WHITE, WHITE);
	ticks_init();
	adc_init();
	button_init();
	GPIO_switch_init();
	uart_init(COM3, 115200);
	uart_interrupt_init(COM3, &bluetooth_listener);
	for(int i = 0; i < 10; i++){
		mean_array[i] = 64;
	}
	
	while(1){
		if(read_GPIO_switch(GPIOA, GPIO_Pin_9) == 1){
			LED_ON(GPIOA, GPIO_Pin_15);
		}
		else if(read_GPIO_switch(GPIOA,GPIO_Pin_9) == 0){
			LED_OFF(GPIOA,GPIO_Pin_15);
		}
	}
	
	
	//---AUTOZONE (LINE TRACER)---//
	while(1){
		if(autoormanual == 'a'){
			autozone();
		}
		else if(autoormanual == 'm'){
			manualzone();
		}
	}
	
	
	
	
	//---MANUALZONE (PICKING UP TUBES)---//
	
		
		
		//---AUTOZONE (LINE TRACER)---//
		
		
		
		
		//---MANUALZONE (RAISE FLAG AND HIT SHUTTLECOCK)---//

		
		
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
	int i = 0;
	/*
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
			*/
		
		didt = (get_moving_average() - mai)/0.01;
	
	return didt;
}

int get_angle(void){
	double mperpx = 0.26; //need to find thru test (from 15cm high, can see 26 cm)
	double angle = 0;
	double speed = 0.5; //need to find thru test
	
	angle = atan((-1*mperpx/(speed*64))*get_didt())*1000;
	return angle;
}

int wheel_speed_on_arc(void){
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

/////-----VVVVVVVVVV-----/////
//---MANUALZONE---//

void bluetooth_listener(const uint8_t byte){	
	switch(byte){
		//case for left gripper
		case lgrip:
			if(uselgrip%2 == 0){
				pneumatic_control(GPIOB, GPIO_Pin_5, 1);
				uselgrip++;
			}
			else{
				pneumatic_control(GPIOB, GPIO_Pin_5, 0);
				uselgrip++;
			}
			
			uart_tx_byte(COM3, 'o');
			break;
		
		//case for right gripper
		case rgrip:
			if(usergrip%2 == 0){
				pneumatic_control(GPIOB, GPIO_Pin_6, 1);
				usergrip++;
			}
			else{
				pneumatic_control(GPIOB, GPIO_Pin_6, 0);
				usergrip++;
			}
		
			uart_tx_byte(COM3, 'p');
			break;
		
		//case for pivoting left
		case pivotleft:
			motor_control(1,0,200); //right wheel forward
			motor_control(2,1,200); //left wheel backward
		
			uart_tx_byte(COM3, 'a');
			break;
		
		//case for pivoting right
		case pivotright:
			motor_control(1,1,200); //right wheel backward
			motor_control(2,0,200); //left wheel forward
		
			uart_tx_byte(COM3, 'd');
			break;
		
		//case for moving forward
		case forward:
			motor_control(1,0,200);
			motor_control(2,0,200);
		
			uart_tx_byte(COM3, 'w');
			break;
		
		//case for moving backward
		case backward:
			motor_control(1,1,200);
			motor_control(2,1,200);
		
			uart_tx_byte(COM3, 's');
			break;
		
		//case for stopping
		case stop:
			motor_control(1,0,0);
			motor_control(2,0,0);
		
			uart_tx(COM3, "Space");
			break;
		
		//case for beginning autozone
		case beginautozone:
			autoormanual = 'a';
		
			uart_tx_byte(COM3, 'b');
			break;
		
		//case for raising flag
		case raiseflag:
			if(flagraise%2 == 0){
				pneumatic_control(GPIOB, GPIO_Pin_7, 1);
				flagraise++;
			}
			else{
				pneumatic_control(GPIOB, GPIO_Pin_7, 0);
				flagraise++;
			}
			
			uart_tx_byte(COM3, 'f');
			break;
			
		
		//case for dropping shuttlecock and hitting with racket
		case hit:
			if(racketswing%2 == 0){
				pneumatic_control(GPIOB, GPIO_Pin_8, 1);
				racketswing++;
			//servo for shuttlecock
			}
			else{
				pneumatic_control(GPIOB, GPIO_Pin_8, 0);
				racketswing++;
			}
			
			uart_tx_byte(COM3, 'h');
			break;
	}
}


/////-----VVVVVVVVVV-----/////
//---TRANSITION BETWEEN AUTOZONE AND MANUALZONE---//


int autozone(void){
	autoormanual = 'a';
	
	while(autoormanual == 'a'){
		if(get_ms_ticks()%50 == 0){
			get_angle();
			//if condition, when met, change autoormanual to false
		}
	}
}

int manualzone(void){
	autoormanual = 'm';
	
	while(autoormanual == 'm'){
		uart_interrupt(COM3);
	}
}