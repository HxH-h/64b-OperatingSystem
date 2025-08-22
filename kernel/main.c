# include "./console/console.h"
# include "./memory/memory.h"
# include "./interrupt/intr.h"
# include "./device/device.h"


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
    init_intr();
    // 开中断
    enable_intr();
    print("enable intr finish\n");

    init_device();

}