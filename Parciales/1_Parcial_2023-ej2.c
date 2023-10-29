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
    	 NVIC_DisableIRQ(EINT3_IRQn); // deshabilito las interrpciones
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

/*
#include <LPC17xx.h>

// Declaración de variables
uint8_t numbers[10] = {0};  // Arreglo para almacenar los últimos 10 números
uint32_t interruptPriority = 0;  // Prioridad inicial de interrupción
uint32_t interruptCount = 0;  // Contador de interrupciones

// Función para actualizar la prioridad de interrupción
void updateInterruptPriority() {
    interruptCount++;
    
    if (interruptCount % 200 == 0) {
        if (interruptPriority < 7) {
            interruptPriority++;
            NVIC_SetPriority(EINT3_IRQn, interruptPriority);  // Actualizar prioridad
        } else {
            // Deshabilitar interrupciones por GPIO
            NVIC_DisableIRQ(EINT3_IRQn);
        }
    }
}

// Función de inicialización de GPIO
void initGPIO() {
    LPC_GPIOINT->IO2IntEnF |= (1 << 10) | (1 << 11) | (1 << 12) | (1 << 13);  // Configurar pines P2.10 a P2.13 para detección de flanco descendente
    NVIC_EnableIRQ(EINT3_IRQn);  // Habilitar interrupción externa
    NVIC_SetPriority(EINT3_IRQn, interruptPriority);  // Establecer prioridad de interrupción
}

// Función de manejo de interrupción
void EINT3_IRQHandler(void) {
    // Leer los valores de los 4 pines de entrada GPIO
    uint8_t inputNumber = ((LPC_GPIO2->FIOPIN >> 10) & 0x0F);

    // Mover los números en el arreglo para hacer espacio para el nuevo número
    for (int i = 9; i > 0; i--) {
        numbers[i] = numbers[i - 1];
    }

    // Almacenar el nuevo número en el arreglo
    numbers[0] = inputNumber;

    // Actualizar la prioridad de interrupción
    updateInterruptPriority();

    // Borrar la bandera de interrupción
    LPC_GPIOINT->IO2IntClr |= (1 << 10) | (1 << 11) | (1 << 12) | (1 << 13);
}

int main() {
    // Inicialización del sistema
    SystemInit();
    
    // Inicialización de GPIO
    initGPIO();
    
    while (1) {
        // El programa continúa ejecutándose
        // Las interrupciones por GPIO manejarán la entrada de números
    }
    
    return 0;
}

*/