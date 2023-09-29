/*Realizar un programa que configure el
 * puerto P0.0 y P2.0 para que provoquen una interrupción
 * por flanco de subida. Si la interrupción es por P0.0
 * guardar el valor binario 100111 en la variable "auxiliar",
 * si es por P2.0 guardar el valor binario 111001011010110.
 */
#include "LPC17xx.h"

void configGPIO(void);
void configIntGPIO(void);
uint8_t interrupcion1=0;;
uint8_t	interrupcion2=0;
uint16_t auxiliar=0;

int main(void){
	configGPIO();
	configIntGPIO();
	while(1){
		if(interrupcion1==1){
			auxiliar = 0b100111;
			interrupcion1=0;
		}
		else if(interrupcion2==1){
			auxiliar = 0b111001011010110;
			interrupcion2=0;
		}
	}
	return 0;
}
void configGPIO(void){
	LPC_PINCON->PINSEL0 &= ~(3); //P0.0 as GPIO
	LPC_PINCON->PINSEL4 &= ~(3); //P2.0 as GPIO

	LPC_GPIO0-> FIODIR &= ~(1<<0);
	LPC_GPIO2-> FIODIR &= ~(1<<0);

	return;
}
void configIntGPIO(void){
	LPC_GPIOINT->IO0IntEnR |= 1;
	LPC_GPIOINT->IO2IntEnR |= 1;

	LPC_GPIOINT->IO0IntClr |= 1;
	LPC_GPIOINT->IO2IntClr |= 1;
	NVIC_EnableIRQ(EINT3_IRQn);
	return;
}
void EINT_IRQHandler(void){
	if(LPC_GPIOINT->IO0IntStatR & 1){
		interrupcion1++;
		LPC_GPIOINT->IO0IntClr |=1;
	}
	else if(LPC_GPIOINT->IO2IntEnR & 1){
		interrupcion2++;
		LPC_GPIOINT->IO2IntClr |=1;
	}

	return;
}
