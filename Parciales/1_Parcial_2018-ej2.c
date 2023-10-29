/* Ejercicio 2 del 1° parcial del 2018
Resumen consigna
-interrumpor por SysTick cada 5 [mS]
-por cada interrupcion testear el P0.1, si hay 3 estados bajos seguidos entonces se envía por el P2 el valor guardado
en la posicion de memoria 0x10002000 y se desactivan las interrupciones por SysTick
- por diseño, el sistema de reloj funciona con el oscilador interno RC y PLL0 apagado y en bypass y division po 1 del reloj del CPU

Analisis y calculos
El pin P0.1 debe ser configurado como GPIO y de entrada
el P2 tmb como GPIO pero de salida
El oscilador IRC funciona normalmente a 4 MHz
para calcular el valor del Registro VALUE del SysTick para 5 mS
(5e-3*4e6)-1=19999 ticks

*/

#include "LPC17xx.h"

void cfgPin (void);
void cfgSystick(void);
uint32_t *ptr = (uint32_t*) 0x10002000; //decalaro un puntero al cual le asigno el valor guardado en la direccion de memoria dada
uint32_t valor=(*ptr); //le guardo el valor del puntero en la variable valor

int main (void){

    cfgPin();
    cfgSystick();
    while(1){}

    return 0;
}
void cfgPin(void){
    /* se considera que el microcontrolador al salir del reset, 
    los pines tienen la configuracion por defecto GPIO y resistencias de Pull-Up */
    LPC_GPIO0 -> FIODIR &= ~(1<<1); //P0.1 as input
    LPC_GPIO0 -> FIOMASK |=(0xFFFFFFFD);
    LPC_GPIO2 -> FIODIR  |=(0xFFFFFFFF); //P2 as output
    return;
}
void cfgSystick(void){
    SysTick ->LOAD = 19999;
    SysTick ->CTRL |=(0x7);
    SysTick ->VAL = 0;
    SysTick ->CTRL &= SysTick -> CTRL; //de esta manera leo y bajo el flag
    return;
}
void SysTick_Handler(void){
static uint8_t counter = 0;
    if ((LPC_GPIO0->FIOPIN & (0x2)) == 0){
        counter++;
        }
    else{
        counter=0;
    }
    if (counter==3){
        LPC_GPIO2->FIOPIN = ((LPC_GPIO2->FIOPIN & valor) | valor);
        SysTick ->CTRL &=~(1<<1);
    }
    SysTick ->CTRL &= SysTick -> CTRL;
}

