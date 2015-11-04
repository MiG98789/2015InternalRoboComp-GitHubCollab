#include "linear_ccd.h"

u32 linear_ccd_buffer1[128];
u32 linear_ccd_buffer2[128];
u8 flag = 0;
u32 road_pos = 0;
u8 l_edge = 0;
u8 r_edge = 0;
u8 mid = 0;

void delay_57(void){
	for(int i = 0;i <4;++i){
		__asm("nop");
	}
}

void CLK(u8 state){//self make clock to control two ccd

		if (state == 1){
	GPIO_SetBits(CLK_PORT, CLK_PIN);
		state=0;
	}
	else if (state == 0){
	GPIO_ResetBits(CLK_PORT,CLK_PIN);
		state=1;
	}
}


void SI(u8 bit){ //controlling Linear_ccd 1  and 2 si1 si2

	if(bit==1){
	GPIO_SetBits(SI1_PORT,SI1_PIN);
	//GPIO_SetBits(SI2_PORT,SI2_PIN);
	}

	else{
		GPIO_ResetBits(SI1_PORT,SI1_PIN);
		//GPIO_ResetBits(SI2_PORT,SI2_PIN);
	}
}

	


u32 AO1(){  // getting data from ccd1 ao1
	u32 temp = 0;
	int get_times = 250;
for(u8 i=0;i<get_times;i++){
temp += get_adc(1);
}
temp=(u32)(temp/get_times);
return temp;
}

u32 AO2(){  // getting data from ccd1 ao1
	u32 temp;
	int get_times = 250;
for(u8 i=0;i<get_times;i++){
temp += get_adc(2);
}
temp=(u32)(temp/get_times);
return temp;
}


void linear_ccd_read(){
//	SI(0);
//	CLK(0);
//	_delay_us(1);
	SI(1);
	delay_57();
	CLK(1);
	SI(0);
	linear_ccd_buffer1[0]=AO1();
	delay_57();
	CLK(0);
	for(u8 n=1;n<128;n++)
	{
		CLK(1);		
		delay_57();
		linear_ccd_buffer1[n]=AO1();
		CLK(0);
		delay_57();
		//linear_ccd_buffer2[n]=AO2();
		
	}
	CLK(1);		
	delay_57();
	CLK(0);
	delay_57();	 
	
		for(u8 y=0;y<128;y++)
	{
	linear_ccd_buffer1[y] = (linear_ccd_buffer1[y])*160*2 / 4095;
	 if(linear_ccd_buffer1[y] >= 160){
		linear_ccd_buffer1[y] = 159;
	}
}


}

void linear_ccd_init()
{//initialization of clk 
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = CLK_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(CLK_PORT, &GPIO_InitStructure);
	
//initialization of si1
	GPIO_InitStructure.GPIO_Pin = SI1_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(SI1_PORT, &GPIO_InitStructure);

//initialization of si2
	GPIO_InitStructure.GPIO_Pin = SI2_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(SI2_PORT, &GPIO_InitStructure);
}

void linear_ccd_print(void){
	check_angle();
}


void linear_ccd_clear(void){

}

int mean_array[10];
int ma = 64;

void array_init(void){
	for(int i = 0; i < 10; i++){
		mean_array[i] = 64;
	}
}

int get_road_pos(void){
	int sum = 0;
	int count = 0;
	int road_pos = 0;
	
	for(int i = 0; i < 128; i++){
		if(linear_ccd_buffer1[i] <= 30){
			//tft_put_pixel(i, linear_ccd_buffer1[i], WHITE);
			sum += i;
			count++;
			}
	}
	if(count == 0){
	road_pos = -1;
	}
	else{
	road_pos = sum/count;
	}
	
	return road_pos;
}

int get_moving_average(void){
	int first_val = mean_array[0];
	
	for(int i = 0; i < 9; i++){
		mean_array[i] = mean_array[i+1];
	}
	
	mean_array[9] = get_road_pos();
	
	ma += (mean_array[9] - first_val)/10;
	
	return ma;
}

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
	double wheelbase = 0;
	
	//if turning right
	//rightspeed = leftspeed*(1-wheelbase)/(1+wheelbase);
	
	//else
	// leftspeed = rightspeed*(1-wheelbase)/(1+wheelbase);
}
