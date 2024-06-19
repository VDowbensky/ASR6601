#include <stdio.h>
#include "lowlevel_retarget.h"
#include "UART_APB.h"

int main()
{
	UartStdOutInit();
	printf("STAR: Hello World!\n");
	while(1)
	{
		printf("Input char is: %c \n", APB_UART_GetChar());
	}
	
}	
