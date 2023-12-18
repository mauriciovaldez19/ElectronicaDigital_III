/*
 Consigna (EJERCICIO Nº2):
 Utilizando el timer0, un dac, interrupciones y el driver del LPC1769, escribir
 un código que permita generar una señal triangular periódica simétrica, que
 tenga el mínimo periodo posible, la máxima excursión de voltaje pico a pico
 posible y el mínimo incremento de señal posible por el dac. Suponer una
 frecuencia de core cclk de 100 Mhz. El código debe estar debidamente comentado..
*/
//No voy a usar drivers

#include "LPC17xx.h"


void configTimer0 (void);
void configDAC    (void);

uint16_t onda[1024];

int main(void) 
{
    
    configTimer0 ();
    configDAC    ();

    
    while(1) 
    {
       
    }
    return 0 ;
}


void configTimer0 (void)
{
    LPC_SC -> PCONP    |= (1<<1);       //Se enciende Timer0
    LPC_SC -> PCLKSEL0 |= (1<<2);       //pclk = cclk
    
    LPC_TIM0 -> PR   =  0;              //Por pedir mínimo periodo
    LPC_TIM0 -> MR0  =  0;              //Ver
    LPC_TIM0 -> MCR |=  3;              //Timer0 interrumpe y se resetea con MAT0.0
    LPC_TIM0 -> IR  |=  0x3F;           //Se limpian todas las flags
    LPC_TIM0 -> TCR  =  3;              //Habilita y resetea el Timer0
    LPC_TIM0 -> TCR &= ~2;

    NVIC_EnableIRQ(TIMER0_IRQn);

}


void configDAC    (void)
{
    LPC_PINCON -> PINSEL1  &= ~(1<<20);  //P0.26 como AOUT
    LPC_PINCON -> PINSEL1  |=  (1<<21);
    LPC_PINCON -> PINMODE1 &= ~(1<<20);  //P0.26 sin resistencia de pull-up ni pull-down
    LPC_PINCON -> PINMODE1 |=  (1<<21);
    
    LPC_DAC -> DACR &= ~(1<<16);
}

void TIMER0_IRQHandler (void)
{
    static uint16_t   i = 0;
    static uint16_t aux = 0;

    //Primera media onda
    if (aux == 0)
    {
        LPC_DAC -> DACR &= ~(0x3FF<<6);     //Se limpian los datos anteriores
        LPC_DAC -> DACR |=  (i<<6); 
        i++;

        if (i == 1023)
        {
            i   = 0;
            aux = 1;
        }
    }
    
    //Segunda media onda
    if (aux == 1)
    {
        i--;                                //Empieza desde 1022 porque el último dato fue 1023
        LPC_DAC -> DACR &= ~(0x3FF<<6);     //Se limpian los datos anteriores
        LPC_DAC -> DACR |=  (i<<6); 


        if (i == 1)                         //El 0 se hace en el if anterior
        {
            i   = 0;
            aux = 0;
        }
    }
    
    LPC_TIM0 -> IR  |=  (1);                //Se limpia el flag

}
