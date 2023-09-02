/*
 * Una famosa empresa de calzados a incorporado a sus zapatillas 10 luces leds
 * comandadas por un microcontrolador LPC1769 y ha pedido a su grupo de ingenieros
 * que diseñen 2 secuencias de luces que cada cierto tiempo se vayan intercalando
 * (secuencia A - secuencia B- secuencia A- ... ).
 * Como todavía no se ha definido la frecuencia a la cual va a funcionar
 * el CPU del microcontrolador, las funciones de retardos que se incorporen
 * deben tener como parametros de entrada variables que permitan modificar el
 * tiempo de retardo que se vaya a utilizar finalmente. Se pide escribir el código
 * que resuelva este pedido, considerando que los leds se encuentran conectados
 * en los puertos P0,0 al P0.9
 */

#include "LPC17xx.h"

#define PIN0	(1)
#define PIN1	(1<<1)
#define PIN2	(1<<2)
#define PIN3	(1<<3)
#define PIN4	(1<<4)
#define PIN5	(1<<5)
#define PIN6	(1<<6)
#define PIN7	(1<<7)
#define PIN8	(1<<8)
#define PIN9	(1<<9)


void configGPIO(void);
void delay_secuencia(uint32_t times);
void delay(uint32_t times);
void secuencia1(void);
void secuencia2(void);

int main(void){

	configGPIO();

	while(1){
	secuencia1();
	delay_secuencia(1000);
	secuencia2();
	delay_secuencia(1000);
	}

	return 0;
}

void configGPIO(void){
	//PINSEL
	LPC_PINCON->PINSEL0 &= ~(0x3FFFF);
	//FIODIR
	LPC_GPIO0->FIODIR |= PIN0;
	LPC_GPIO0->FIODIR |= PIN1;
	LPC_GPIO0->FIODIR |= PIN2;
	LPC_GPIO0->FIODIR |= PIN3;
	LPC_GPIO0->FIODIR |= PIN4;
	LPC_GPIO0->FIODIR |= PIN5;
	LPC_GPIO0->FIODIR |= PIN6;
	LPC_GPIO0->FIODIR |= PIN7;
	LPC_GPIO0->FIODIR |= PIN8;
	LPC_GPIO0->FIODIR |= PIN9;
}
void delay_secuencia(uint32_t times){
	for(uint32_t i=0; i<times;i++)
		for(uint32_t j=0; j<times; j++);
}

void delay(uint32_t times){
	for(uint32_t i=0; i<times;i++)
		for(uint32_t j=0; j<times; j++);
}

void secuencia1(void){
	LPC_GPIO0->FIOSET |= PIN0;
	delay(1000);
	LPC_GPIO0->FIOCLR |= PIN0;
	LPC_GPIO0->FIOSET |= PIN1;
	delay(1000);
	LPC_GPIO0->FIOCLR |= PIN1;
	LPC_GPIO0->FIOSET |= PIN2;
	delay(1000);
	LPC_GPIO0->FIOCLR |= PIN2;
	LPC_GPIO0->FIOSET |= PIN3;
	delay(1000);
	LPC_GPIO0->FIOCLR |= PIN3;
	LPC_GPIO0->FIOSET |= PIN4;
	delay(1000);
	LPC_GPIO0->FIOCLR |= PIN4;
	LPC_GPIO0->FIOSET |= PIN5;
	delay(1000);
	LPC_GPIO0->FIOCLR |= PIN5;
	LPC_GPIO0->FIOSET |= PIN6;
	delay(1000);
	LPC_GPIO0->FIOCLR |= PIN6;
	LPC_GPIO0->FIOSET |= PIN7;
	delay(1000);
	LPC_GPIO0->FIOCLR |= PIN7;
	LPC_GPIO0->FIOSET |= PIN8;
	delay(1000);
	LPC_GPIO0->FIOCLR |= PIN8;
	LPC_GPIO0->FIOSET |= PIN9;
	delay(1000);
	LPC_GPIO0->FIOCLR |= PIN9;
}
void secuencia2(void){
	for(int k=0; k<10;k++){
		LPC_GPIO0->FIOSET |= PIN0;
		LPC_GPIO0->FIOSET |= PIN1;
		LPC_GPIO0->FIOSET |= PIN2;
		LPC_GPIO0->FIOSET |= PIN3;
		LPC_GPIO0->FIOSET |= PIN4;
		LPC_GPIO0->FIOSET |= PIN5;
		LPC_GPIO0->FIOSET |= PIN6;
		LPC_GPIO0->FIOSET |= PIN7;
		LPC_GPIO0->FIOSET |= PIN8;
		LPC_GPIO0->FIOSET |= PIN9;
		delay(1000);
		LPC_GPIO0->FIOCLR |= PIN0;
		LPC_GPIO0->FIOCLR |= PIN1;
		LPC_GPIO0->FIOCLR |= PIN2;
		LPC_GPIO0->FIOCLR |= PIN3;
		LPC_GPIO0->FIOCLR |= PIN4;
		LPC_GPIO0->FIOCLR |= PIN5;
		LPC_GPIO0->FIOCLR |= PIN6;
		LPC_GPIO0->FIOCLR |= PIN7;
		LPC_GPIO0->FIOCLR |= PIN8;
		LPC_GPIO0->FIOCLR |= PIN9;
		delay(1000);
	}

}
