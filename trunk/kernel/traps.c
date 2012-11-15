/**
 * 硬件中断代码
 */
#include <asm/system.h>

//void divide_error(void);

static void die(char *str,long esp_ptr,long nr){
	long * esp =(long *) esp_ptr;
}


void do_divide_error(long esp, long error_code)
{
	die("divide error",esp,error_code);
}

void trap_init(){
	//set_trap_gate(0,&divide_error);

}
