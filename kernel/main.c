# include "./console/console.h"



void init();

int main(){
	init();
    print_color("Hello %s World!\n %d",COLOR_GREEN , "HXH" , 123);
	
    while (1){
        
    }
    
    return 0;
}


void init(){
	init_console(COLOR_WHITE);
}