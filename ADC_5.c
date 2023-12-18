/*
 Consigna (EJERCICIO N°1):
 Programar el microcontrolador LPC1769 para que mediante su ADC digitalice dos
 señales analógicas cuyos anchos de bandas son de 10 Khz cada una. Los canales
 utilizados deben ser el 2 y el 4 y los datos deben ser guardados en dos regiones
 de memorias distintas que permitan contar con los últimos 20 datos de cada canal.
 Suponer una frecuencia de core cclk de 100 Mhz. El código debe estar debidamente
 comentado.
*/

/*
 cclk: 100 MHz -> % 1            -> pclk = 100 MHZ
 Dos señales, de 10 KHz cada una -> 20 KHz
 Por Nyquisr                     -> 40 KHz
 64 clocks = 1 muestra           -> 2.56 MHz

 Entonces para llevar pclk a 2.56 MHz se deve dividir la frecuencia por 39
 Si se usa otra división de pclk, no da número redondo
*/

#include "LPC17xx.h"

void configADC (void);
void configDMA (void);
uint16_t resultado_2 [20];
uint16_t resultado_4 [20];

int main(void) 
{
    configADC ();
    configDMA ();

    while(1) 
    {
    }
    return 0 ;
}

void configADC (void)
{
    LPC_SC -> PCONP |= (1<<12);        //Se enciende el ADC

    LPC_SC -> PCLKSEL0 |= (1<<24);      //pclk = cclk

    LPC_PINCON -> PINSEL1 |= (1<<18);  //P0.24 como AD0.2
    LPC_PINCON -> PINSEL3 |= (1<<28);  //P0.25 como AD0.4

    LPC_PINCON -> PINMODE1 |= (1<<19); //Sin resistencia de pull-up ni pull-down
    LPC_PINCON -> PINMODE3 |= (1<<29); 

    LPC_ADC -> ADCR |= (0x0A);         //Se seleccionan los canales AD0.2 y AD0.4
    LPC_ADC -> ADCR |= (0x26<<8);      //clkdiv = se divide por (38+1)
    LPC_ADC -> ADCR |= (1<<16);        //Modo burst
    LPC_ADC -> ADCR |= (1<<21);        //Se hablilita el ADC

    LPC_ADC -> ADINTEN |= (1<<2);      //AD0.2 interrumpe al terminar la conversión
    LPC_ADC -> ADINTEN |= (1<<4);      //AD0.4 interrumpe al terminar la conversión

    NVIV_EnableIRQ(ADC_IRQn);

}

void ADC_IRQHandler (void)
{   
    static uint8_t contador_1 = 0;
    static uint8_t contador_2 = 0;

    //Termina de convertir AD0.2
    if((LPC_ADC -> ADDR2) & (0x02))
    {
        //Si se llega al límite de datos, se comienza a cargar nuevamente
        if (contador_1 > 20)
        {
            contador_1 = 0;
        }

        resultado_2 [contador_1] = ((LPC_ADC -> ADDR2)>>4) & 0xFFF; //Se guarda valor
        contador_1 ++;                                              //Se suma 1 a contador_1
        LPC_ADC -> ADDR2 &= LPC_ADC -> ADDR2;                       //Se limpia flag
        
    }
    
    //Termina de convertir AD0.2
    if((LPC_ADC -> ADDR4) & (0x04))
    {
        //Si se llega al límite de datos, se comienza a cargar nuevamente
        if (contador_2 > 20)
        {
            contador_2 = 0;
        }

        resultado_4 [contador_2] = ((LPC_ADC -> ADDR4)>>4) & 0xFFF; //Se guarda valor
        contador_2 ++;                                              //Se suma 1 a contador_2
        LPC_ADC -> ADDR4 &= LPC_ADC -> ADDR4;                       //Se limpia flag
    }
}
