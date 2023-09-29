#include "LPC17xx.h"

void configPin(void);
void configIntGPIO(void);
uint8_t dato [16];
int aux=0;
int main(void){
	configPin();
	configIntGPIO();

	while(1){

	}

	return 0;
}
void EINT3_IRQHandler(void){
	if (LPC_GPIOINT->IO2IntStatF & (0xFF) || LPC_GPIOINT->IO2IntStatR & (0xFF))
	{
		dato[aux]= LPC_GPIO2->FIOPIN &(0xFF);
		aux++;
		if(aux>16){
			aux=0;
		}
		LPC_GPIOINT->IO2IntClr |= (0xFF);

	}
}
void configPin(void){
	LPC_PINCON -> PINSEL4 &= ~(0xFFFF);
	LPC_GPIO2->FIODIR &=~(0xFF);
	return;
}
void configIntGPIO(void){
	LPC_GPIOINT->IO2IntEnR |= (0xFF);
	LPC_GPIOINT->IO2IntEnF |= (0xFF);

	NVIC_EnableIRQ(EINT3_IRQn);
	return;
}
