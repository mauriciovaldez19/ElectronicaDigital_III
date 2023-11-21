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
#include "lpc17xx_uart.h"
#include <stdlib.h>


#define NUM_TONOS 		2
#define TONE 			4000
#define MaxData 		50

//Variables ADC
uint32_t inputDATA[MaxData]={0};	//Arreglo con las mediciones del ADC
double    promedio;					//Promedio de las mediciones realizadas

//Variables DAC

uint32_t Tono1[NUM_TONOS] = {TONE, 0};

//Variables UART
uint8_t msg_1[]= "Luxómetro\t-\tTrabajo Final Electrónica Digital 3\t-\t 2023\n\r";
uint8_t msg_2[]= "Ingrese la cantidad de lúmenes a comparar\n\r";
uint8_t msg_3[]= "Opción 1->100 Lúmenes\n\r";
uint8_t msg_4[]= "Opción 2->200 Lúmenes\n\r";
uint8_t msg_5[]= "Opción 3->300 Lúmenes\n\r";
uint8_t msg_6[]= "Opción 4->500 Lúmenes\n\r";
uint8_t msg_7[]= "Opción 5->800 Lúmenes\n\r";
uint8_t msg_8[]= "Opción 6->1000 Lúmenes\n\r";

uint8_t msg_i[]= "La opción elegida es:   \n\r";
uint8_t error[]= "Ingrese un valor válido \n\r";

uint8_t info[1]    = "";					//Variable para guardar valores recibidos
uint8_t mensaje[1] = "";

uint16_t referencia = 0;			//Valor ingrasado por UART en bits
float    parametro  = 0;	        //Valor ingrasado por UART en voltios



GPDMA_LLI_Type LLI1;

//Declaración de funciones
void verificacion	(void);
void cfgGPIO	 	(void);
void cfgADC	 	 	(void);
void cfgTIMER0	 	(void);
void cfgTIMER1      (void);
void cfgEINT     	(void);
void configDAC      (void);
void configDMA   	(void);
void cfgUART        (void);
void UART_IntReceive(void);
void Send_Msg		(void);



//Programa principal
int main (void)
{

    cfgGPIO  ();
    cfgADC   ();
    cfgTIMER0();
	cfgTIMER1();
    cfgEINT  ();
	cfgUART  ();
    configDAC();
    configDMA();

    LPC_GPIO2 -> FIOCLR |= (1<<3);					//Se apagan los leds
    LPC_GPIO2 -> FIOCLR |= (1<<4);


	GPDMA_ChannelCmd(0, ENABLE);

	UART_Send(LPC_UART2,msg_1,sizeof(msg_1),BLOCKING);
	Send_Msg ();

    while(1)
	{

	}

    return 0;

}


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

//Configuración ADC
void cfgADC()
{
	PINSEL_CFG_Type adcPin_cfg;							//P0.23 como ADC0
	adcPin_cfg.Portnum   = PINSEL_PORT_0;
	adcPin_cfg.Pinnum    = PINSEL_PIN_23;
	adcPin_cfg.Funcnum   = PINSEL_FUNC_1;
	adcPin_cfg.Pinmode 	 = PINSEL_PINMODE_TRISTATE;
	adcPin_cfg.OpenDrain = PINSEL_PINMODE_NORMAL;
	PINSEL_ConfigPin(&adcPin_cfg);

	ADC_Init      (LPC_ADC, 200000);					//Frecuencia de conversión
	ADC_ChannelCmd(LPC_ADC, ADC_CHANNEL_0, ENABLE);
	ADC_StartCmd  (LPC_ADC, ADC_START_CONTINUOUS);		//Conversión continua
	ADC_BurstCmd  (LPC_ADC, 1);
	ADC_IntConfig (LPC_ADC, ADC_ADINTEN0, SET);
	LPC_ADC -> ADGDR &= ~(1<<31);

	return;

}


//Configuración timer TIM0
void cfgTIMER0(void)
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
	matchCfg.MatchValue			= 249999999;			//Tiempo para 5 seg

	TIM_Init	   (LPC_TIM0,TIM_TIMER_MODE,(void*)&timCfg);
	TIM_ConfigMatch(LPC_TIM0,&matchCfg);
	TIM_Cmd	       (LPC_TIM0,DISABLE);
	NVIC_EnableIRQ (TIMER0_IRQn);
	NVIC_SetPriority(TIMER1_IRQn,4);
    return;
}

//Configuración timer TIM1
void cfgTIMER1(void)
{
    //Configuración prescaler
	TIM_TIMERCFG_Type tim1Cfg;
	tim1Cfg.PrescaleOption = TIM_PRESCALE_TICKVAL;
	tim1Cfg.PrescaleValue  = 1;							//Valor de prescaler

	//Configuración match
	TIM_MATCHCFG_Type      matchCfg;
	matchCfg.MatchChannel		= 1;					//MAT1.1
	matchCfg.IntOnMatch			= ENABLE; 				//interrumpe al hacer match
	matchCfg.StopOnMatch		= DISABLE;				//TIM1 no se detiene al hacer match
	matchCfg.ResetOnMatch	    = ENABLE;				//TIM1 se resetea al hacer match
	matchCfg.ExtMatchOutputType = TIM_EXTMATCH_NOTHING;
	matchCfg.MatchValue			= 49999999;			    //Tiempo para 1 seg

	TIM_Init	   (LPC_TIM1,TIM_TIMER_MODE,&tim1Cfg);
	TIM_ConfigMatch(LPC_TIM1,&matchCfg);
	TIM_Cmd	       (LPC_TIM1,DISABLE);

	NVIC_EnableIRQ (TIMER1_IRQn);
	NVIC_SetPriority(TIMER1_IRQn,3);
    return;
}

//Configuración interrupción externa EINT0
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

    return;
}

//Configuración DAC
void configDAC()
{
	PINSEL_CFG_Type pin_dac;
	pin_dac.Portnum   = PINSEL_PORT_0;					//P0.26 como DAC
	pin_dac.Pinnum    = PINSEL_PIN_26;
	pin_dac.Funcnum   = PINSEL_FUNC_2;
	pin_dac.Pinmode   = PINSEL_PINMODE_TRISTATE;
	pin_dac.OpenDrain = PINSEL_PINMODE_NORMAL;
	PINSEL_ConfigPin(&pin_dac);

	DAC_CONVERTER_CFG_Type dacCfg;						//DAC por timmer
	dacCfg.CNT_ENA = SET;
	DAC_Init(LPC_DAC);
	DAC_ConfigDAConverterControl(LPC_DAC, &dacCfg);

}

//Configuración DMA
void configDMA()
{

	LLI1.SrcAddr  = (uint32_t) &(LPC_ADC -> ADDR0);
	LLI1.DstAddr  = (uint32_t) &inputDATA;
	LLI1.NextLLI  = (uint32_t) &LLI1;
	LLI1.Control  = MaxData
				    | (1<<17)					//Longitud de destino: 32bits
				  	| (1<<19)					//Longitud de fuente : 32 bits
				  	| (1<<27);					//Incremento de destino

	GPDMA_Init();

	GPDMA_Channel_CFG_Type DMACFG;
	DMACFG.ChannelNum    = 0;
	DMACFG.SrcMemAddr    = 0;						//Dirección de fuente
	DMACFG.DstMemAddr    = (uint32_t) inputDATA;	//Dirección de destino
	DMACFG.TransferSize  = MaxData;					//Cantidad de datos a transferir
	DMACFG.TransferType  = GPDMA_TRANSFERTYPE_P2M;	//Formato de transferencia
	DMACFG.TransferWidth = 0;
	DMACFG.SrcConn		 = GPDMA_CONN_ADC;
	DMACFG.DstConn		 = 0;
	DMACFG.DMALLI		 = (uint32_t) &LLI1;

	LPC_GPDMA -> DMACSync &=~(1<<4);
	GPDMA_Setup(&DMACFG);
}

//Configuración de UART
void cfgUART(void)
{

    PINSEL_CFG_Type pin_cfg;
    pin_cfg.Portnum   = PINSEL_PORT_0;
    pin_cfg.Pinnum    = PINSEL_PIN_10;
    pin_cfg.Funcnum   = PINSEL_FUNC_1;
    pin_cfg.Pinmode   = PINSEL_PINMODE_TRISTATE;
    pin_cfg.OpenDrain = 0;
    PINSEL_ConfigPin(&pin_cfg);

    pin_cfg.Pinnum    = PINSEL_PIN_11;
    PINSEL_ConfigPin(&pin_cfg);


	UART_CFG_Type UARTConfigStruct;
	UART_FIFO_CFG_Type UARTFIFOConfigStruct;

	UART_ConfigStructInit(&UARTConfigStruct);			//Configuración por defecto
	UART_Init(LPC_UART2, &UARTConfigStruct);			//Inicializa periférico
	UART_FIFOConfigStructInit(&UARTFIFOConfigStruct);
	UART_FIFOConfig(LPC_UART2, &UARTFIFOConfigStruct);	//Inicializa FIFO
	UART_TxCmd(LPC_UART2, ENABLE);						//Habilita transmisión
	UART_IntConfig(LPC_UART2,UART_INTCFG_RBR,ENABLE);	//Habilita interrupcion Tx
	UART_IntConfig(LPC_UART2,UART_INTCFG_RLS,ENABLE);
	//UART_IntConfig(LPC_UART2,UART_INTCFG_THRE,ENABLE);
	NVIC_EnableIRQ(UART2_IRQn);
	return;
}


//############################################################################
//Función de mensaje inicial
void Send_Msg(void)
{
    UART_Send(LPC_UART2,msg_2,sizeof(msg_2),BLOCKING);
    UART_Send(LPC_UART2,msg_3,sizeof(msg_3),BLOCKING);
    UART_Send(LPC_UART2,msg_4,sizeof(msg_4),BLOCKING);
    UART_Send(LPC_UART2,msg_5,sizeof(msg_5),BLOCKING);
    UART_Send(LPC_UART2,msg_6,sizeof(msg_6),BLOCKING);
    UART_Send(LPC_UART2,msg_7,sizeof(msg_7),BLOCKING);
    UART_Send(LPC_UART2,msg_8,sizeof(msg_8),BLOCKING);

	return;
}

//UART interrumpe para ingresar un mensaje
void UART2_IRQHandler(void)
{
	uint8_t index  = 0;
	uint8_t status = 0;
	UART_Receive(LPC_UART2,info,sizeof(info),NONE_BLOCKING);

    for (int i = 0; i < sizeof(info); i++)
	{
        mensaje[index] = info[i];
        index++;

        if (index == sizeof(mensaje))
		{
        	status = 1;
        	index  = 0;
        }
    }

	if(status)
	{
		UART_Send(LPC_UART2,msg_i,  sizeof(msg_i),BLOCKING);
		UART_Send(LPC_UART2,mensaje,sizeof(mensaje),BLOCKING);
	}

	uint32_t aux = atoi((char*)mensaje);

	switch(aux){
	        case 1:
	        referencia = 100;
	        break;
	        case 2:
	        referencia = 200;
	        break;
	        case 3:
	        referencia = 300;
	        break;
	        case 4:
	        referencia = 500;
	        break;
	        case 5:
	        referencia = 800;
	        break;
	        case 6:
	        referencia = 1000;
	        break;
	        default:
	        UART_Send(LPC_UART2,error,sizeof(error),BLOCKING);
	}

	//Se convierte la referencia a volts
	parametro = -0.0004*referencia*referencia + 1.7346*referencia + 285.35;
	parametro = parametro/1000;

	//Se habilita la interrupción externa
	NVIC_EnableIRQ	  (EINT0_IRQn);

	return;
}

//Al presionar el interruptor
void EINT0_IRQHandler(void)
{
	TIM_Cmd(LPC_TIM0, ENABLE);						//Se habilita TIM0
	TIM_Cmd(LPC_TIM1, ENABLE);						//Se habilita TIM1

	//No es posible interrumpir durante la medición
	NVIC_DisableIRQ	  (EINT0_IRQn);					//Se deshabilita EINT0
    LPC_GPIO2 -> FIOCLR |= (1<<3);					//Se apagan los leds
    LPC_GPIO2 -> FIOCLR |= (1<<4);

    EXTI_ClearEXTIFlag(EXTI_EINT0);					//Se limpia flag

	return;
}

//Luego de 5 segundos, al hacer match
void TIMER0_IRQHandler(void)
{
	//Variabeles
	uint16_t  valorBinario;
	double    valorVolt  = 0;
	double    acumulador = 0;

	uint8_t i;

	//Se desplazan los valores obtenidos
	for (i = 0; i < 50; i++)
	{
		valorBinario = (inputDATA[i] & 0xFFF0)>>4;
		valorVolt    = (valorBinario*3.3)/4096;		//Conversión de binario a voltaje
		acumulador  += valorVolt;   				//Acumulador de mediciones

	}

	promedio   = acumulador/50;						//Se calcula el promedio
	acumulador = 0;
	TIM_Cmd(LPC_TIM0,DISABLE);						//Se deshabilita TIM0
	TIM_Cmd(LPC_TIM1,DISABLE);						//Se deshabilita TIM1
    verificacion();									//Verificación de los datos
    LLI1.Control |= 50<<0; 							// Resetea contador del DMA
    TIM_ClearIntPending(LPC_TIM0, TIM_MR1_INT);		//Se limpia flag TIM0

    return;
}

//Función de verificación: compara el valor de luxes ingresado por UART con el medido por el LDR
void verificacion (void)
{

	if(promedio < parametro)					    //Si no pasa la prueba
	{
		LPC_GPIO2 -> FIOSET |= (1<<3); 				//Se enciende led rojo
		LPC_GPIO2 -> FIOCLR |= (1<<4);				//Se apaga led verde

	}

	else											//Si pasa la prueba
	{
		LPC_GPIO2 -> FIOSET |= (1<<4); 				//Se enciende led verde
		LPC_GPIO2 -> FIOCLR |= (1<<3);				//Se apaga led rojo
	}

	promedio = 0;									//Se resetea promedio

	Send_Msg ();									//Mensaje inicial
	return;
}

//Luego de 0.5 segundos, al hacer match
void TIMER1_IRQHandler(void)
{
	static uint8_t count = 0;


	if (count%2 == 0)
	{
		DAC_UpdateValue(LPC_DAC, Tono1[0]);
	}

	else
	{
		DAC_UpdateValue(LPC_DAC, Tono1[1]);
	}

	count ++;
    TIM_ClearIntPending(LPC_TIM1, TIM_MR1_INT);		//Se limpia flag TIM1

    return;
}
