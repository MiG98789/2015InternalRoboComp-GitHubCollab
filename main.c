#include "main.h"

int autozone()
{
	array_init();
	int count = 0;
	
	while(1){		
		if(count != get_ms_ticks()){
			count = get_ms_ticks();	
			if(count % 10 == 0){
				linear_ccd_clear();
				linear_ccd_read();
				linear_ccd_print();
			}
		}
	}	
}

int main()
{
	init();
	
	while (true)
	{
		if (!read_button(0))
		{
			while (!read_button(0)){
			
			}
			
			// ...
		}
		
		systemPolling();
	}
	
}
