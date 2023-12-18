

/*
===============================================================================
 Name        : Eje12_4_.c
 Author      : $(FerG)
 Version     :
 Copyright   : $(copyright)
 Description : main definition
===============================================================================
*/

#ifdef __USE_CMSIS
#include "LPC17xx.h"
#endif

#include <cr_section_macros.h>

#include <stdio.h>

void confADC();

int main(void) {
	SystemInit();
	confADC();
	while(1){}
    return 0 ;
}

void confADC(){

	LPC_SC->PCONP |= (1<<12); //enciende el periferico ADC
	LPC_ADC->ADCR |= (1<<21);//PDN A/D converter is oprational
	LPC_SC->PCLKSEL0 |= (3<<24); //Seleccionamos el PCLK=CCLK/8 =12.5Mhz
	LPC_PINCON->PINSEL1 = (1<<18);//Usamos todos por defecto menos el p0.25 en modo ADC0.2
	LPC_PINCON->PINSEL4 = (1<<20);//Usamos todos por defecto menos el p2.10 en EINT0
	LPC_PINCON->PINMODE1 = (1<<19);//Todas por defecto menos el p0.25 ni pull up ni pull down
	LPC_PINCON->PINMODE4 = 0;
	LPC_ADC->ADCR = (1<<2); //uso el AD2 ademas de poner todo lo demas en 0
	LPC_ADC->ADCR &= (7<<24);
	LPC_ADC->ADCR |= (1<<25);//Que inicie por EINT0
	LPC_ADC->ADCR |= (1<<27); //por flanco descendente
	LPC_ADC->ADINTEN |= (1<<2);
	NVIC_EnableIRQ(ADC_IRQn);

}

void ADC_IRQHandler (){
	static uint16_t valorBinario=0;
	static float voltADC =0;
	valorBinario=((LPC_ADC->ADDR2>>4) & 0xFFF);
	voltADC= (valorBinario/4096)*3.3;
	return;
}


