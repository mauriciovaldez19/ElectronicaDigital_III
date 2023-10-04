/*Generar una señal de periodo T2. Con T1=T2/2 y n es un numero que ingresa por 4 pines del GPIO.
Cuando n=0, la señal es cero y cuando n=15 -->T1=T2
T2=50 ms, además cuando se ingresa una señal externa por algun pin de interrupcion, se saca la secuencia
(0xA0A3) con la caracteristica que los 1 duran 10ms y los 0 duran 5 ms*/
#include "LPC17xx.h"

void configGPIOInt(void);
void configTimer(void);

uint8_t n=0;

uint32_t T1=2499999;	//25 ms para 100 Mhz valor que inicio por defecto
uint32_t T2=4999999;	//50 ms para 100 Mhz

int main(void) {
	configGPIOInt();
	configTimer();
	while(1){
    	 	}
    return 0 ;
}

void EINT3_IRQHandler(void){
	n = ((LPC_GPIO0->FIOPIN & (0xF)) | LPC_GPIO0->FIOPIN); //leo el puerto y lo guardo
	if (n==0){
		LPC_TIM0 ->EMR	&=~(1<<0); 	//pag 509 external match0.0 comienza en low
		LPC_TIM0 ->TCR		  =3; //pag505
		LPC_TIM0 ->TCR		 &=~(1<<1);

	}
	else if(n==15){
		T1=T2;
		LPC_TIM0 ->EMR	|=(1<<0);//external match0.0 comienza en high
		LPC_TIM0 ->TCR		  =3; //pag505
		LPC_TIM0 ->TCR		 &=~(1<<1);

	}
	else{
		T1=T2/n;
		LPC_TIM0 ->EMR	|=(1<<0);//external match0.0 comienza en high
		LPC_TIM0 ->TCR		  =3; //pag505
		LPC_TIM0 ->TCR		 &=~(1<<1);

	}

	LPC_GPIOINT->IO0IntClr |=(0xF); //limpio los flags de interrupcion

}
void configTimer(void){
	LPC_SC ->PCONP 		 |=(1<<1); 	//pag65		power on timer
	LPC_SC ->PCLKSEL0 	 |=(1<<2); 	//pag 59		cclk divisor
	LPC_PINCON ->PINSEL3 |=(3<<24); //pag 120	funcion pin math0.0
	LPC_PINCON ->PINSEL3 |=(3<<26);	//funcion pin match0.1
	LPC_TIM0 ->EMR		 |=(1<<0); 	//pag 509 external match0.0 comienza en high
	LPC_TIM0 ->EMR		 |=(1<<4); 	//pag 509 pone 0 a la salida cuando hay match0.0

	LPC_TIM0 ->MR0 		  =T1;	//valor match0.0
	LPC_TIM0 ->MR1		  =T2;	//valor match0.0

	LPC_TIM0 ->MCR		  =(2<<3); //pag 507 resetea en match0.1
	LPC_TIM0 ->TCR		  =3; //pag505
	LPC_TIM0 ->TCR		 &=~(1<<1);

	return;
}
void configGPIOInt(void){
	LPC_PINCON->PINSEL0  &=~(0xFF); //P0.0 - P0.3 como GPIO
	LPC_PINCON->PINMODE0 &=~(0xFF); // P0.0 - P0.3 como entrada
	LPC_GPIO0->FIODIR 	 &=~(0xF);//P0.0 ->LSB to P0.3->MSB  input
	LPC_GPIO0 ->FIOMASK   =~(0x0000000F); // enmascaro el puerto.

	// habilito interrupciones por flanco ascedente  y descendente
	LPC_GPIOINT->IO0IntEnF |= (0xF);
	LPC_GPIOINT->IO0IntEnR |= (0xF);

	//limpio los flags de interrupcion
	LPC_GPIOINT->IO0IntClr |=(0xF);

	NVIC_SetPriority(EINT3_IRQn,3);
	NVIC_EnableIRQ(EINT3_IRQn);
	return;
}
