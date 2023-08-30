#include "LPC17xx.h"

void delay(void);


int main(void) {
/*
 * &= and bit a bit y ~ es negación
 * |= or inclusiva bit a bit
 * */

    LPC_PINCON->PINSEL1 &= ~(0b11<<12);  //00 en 13 y 12 - Puerto 0 pin 22
    LPC_GPIO0->FIODIR2 |=(1<<6);

    while(1){

    	LPC_GPIO0->FIOCLR |= (1<<22);
    	delay();
    	LPC_GPIO0->FIOSET |= (1<<22);
    	delay();

    }

    return 0 ;
}

void delay(void){
	uint32_t counter;
	for(counter = 0; counter<6000000; counter++){};

}

