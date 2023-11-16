/* Pasos propuestos para el funcionamiento del Luxometro:
 * La señal de tensión a bornes del LDR ingresa por un canal del ADC (canal 0)
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
#define RED_LED 1<<3
#define GREEN_LED 1<<4
#define PIN22 1<<22

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
void cfgTIMER(void);
void cfgEINT(void);

uint16_t inputDATA[20]={0};

int main(void){
    cfgGPIO();
    cfgDAC();
    cfgADC();
    cfgTIMER();
    cfgEINT();

PINSEL_CFG_Type led_Red;
    led_Red.Portnum = PINSEL_PORT_0;
    led_Red.Pinnum = PINSEL_PIN_22;
    led_Red.Funcnum = PINSEL_FUNC_0;
    led_Red.Pinmode = PINSEL_PINMODE_PULLUP;
    led_Red.OpenDrain = PINSEL_PINMODE_NORMAL;
PINSEL_ConfigPin(&led_Red);
GPIO_SetDir(PINSEL_PORT_0, PIN22,1);


    while(1){}

    return 0;
    
}
void ADC_IRQHandler (){
	static uint16_t valorBinario=0;
	static float voltADC =0;
	valorBinario=ADC_ChannelGetData(LPC_ADC,ADC_CHANNEL_0);
	voltADC= (valorBinario/4096)*3.3;
	return;
}
void EINT0_IRQHandler(void){
	static uint8_t state=0;
    state++;
    if(state%2){
       LPC_GPIO0->FIOSET |= PIN22;
    }
    else{
        LPC_GPIO0->FIOCLR |= PIN22;
    }
    EXTI_ClearEXTIFlag(EXTI_EINT0);
	return;
}
void cfgEINT(void){
PINSEL_CFG_Type ext_int_cfg;
    ext_int_cfg.Portnum = PINSEL_PORT_2;
    ext_int_cfg.Pinnum = PINSEL_PIN_10;
    ext_int_cfg.Funcnum = PINSEL_FUNC_1;
    ext_int_cfg.Pinmode = PINSEL_PINMODE_PULLUP;
    ext_int_cfg.OpenDrain = PINSEL_PINMODE_NORMAL;
PINSEL_ConfigPin(&ext_int_cfg);  
EXTI_InitTypeDef ext_int;
    ext_int.EXTI_Line = EXTI_EINT0;
    ext_int.EXTI_Mode = EXTI_MODE_EDGE_SENSITIVE;
    ext_int.EXTI_polarity = EXTI_POLARITY_LOW_ACTIVE_OR_FALLING_EDGE;
    EXTI_Config(&ext_int);
    EXTI_ClearEXTIFlag(EXTI_EINT0);
    NVIC_EnableIRQ(EINT0_IRQn);
    return;
}
void cfgTIMER(void){
    return;
}
void cfgADC(void){
PINSEL_CFG_Type adcPin_cfg;
	adcPin_cfg.Portnum   = PINSEL_PORT_0;
	adcPin_cfg.Pinnum    = PINSEL_PIN_23;
	adcPin_cfg.Funcnum   = PINSEL_FUNC_1;
	adcPin_cfg.Pinmode 	 = PINSEL_PINMODE_TRISTATE;
	adcPin_cfg.OpenDrain = PINSEL_PINMODE_NORMAL;
PINSEL_ConfigPin(&adcPin_cfg);
    
ADC_Init(LPC_ADC, 200000); 
LPC_ADC->ADGDR &= ~(1<<31);
ADC_ChannelCmd(LPC_ADC,ADC_CHANNEL_0,ENABLE);
ADC_StartCmd (LPC_ADC, ADC_START_ON_MAT01);

return;
}
void cfgDAC(void){
PINSEL_CFG_Type dacPin_cfg;
    dacPin_cfg.Portnum = PINSEL_PORT_0;
    dacPin_cfg.Pinnum = PINSEL_PIN_26;
    dacPin_cfg.Funcnum = PINSEL_FUNC_2;
    dacPin_cfg.Pinmode = PINSEL_PINMODE_PULLUP;
    dacPin_cfg.OpenDrain = PINSEL_PINMODE_NORMAL;
PINSEL_ConfigPin(&dacPin_cfg);
DAC_Init(LPC_DAC);
return;
}
void cfgGPIO(void){
PINSEL_CFG_Type led_Red;
    led_Red.Portnum = PINSEL_PORT_2;
    led_Red.Pinnum = PINSEL_PIN_3;
    led_Red.Funcnum = PINSEL_FUNC_0;
    led_Red.Pinmode = PINSEL_PINMODE_PULLUP;
    led_Red.OpenDrain = PINSEL_PINMODE_NORMAL;
PINSEL_ConfigPin(&led_Red);
GPIO_SetDir(PINSEL_PORT_2, RED_LED,1); 

PINSEL_CFG_Type led_Green;
    led_Green.Portnum = PINSEL_PORT_2;
    led_Green.Pinnum = PINSEL_PIN_4;
    led_Green.Funcnum = PINSEL_FUNC_0;
    led_Green.Pinmode = PINSEL_PINMODE_PULLUP;
    led_Green.OpenDrain = PINSEL_PINMODE_NORMAL;
PINSEL_ConfigPin(&led_Green);
GPIO_SetDir(PINSEL_PORT_2, GREEN_LED,1);

return;
}