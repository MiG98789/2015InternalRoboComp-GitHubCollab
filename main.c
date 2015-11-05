#include "main.h"

int main(void){
	tft_init(0, BLACK, WHITE, WHITE);
	linear_ccd_init();
	adc_init();
	ticks_init();
	LED_INIT();
	int mean = 0;
	int mean_array[10];
	for(int i = 0; i < 10; i++){
		mean_array[i] = 64;
	}
	
		while(1){
			
			int count = 0;
			int sum = 0;
			
				if((get_ms_ticks()%50==0)&&(get_ms_ticks()%100 !=0)){
					for(int i = 0; i < 128; i++){
						tft_put_pixel(i, linear_ccd_buffer1[i], BLACK);
					}
					for(int j = 0; j < 160; j++){
						tft_put_pixel(mean, j, BLACK);
					}
				}
				else if(get_ms_ticks()%100 == 0){
					
					linear_ccd_read();
					
					for(int i = 0; i < 128; i++){
						if(linear_ccd_buffer1[i]>=150){
							tft_put_pixel(i, linear_ccd_buffer1[i], WHITE);
							sum = sum + i;
							count++;
						}
						else{
							tft_put_pixel(i, linear_ccd_buffer1[i], BLUE);
						}
					}
					
					mean = sum/count;		
					
					for(int j = 0; j < 160; j++){
						tft_put_pixel(mean, j, WHITE);
					}
				}
		}
return 0;
}

