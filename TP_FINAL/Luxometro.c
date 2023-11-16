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
 * LPC_GPDMA->DMACSync &= ~(1 << 4); //linea para subsanar error entre ADC y DMA
 *  */
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
 * LPC_GPDMA->DMACSync &= ~(1 << 4); //linea para subsanar error entre ADC y DMA
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

uint8_t  index=0;
float inputDATA[20]={0};
double acumulador=0;
float parametro =2;

void verificacion(void);
void cfgGPIO(void);
//void cfgDAC(void);
void cfgADC(void);
void cfgTIMER(void);
void cfgEINT(void);

int main(void){
    cfgGPIO();
//    cfgDAC();
    cfgADC();
    cfgTIMER();
    cfgEINT();
    LPC_GPIO2->FIOCLR |= (1<<3);
    LPC_GPIO2->FIOCLR |= (1<<4);
    while(1){}

    return 0;

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

void cfgTIMER(void)
{
    // prescaler
	TIM_TIMERCFG_Type timCfg;
	timCfg.PrescaleOption=TIM_PRESCALE_TICKVAL;
	timCfg.PrescaleValue=1;
	//parametreos match
	TIM_MATCHCFG_Type matchCfg;
	matchCfg.MatchChannel=1; //match0.1 p/ ADC
	matchCfg.IntOnMatch= ENABLE; //que interrumpa
	matchCfg.StopOnMatch=DISABLE;// no se detenga
	matchCfg.ResetOnMatch=ENABLE;
	matchCfg.ExtMatchOutputType=TIM_EXTMATCH_NOTHING;//no haga nada por el pin del match
	matchCfg.MatchValue=6249999;// tiempo para 250ms

	TIM_Init(LPC_TIM0,TIM_TIMER_MODE,(void*)&timCfg);
	TIM_ConfigMatch(LPC_TIM0,&matchCfg);
	TIM_Cmd(LPC_TIM0,DISABLE);
	NVIC_EnableIRQ(TIMER0_IRQn);

    return;
}

void cfgADC(void)
{
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
	ADC_StartCmd (LPC_ADC, ADC_START_CONTINUOUS);
	ADC_BurstCmd(LPC_ADC,1);
	return;
}
/*
void cfgDAC(void)
{
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
*/
void cfgGPIO(void)
{
    PINSEL_CFG_Type led_Red;
    led_Red.Portnum = PINSEL_PORT_2;
    led_Red.Pinnum = PINSEL_PIN_3;
    led_Red.Funcnum = PINSEL_FUNC_0;
    led_Red.Pinmode = PINSEL_PINMODE_PULLDOWN;
    led_Red.OpenDrain = PINSEL_PINMODE_NORMAL;
    PINSEL_ConfigPin(&led_Red);
    //GPIO_SetDir(2, 0X3, 1);
    LPC_GPIO2->FIODIR |=(1<<3);

    PINSEL_CFG_Type led_Green;
    led_Green.Portnum = PINSEL_PORT_2;
    led_Green.Pinnum = PINSEL_PIN_4;
    led_Green.Funcnum = PINSEL_FUNC_0;
    led_Green.Pinmode = PINSEL_PINMODE_TRISTATE;
    led_Green.OpenDrain = PINSEL_PINMODE_NORMAL;
    PINSEL_ConfigPin(&led_Green);
    //GPIO_SetDir(2, 0X4, 1);
    LPC_GPIO2->FIODIR |=(1<<4);
return;
}


void EINT0_IRQHandler(void)
{
	TIM_Cmd(LPC_TIM0,ENABLE);
    LPC_GPIO2->FIOCLR |= (1<<3);
    LPC_GPIO2->FIOCLR |= (1<<4);
    EXTI_ClearEXTIFlag(EXTI_EINT0);

	return;
}

void TIMER0_IRQHandler(void)
{
	static uint16_t valorBinario;
	static float valorVolt;

	valorBinario = ADC_GetData(ADC_CHANNEL_0);
	valorVolt= (valorBinario*3.3)/4096;
	inputDATA[index]= valorVolt;
	acumulador+=valorVolt;
	index++;

    if(index>20)
    {
    	TIM_Cmd(LPC_TIM0,DISABLE);
        index=0;

        verificacion();
    }

    LPC_ADC -> ADGDR &= (1<<31);
    TIM_ClearIntPending(LPC_TIM0,TIM_MR1_INT);

    return;
}


void verificacion (void){
float aux=0;
aux=acumulador;

	if((acumulador/20)<parametro)	//Si no pasa la prueba
	{
		LPC_GPIO2->FIOSET |= (1<<3); //Se enciende led rojo
		LPC_GPIO2->FIOCLR |= (1<<4);	//Se apaga led verde

	}

	else						//Si pasa la prueba
	{
		LPC_GPIO2->FIOSET |= (1<<4); //Se enciende led verde
		LPC_GPIO2->FIOCLR |= (1<<3);	//Se apaga led rojo
	}

	return;
}

