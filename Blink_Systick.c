/*Blink led con uso de Systick
el retardo es c/ 1 segundo con clock interno
de 100Mhz
*/
#include "LPC17xx.h"

void configSystick (void);
uint32_t aux =0;

int main(void) {
	configSystick();
    LPC_PINCON->PINSEL1 &= ~(0b11<<12);  //00 en 13 y 12 - Puerto 0 pin 22
    LPC_GPIO0->FIODIR2 |=(1<<6);
uint32_t time= SystemCoreClock;
    while(1){
    	if(aux<20){

    	  	if(aux<10){
    		LPC_GPIO0->FIOCLR |=(1<<22);
    	}
    	else{
    		LPC_GPIO0->FIOSET |=(1<<22);
    	}

    	}
    	else{
    		aux=0;
    	}
    	}
    return 0 ;
}
void SysTick_Handler(void){
	aux++;
	SysTick->CTRL &= SysTick->CTRL; //Clear flag
}

void configSystick (void){
	SysTick->LOAD = ((SystemCoreClock/10)-1);
	SysTick->CTRL=(0x7);
	SysTick->VAL = 0;
	NVIC_SetPriority(SysTick_IRQn,5);
}
