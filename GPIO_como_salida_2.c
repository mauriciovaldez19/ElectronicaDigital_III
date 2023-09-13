/*
 * En los pines P2.0 a P2.7 se encuentra conectado un display de 7 segmentos.
 * Utilizando la variable
 * numDisplay [10] ={0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x67}
 * que codifica los números del 0 a 9 para ser mostrados en el display, realizar un
 * programa que muestre indefinidamente la cuenta de 0 a 9 en dicho display.
 *
 */

#include "LPC17xx.h"

void configGPIO(void);
void delay(uint32_t times);

int main(void){
	configGPIO();
	int numDisplay [10] ={0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x67};
	int k;
	while(1){
			for(k=0;k<10;k++){
			LPC_GPIO2->FIOCLR |= (0xFF);
			LPC_GPIO2->FIOSET |= numDisplay[k];
			delay(1000);
							}
			}

	return 0;
}

void configGPIO(void){
	//PINSEL ->selecciono el modo
	LPC_PINCON->PINSEL4 &=~(0xFFFF);
	//FIODIR -> in / out
	LPC_GPIO2->FIODIR |= (0xFFFF);
}
void delay(uint32_t times){
	for(uint32_t i=0;i<times;i++)
		for(uint32_t j=0;j<times;j++);

}
