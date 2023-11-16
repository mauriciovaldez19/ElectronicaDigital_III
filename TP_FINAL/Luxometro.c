/* Pasos propuestos para el funcionamiento del Luxometro:
 * La señal de tensión a bornes del LDR ingresa por un canal del ADC
 * el ADC convierte los datos y los guarda en un arreglo de 20 valores; la conversion sera en modo burst pero habilitado/deshabilitado por Timer.
 * Se calcula 1 muestra cada 250[ms] es decir las 20 muestras en 5 [s].
 * Los datos son tratados por una funcion internamente para obtener un promedio de la medición.
 * Antes de comenzar y una vez finalizada la toma de datos, se envía por el DAC un arreglo de valores que represente 
 * una señal sonora  reproducida por un buzzer.
 * Luego de obtenidos y tratados los datos, se envía el resultado mediante UART a la pc. Además se hará la valoración de que si el valor medido es menor
 * al valor reglamentario se prende un led(rojo) por GPIO, si el resultado es igual o mayor al permitido se prende otro led (verde).
 * La configuración del valor de referencia se envía desde la pc por UART
 * 
 * La DMA se utiliza para: 
 * 							movilizar datos de ADC->Memoria
 *  */
#include "LPC17xx.h"
#include "lpc17xx_adc.h"
#include "lpc17xx_dac.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_timer.h"
#include "lpc17xx_gpdma.h"
#include "lpc17xx_exti.h"

void cfgGPIO(void);
void cfgDAC(void);
void cfgADC(void);

int main(void){
    cfgGPIO();
    cfgDAC();

    while(1){}

    return 0;
}
void cfgADC(void){
PINSEL_CFG_Type adcPin_cfg;
	adcPin_cfg.Portnum   = 0;
	adcPin_cfg.Pinnum    = 23;
	adcPin_cfg.Funcnum   = 1;
	adcPin_cfg.Pinmode 	 = PINSEL_PINMODE_TRISTATE;
	adcPin_cfg.OpenDrain = 0;
PINSEL_ConfigPin(&adcPin_cfg);
    
return
}
void cfgDAC(void){
PINSEL_CFG_Type dacPin_cfg;
    dacPin_cfg.Portnum = 0;
    dacPin_cfg.Pinnum = 26;
    dacPin_cfg.Funcnum = 2;
    dacPin_cfg.Pinmode = 0;
    dacPin_cfg.OpenDrain = 0;
PINSEL_ConfigPin(&dacPin_cfg);
DAC_Init(LPC_DAC);
return;
}

void cfgGPIO(void){
PINSEL_CFG_Type led_Red;
    PinCfg.Portnum = PINSEL_PORT_2;
    PinCfg.Pinnum = PINSEL_PIN_3;
    PinCfg.Funcnum = PINSEL_FUNC_0;
    PinCfg.Pinmode = PINSEL_PINMODE_PULLUP;
    PinCfg.OpenDrain = PINSEL_PINMODE_NORMAL;
PINSEL_ConfigPin(&led_Red);
GPIO_SetDir(PINSEL_PORT_2, PINSEL_PIN_3,1); 

PINSEL_CFG_Type led_Green;
    PinCfg.Portnum = PINSEL_PORT_2;
    PinCfg.Pinnum = PINSEL_PIN_4;
    PinCfg.Funcnum = PINSEL_FUNC_0;
    PinCfg.Pinmode = PINSEL_PINMODE_PULLUP;
    PinCfg.OpenDrain = PINSEL_PINMODE_NORMAL;
PINSEL_ConfigPin(&led_Green);
GPIO_SetDir(PINSEL_PORT_2, PINSEL_PIN_4,1);

return;
}