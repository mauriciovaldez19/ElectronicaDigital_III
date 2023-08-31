#include "LPC17xx.h"

void delay(uint32_t tiempo);

int main (void){
	uint32_t clk = SystemCoreClock;
	//configuro pin 22 del port 0 para ver el led
	LPC_PINCON->PINSEL1 &= ~(0b11<<12);
	LPC_GPIO0->FIODIR2 |=(1<<6);

	//configuro el pin 9 del port 0 para enviar un pulso
	LPC_PINCON->PINSEL0 &= ~(0b11<<18);
	LPC_PINCON->PINMODE0 &= ~(0b11<<18);
	LPC_GPIO0->FIODIR &= ~(1<<9);

	uint32_t tiempo;
	    while(1){
	    			if((LPC_GPIO0->FIOPIN>>9)&0x01)
	    					{
	    				tiempo = 5000000;
	    					}
	    			else
	    			{
	    				tiempo = 1000000;
	    			}
	    	LPC_GPIO0->FIOCLR |= (1<<22);
	    	delay(tiempo);
	    	LPC_GPIO0->FIOSET |= (1<<22);
	    	delay(tiempo);

	    }

	    return 0 ;
	}

	void delay(uint32_t tiempo){
		uint32_t counter;
		for(counter = 0; counter<tiempo; counter++){};



	return 0;
}
