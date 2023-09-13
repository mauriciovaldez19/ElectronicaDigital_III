#include "LPC17xx.h"

void configPin(void);
void configInt(void);
uint8_t values[8];
uint8_t aux=0;

int main(void){
	configPin();
	configInt();
	while(1);

	return 0;
}
void configPin(void){
	LPC_PINCON->PINSEL0 &=~(0xFFFF); //P0 as GPIO
	LPC_PINCON->PINSEL4 &=~(1<<22);
	LPC_PINCON->PINSEL4 |=(1<<23);
	LPC_GPIO0->FIODIR |=(0xFF);

	return;
}
void configInt(void){
	LPC_SC->EXTMODE |=1<<1;
	LPC_SC->EXTPOLAR |=1<<1;
	LPC_SC->EXTINT |=1<<1;
	NVIC_EnableIRQ(EINT1_IRQn);
	NVIC_SetPriority(EINT1_IRQn,0);

	SysTick->LOAD=(619999);
	SysTick->CTRL= 1 | 1<<1 | 1<<2;
	SysTick->VAL=0;

	NVIC_SetPriority(SysTick_IRQn,2);
	return 0;
}
void EINT1_IRQHandler(void){
	if(aux==0){
		SysTick->CTRL |=(1<<1);
		aux++;
	}
	else{
		SysTick->CTRL &=~(1<<1);
	}
LPC_SC->EXTINT |=1<<1;
}
void SysTick_Handler(void){
	uint8_t promedio=0;
	int i=0;
	int suma=0;
	while(i<8){
		suma=suma+values[i];
		i++;
	}
	promedio=suma/8;
	LPC_GPIO0->FIOPIN = (LPC_GPIO0->FIOPIN & promedio) | promedio;
	SysTick->CTRL &=SysTick->CTRL;
}
