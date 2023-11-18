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


#include "LPC17xx.h"
#include "lpc17xx_adc.h"
#include "lpc17xx_dac.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_timer.h"
#include "lpc17xx_gpdma.h"
#include "lpc17xx_exti.h"

//Variables globales

float inputDATA[20] = {0};	//Arreglo con las mediciones del ADC
float parametro     = 2;	//Valor ingrasado por UART
float promedio;				//Promedio de las mediciones realizadas

//Declaración de funciones
void verificacion(void);
void cfgGPIO	 (void);
//void cfgDAC      (void);
void cfgADC	 	 (void);
void cfgTIMER	 (void);
void cfgEINT     (void);


//Programa principal
int main (void)
{
    cfgGPIO ();
	//cfgDAC  ();
    cfgADC  ();
    cfgTIMER();
    cfgEINT ();
    LPC_GPIO2->FIOCLR |= (1<<3);
    LPC_GPIO2->FIOCLR |= (1<<4);

    while(1)
	{

	}

    return 0;

}

//Configuración iinterrupción externa EINT0
void cfgEINT(void)
{
	PINSEL_CFG_Type ext_int_cfg;
    ext_int_cfg.Portnum   = PINSEL_PORT_2;			//P2.10 como EINT0
    ext_int_cfg.Pinnum 	  = PINSEL_PIN_10;
    ext_int_cfg.Funcnum	  = PINSEL_FUNC_1;
    ext_int_cfg.Pinmode	  = PINSEL_PINMODE_PULLUP;
    ext_int_cfg.OpenDrain = PINSEL_PINMODE_NORMAL;

	PINSEL_ConfigPin(&ext_int_cfg);

	EXTI_InitTypeDef ext_int;
    ext_int.EXTI_Line 	  = EXTI_EINT0;				//Interrumpe por flanco de subida
    ext_int.EXTI_Mode	  = EXTI_MODE_EDGE_SENSITIVE;
    ext_int.EXTI_polarity = EXTI_POLARITY_LOW_ACTIVE_OR_FALLING_EDGE;

    EXTI_Config	      (&ext_int);
    EXTI_ClearEXTIFlag(EXTI_EINT0);
    NVIC_EnableIRQ	  (EINT0_IRQn);

    return;
}

//Configuración timer TIM0
void cfgTIMER(void)
{
    //Configuración prescaler
	TIM_TIMERCFG_Type timCfg;
	timCfg.PrescaleOption = TIM_PRESCALE_TICKVAL;
	timCfg.PrescaleValue  = 1;							//Valor de prescaler

	//Configuración match
	TIM_MATCHCFG_Type matchCfg;
	matchCfg.MatchChannel		= 1;					//MAT0.1
	matchCfg.IntOnMatch			= ENABLE; 				//interrumpe al hacer match
	matchCfg.StopOnMatch		= DISABLE;				//TIM0 no se detiene al hacer match
	matchCfg.ResetOnMatch	    = ENABLE;
	matchCfg.ExtMatchOutputType = TIM_EXTMATCH_NOTHING;
	matchCfg.MatchValue			= 6249999;				//Tiempo para 250 ms

	TIM_Init	   (LPC_TIM0,TIM_TIMER_MODE,(void*)&timCfg);
	TIM_ConfigMatch(LPC_TIM0,&matchCfg);
	TIM_Cmd	       (LPC_TIM0,DISABLE);
	NVIC_EnableIRQ (TIMER0_IRQn);

    return;
}

//Configuración ADC
void cfgADC(void)
{
	PINSEL_CFG_Type adcPin_cfg;							//P0.23 como ADC0
	adcPin_cfg.Portnum   = PINSEL_PORT_0;
	adcPin_cfg.Pinnum    = PINSEL_PIN_23;
	adcPin_cfg.Funcnum   = PINSEL_FUNC_1;
	adcPin_cfg.Pinmode 	 = PINSEL_PINMODE_TRISTATE;
	adcPin_cfg.OpenDrain = PINSEL_PINMODE_NORMAL;
	PINSEL_ConfigPin(&adcPin_cfg);

	ADC_Init      (LPC_ADC, 200000);					//Frecuencia de conversión
	ADC_ChannelCmd(LPC_ADC,ADC_CHANNEL_0,ENABLE);
	ADC_StartCmd  (LPC_ADC, ADC_START_CONTINUOUS);		//Conversión continua
	ADC_BurstCmd  (LPC_ADC,1);
	LPC_ADC -> ADGDR &= ~(1<<31);

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
//Configuración GPIO
void cfgGPIO(void)
{
	//Led rojo
    PINSEL_CFG_Type led_Red;
    led_Red.Portnum   = PINSEL_PORT_2;				//P2.3 como GPIO
    led_Red.Pinnum 	  = PINSEL_PIN_3;
    led_Red.Funcnum   = PINSEL_FUNC_0;
    led_Red.Pinmode   = PINSEL_PINMODE_PULLDOWN;
    led_Red.OpenDrain = PINSEL_PINMODE_NORMAL;

    PINSEL_ConfigPin(&led_Red);
    //GPIO_SetDir(2, 0X3, 1);
    LPC_GPIO2->FIODIR |=(1<<3);						//P2.3 como salida

	//Led verde
    PINSEL_CFG_Type led_Green;
    led_Green.Portnum   = PINSEL_PORT_2;			//P2.4 como GPIO
    led_Green.Pinnum    = PINSEL_PIN_4;
    led_Green.Funcnum   = PINSEL_FUNC_0;
    led_Green.Pinmode   = PINSEL_PINMODE_TRISTATE;
    led_Green.OpenDrain = PINSEL_PINMODE_NORMAL;

    PINSEL_ConfigPin(&led_Green);
    //GPIO_SetDir(2, 0X4, 1);
    LPC_GPIO2->FIODIR |=(1<<4);						//P2.4 como salida

	return;
}

//Al presionar el interruptor
void EINT0_IRQHandler(void)
{
	TIM_Cmd(LPC_TIM0, ENABLE);						//Se habilita TIM0

    LPC_GPIO2->FIOCLR |= (1<<3);					//Se apagan los leds
    LPC_GPIO2->FIOCLR |= (1<<4);

    EXTI_ClearEXTIFlag(EXTI_EINT0);					//Se limpia flag

	return;
}

//Cada vez que haga match
void TIMER0_IRQHandler(void)
{
	//Variabeles
	static double   acumulador = 0;
	static uint8_t  index      = 0;
		   uint16_t valorBinario;
	       float    valorVolt;

	valorBinario     = ADC_GetData(ADC_CHANNEL_0);	//Se almacena valor de ADC0
	valorVolt	     = (valorBinario*3.3)/4096;		//Conversión de binario a voltaje
	inputDATA[index] = valorVolt;					//Valores a transmitir por UART
	acumulador      += valorVolt;					//Acumulador de mediciones
	index++;										//Contador de interrupción

    if(index > 20)									//Luego de tomar 20 muestras
    {
    	TIM_Cmd(LPC_TIM0,DISABLE);					//Se deshabilita TIM0
        index = 0;									//Se resetea contador de interrupción

        promedio  = acumulador/20;					//Se calcula el promedio

        verificacion();								//Verificación de los datos
    }

    LPC_ADC -> ADGDR &= (1<<31);					//Se limpia flag ADC0
    TIM_ClearIntPending(LPC_TIM0, TIM_MR1_INT);		//Se limpia flag TIM0

    return;
}

//Función de verificación: compara el valor de luxes ingresado por UART con el medido por el LDR
void verificacion (void)
{

	if(promedio < parametro)					    //Si no pasa la prueba
	{
		LPC_GPIO2->FIOSET |= (1<<3); 				//Se enciende led rojo
		LPC_GPIO2->FIOCLR |= (1<<4);				//Se apaga led verde

	}

	else											//Si pasa la prueba
	{
		LPC_GPIO2->FIOSET |= (1<<4); 				//Se enciende led verde
		LPC_GPIO2->FIOCLR |= (1<<3);				//Se apaga led rojo
	}

	return;
}
