#include "LPC17xx.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_uart.h"
#include <stdlib.h>

uint8_t msg_1[]= "Luxómetro\t-\tTrabajo Final Electrónica Digital 3\t-\t 2023\n\r";
uint8_t msg_2[]= "Ingrese la cantidad de lúmenes a comparar\n\r";
uint8_t msg_3[]= "Opción 1->100 Lúmenes\t-\tOpción 2->200 Lúmenes\t-\tOpción 3->300 Lúmenes\n\r";
uint8_t msg_4[]= "Opción 4->400 Lúmenes\t-\tOpción 5->500 Lúmenes\t-\tOpción 6->600 Lúmenes\n\r";
uint8_t msg_5[]= "La opción elegida es: \n\r";
uint8_t error[]= "\n Ingrese un valor válido \n\r";

uint8_t info[1]="";// variable para guardar valores recibidos
uint8_t mensaje[1] = "";
uint8_t index=0;
uint8_t status=0;
uint16_t referencia=0;

void cfgUART (void);
void UART_IntReceive(void);
int main(void)
{
    cfgUART();
    UART_Send(LPC_UART2,msg_1,sizeof(msg_1),BLOCKING);
    UART_Send(LPC_UART2,msg_2,sizeof(msg_2),BLOCKING);
    UART_Send(LPC_UART2,msg_3,sizeof(msg_3),BLOCKING);
    UART_Send(LPC_UART2,msg_4,sizeof(msg_4),BLOCKING);
    while(1)
    {

    }
    return 0 ;
}

void cfgUART(void){

    PINSEL_CFG_Type pin_cfg;
    pin_cfg.Portnum = PINSEL_PORT_0;
    pin_cfg.Pinnum  = PINSEL_PIN_10;
    pin_cfg.Funcnum = PINSEL_FUNC_1;
    pin_cfg.Pinmode = PINSEL_PINMODE_TRISTATE;
    pin_cfg.OpenDrain = 0;
    PINSEL_ConfigPin(&pin_cfg);
    pin_cfg.Pinnum  = PINSEL_PIN_11;
    PINSEL_ConfigPin(&pin_cfg);


	UART_CFG_Type UARTConfigStruct;
	UART_FIFO_CFG_Type UARTFIFOConfigStruct;
	//configuración por defecto:
	UART_ConfigStructInit(&UARTConfigStruct);
	//inicializa periférico
	UART_Init(LPC_UART2, &UARTConfigStruct);
	UART_FIFOConfigStructInit(&UARTFIFOConfigStruct);
	//Inicializa FIFO
	UART_FIFOConfig(LPC_UART2, &UARTFIFOConfigStruct);
	//Habilita transmisión
	UART_TxCmd(LPC_UART2, ENABLE);
	//Habilita interrupcion Tx
	UART_IntConfig(LPC_UART2,UART_INTCFG_RBR,ENABLE);
	UART_IntConfig(LPC_UART2,UART_INTCFG_RLS,ENABLE);
	//UART_IntConfig(LPC_UART2,UART_INTCFG_THRE,ENABLE);
	NVIC_EnableIRQ(UART2_IRQn);
	return;
}

void UART2_IRQHandler(void){
	status=0;
	UART_Receive(LPC_UART2,info,sizeof(info),NONE_BLOCKING);
    for (int i = 0; i < sizeof(info); i++) {
        mensaje[index] = info[i];
        index++;
        if (index == sizeof(mensaje)) {
        	status=1;
        	index = 0;
        }
    }
	if(status){
		UART_Send(LPC_UART2,msg_3,sizeof(msg_5),BLOCKING);
		UART_Send(LPC_UART2,mensaje,sizeof(mensaje),BLOCKING);
	}
	uint32_t aux = atoi((char*)mensaje);

	switch(aux){
	        case 1:
	        referencia=100;
	        break;
	        case 2:
	        referencia=200;
	        break;
	        case 3:
	        referencia=300;
	        break;
	        case 4:
	        referencia=400;
	        break;
	        case 5:
	        referencia=500;
	        break;
	        case 6:
	        referencia=600;
	        break;
	        default:
	        UART_Send(LPC_UART2,error,sizeof(error),BLOCKING);
	}
return;
}
