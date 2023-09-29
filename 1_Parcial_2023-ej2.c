/*Utilizando interrupciones por GPIO realizar un codigo en C que permita, mediante
 * 4 pines de entrada GPIO, leer y guardar un numero compuesto por 4bits. Dicho numero
 * puede ser cambiado por un usuario mediante 4 switches, los cuales cuentan
 * con sus respectivas resistencias pull-up externas. El almacenamiento debe realizase en una
 * variable tipo array de forma tal que se asegure tener disponible siempre los ultimos 10 numeros
 * elegidos por el usuario, garantizando ademas que el numero ingresado mas antiguo,
 * de este conjunto de 10 se encuentre en el elemento 9 y el numero actual en el elemento 0 de dicho array.
 * La interrupcion por GPIO empezará teniendo la maxima prioridad de interrupcion posible y cada 200 numeros
 * ingresados debera disminuir en 1 su prioridad hasta alcanzar la minima posible.
 * LLegado este momento, el programa deshabilitara todo tipo de interrupciones
 * producidas por las entradas GPIO. Tener en cuenta que el codigo debe estar debidamente comentado*/

#include "LPC17xx.h"

void configPin(void);
void configGPIOInt(void);
int8_t aux=9;
uint8_t arreglo[10];
uint8_t nPrioridad=0;
uint16_t nInterrup=1;

int main(void) {
	configPin();
	configGPIOInt();
    while(1){
    	 	}
    return 0 ;
}

void EINT3_IRQHandler(void){

	if(nInterrup % 200){	//cada 200 interrupciones va a aumentar en 1 la prioridad
		nInterrup++;
}
	else{
		nPrioridad++;
		NVIC_SetPriority(EINT3_IRQn,nPrioridad);
}
	if (nPrioridad<32){  //chequeo q la prioridad llegue has 31 inclusive
		if (aux<0){ aux=9;	} //reseteo la posicion del almacenamiento

		arreglo[aux] = ((LPC_GPIO0->FIOPIN & (0xF)) | LPC_GPIO0->FIOPIN); //leo el puerto y lo guardo en el arreglo
		aux--;
		//limpio los flags de interrupcion
		LPC_GPIOINT->IO0IntClr |=(0xF);
}
     else{
    	 NVIC_DisableIRQ(EINT3_IRQn); // desabilito las interrpciones
}}

void configPin(void){
LPC_PINCON->PINSEL0 &=~(0xFF); //P0.0 - P0.3 as GPIO
//LPC_PINCON->PINMODE0 =(1<<1)|(1<<3)|(1<<5)|(1<<7); //neither pull-up nor pull-down
LPC_PINCON->PINMODE0 &=~(0xFF); //pull-up para testear

LPC_GPIO0->FIODIR &=~(0xF);//P0.0 ->LSB to P0.3->MSB  input
LPC_GPIO0 ->FIOMASK =~(0x0000000F); // enmascaro el puerto.
	return;
}
void configGPIOInt(void){
//todas las entradas configuradas para interrumpir por flanco ascendente y descendente
LPC_GPIOINT->IO0IntEnF |= (0xF);

LPC_GPIOINT->IO0IntEnR |= (0xF);

//limpio los flags de interrupcion
LPC_GPIOINT->IO0IntClr |=(0xF);

NVIC_SetPriority(EINT3_IRQn,0);
NVIC_EnableIRQ(EINT3_IRQn);
	return;
}
