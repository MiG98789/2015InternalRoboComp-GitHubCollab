//---!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!IMPORTANT NOTES!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!---//
/*
-    	Consider only the biggest for get_road_pos -Vikram 
-
-			Moved some declarations into main.h so that order of declaration of functions and variables
-			will not matter
-
-			Changed speed to be dependent on a ratio	
-
-			motor_control PWMx: 1 for PB15, 2 for PB14
-
-				How to break out of while loop when: (A: Auto, M: Manual)
-					(X) --> Reached holder zone (A to M)
-					(X) -->	Start line tracing again	(M to A)
-					(X) --> Reached serving zone	(A to M)
-
-				If time allows (but still really important):
-					(X) --> Find a way to reset the whole int main(void){...}
-					(X) --> Still need to know how to actually detect which holder tube is correct
-							(X) SUGGESTED BY HARDWARE:
-								(X) --> Either (1) pass current through gripper and automatically grip if detected current
-								(X) --> Or (2) light up LED if current is detected
-
-				TODO (before Saturday):
-					--> Find the correct magnitude of motor_control() for wheels
-					--> How to use get_angle() to turn 
-					--> When to call wheel_speed_on_arc()
-					(X) --> What to do on a 90deg turn
-					--> What to do on a 135deg turn
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
	/*
	while(1){
		if(autoormanual == 0){
			autozone();
		}
		else if(autoormanual == 1){
			manualzone();
		}
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

int get_road_pos(void){
	int sum = 0;
	int count = 0;
	
//	//---Clearing TFT---//
//	for(int i = 0; i < 128; i++){
//		tft_put_pixel(i, linear_ccd_buffer1[i], BLACK);
//	}
//	/*
//	for(int j = 0; j < 160; j++){
//		tft_put_pixel(road_pos, j, BLACK);
//	}
//	*/
	
	//---Updating CCD and getting road_pos---//
	linear_ccd_read();
	
	//---Updating TFT and calculating road_pos---//
	for(int i = 0; i < 128; i++){
		if(linear_ccd_buffer1[i] >= 150){
			//tft_put_pixel(i, linear_ccd_buffer1[i], WHITE);
			sum = sum + i;
			count++;
		}
//	else{
//		tft_put_pixel(i, linear_ccd_buffer1[i], BLUE);
//	}
	}
			
	if(count < 5){
		road_pos = -1;
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
	double halfccdrange = 0.26; //need to find thru test (from 15cm high, can see 26 cm)
	double speed = 0.5; //need to find thru test
	
	angle = atan((-1*halfccdrange/(speed*64))*get_didt())*1000;
	return angle;
}

int wheel_speed_on_arc(void){
	double wheelbase = 0.4;
	
	if(angle > 0){
	rightspeedratio = leftspeedratio*(1-wheelbase)/(1+wheelbase);
	}
	
	else if(angle < 0){
	leftspeedratio = rightspeedratio*(1-wheelbase)/(1+wheelbase);
	}
}

int stabiliser(void){
	if(road_pos != -1 && road_pos <64){
		rightspeedratio = 0.8;
		leftspeedratio = 0.8;
	}
	else if(road_pos > 64){
		rightspeedratio = 0.8;
		leftspeedratio = 0.6;
	}
	else if(road_pos == 64){
		rightspeedratio = 0.6;
		leftspeedratio = 0.8;
	}
	else if(road_pos == -1){
		rightspeedratio = 0;
		leftspeedratio = 0;
		autoormanual = 1;
	}
}



















/////-----VVVVVVVVVV-----/////
//---MANUALZONE---//

void bluetooth_listener(const uint8_t byte){	
	rightspeedratio = 0.6;
	leftspeedratio = 0.8;
	
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
			motor_control(1,1,motormag*rightspeedratio); //right wheel forward
			motor_control(2,0,motormag*leftspeedratio); //left wheel backward
		
			uart_tx_byte(COM3, 'a');
			break;
		
		//case for pivoting right
		case pivotright:
			motor_control(1,0,motormag*rightspeedratio); //right wheel backward
			motor_control(2,1,motormag*leftspeedratio); //left wheel forward
		
			uart_tx_byte(COM3, 'd');
			break;
		
		//case for moving forward
		case forward:
			motor_control(1,1,motormag*rightspeedratio);
			motor_control(2,1,motormag*leftspeedratio);
		
			uart_tx_byte(COM3, 'w');
			break;
		
		//case for moving backward
		case backward:
			motor_control(1,0,motormag*rightspeedratio);
			motor_control(2,0,motormag*leftspeedratio);
		
			uart_tx_byte(COM3, 's');
			break;
		
		//case for stopping
		case stop:
			motor_control(1,0,0);
			motor_control(2,0,0);
		
			uart_tx(COM3, "%s","Stop");
			break;
		
		//case for beginning autozone
		case beginautozone:
			autoormanual = 0;
		
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
				//if to set timing to release shuttlecock
				pneumatic_control(GPIOB, GPIO_Pin_5, 1);
				racketswing++;
			}
			else{
				pneumatic_control(GPIOB, GPIO_Pin_8, 0);
				pneumatic_control(GPIOB, GPIO_Pin_5, 0);
				racketswing++;
			}
			
			uart_tx_byte(COM3, 'h');
			break;
	}
}



























/////-----VVVVVVVVVV-----///// 
//---TRANSITION BETWEEN AUTOZONE AND MANUALZONE---//

//If autozone code does not work, just try running stabiliser() on a while(autoormanual == 0) loop
int autozone(void){
	autoormanual = 0;
	
	while(autoormanual == 0){		
		if(get_ms_ticks()%50 == 0){
			get_angle();
			
			//between -90deg to 90deg
			if(angle > -1570.79632679 && angle < 1570.79632679){ 
				get_angle();
				stabiliser();
				motor_control(1,1,rightspeedratio*motormag);
				motor_control(2,1,leftspeedratio*motormag);
			}
			
			//90deg left
			else if(angle > -1580.79632679 && angle < -1560.79632679){ 
				int ticksi = get_ms_ticks();
				rightspeedratio = 0.6;
				leftspeedratio = 0.8;
				motor_control(1,0,0);
				motor_control(2,0,0);
				
				//wait 1 sec
				while(get_ms_ticks() - ticksi < 1000){ 
				}
				
				while(1){
					get_angle();
					
					if(angle <= 0.174533 && angle >= -0.174533){
						break;
					}
					
					motor_control(1,0,motormag*rightspeedratio); //right wheel backward
					motor_control(2,1,motormag*leftspeedratio); //left wheel forward
				}
			}
			
			//90deg right
			else if(angle < 1580.79632679 && angle > 1560.79632679){ 
				int ticksi = get_ms_ticks();
				rightspeedratio = 0.6;
				leftspeedratio = 0.8;
				motor_control(1,0,0);
				motor_control(2,0,0);
				
				//wait 1 sec
				while(get_ms_ticks() - ticksi < 1000){ 
				}
				
				while(1){
					get_angle();
					
					if(angle <= 0.174533 && angle >= -0.174533){
						break;
					}
					
					motor_control(1,1,motormag*rightspeedratio); //right wheel forward
					motor_control(2,0,motormag*leftspeedratio); //left wheel backward
				}
			}
			
			//else if for arc and 135deg
		}
	}
}

int manualzone(void){
	autoormanual = 1;
	
	while(autoormanual == 1){
		uart_interrupt(COM3);
	}
}