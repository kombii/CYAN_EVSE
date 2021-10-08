#include	"delay.h"
void delay_ms(unsigned char ms)
{
	unsigned int i;
	do{
		i = MAIN_Fosc / 1000;
		while(--i);
	}while(--ms);
}
