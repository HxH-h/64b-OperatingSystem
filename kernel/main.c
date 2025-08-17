# include "./console/console.h"
# include "./memory/memory.h"

void init();

int main(){
	init();
    print("Hello OS!\n");
    

    
    while (1){
        
    }
    
    return 0;
}


void init(){
	init_console(DARK);
    init_memory();
}