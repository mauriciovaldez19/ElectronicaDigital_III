/*
 * Usar los pines 8 y 9 del puerto0 como entrada
 * y el pin22 del mismo puerto como salida.
 *
 * si interrumpe con el pin 8 muestra la secuencia 1
 * si interrumpe por el pint 9 muestra la secuencia 2
 * */


#include "LPC17xx.h"
#define INPUT1 1<<8
#define INPUT2 1<<9
#define OUTPUT 1<<22

void configGPIO(void);
void configIntGPIO(void);
void delay(void);
int secuencia1[9]={0,1,0,0,1,1,0,1,0};
int secuencia2[9]={0,1,1,1,0,0,1,1,0};

int main(void){
	configGPIO();
	configIntGPIO();
	LPC_GPIO0->FIOSET |= OUTPUT;

	while(1){
		//do nothing
	}

	return 0;
}

void EINT3_IRQHandler(void){
	if(LPC_GPIOINT->IO0IntStatF & INPUT1){
		for(int i=0;i<10;i++){
			LPC_GPIO0->FIOPIN |= OUTPUT & secuencia1[i];
			delay();
		}
		LPC_GPIOINT->IO0IntClr |= INPUT1;
	}
		else if (LPC_GPIOINT->IO0IntStatF & INPUT2)
		{
			for(int i=0;i<10;i++){
			LPC_GPIO0->FIOPIN |= OUTPUT & secuencia2[i];
			delay();
		}
		LPC_GPIOINT->IO0IntClr |= INPUT2;
		}
}

void configGPIO(void){
	LPC_PINCON->PINSEL0 &=~(0xCF<<15); //P0.8 P0.9 P0.22 as GPIO
	LPC_GPIO0->FIODIR &=~(INPUT1);
	LPC_GPIO0->FIODIR &=~(INPUT2);
	LPC_GPIO0->FIODIR |=(OUTPUT);

	return;
}

void configIntGPIO(void){
	LPC_GPIOINT->IO0IntEnF |= INPUT1;
	LPC_GPIOINT->IO0IntEnF |= INPUT2;

	LPC_GPIOINT->IO0IntClr |=INPUT1;
	LPC_GPIOINT->IO0IntClr |=INPUT2;
	NVIC_EnableIRQ(EINT3_IRQn);
	return;
}
void delay(void){
	for(int j=0;j<6000;j++)
	{
		for(int k=0;k<6000;k++){}
	}

}
