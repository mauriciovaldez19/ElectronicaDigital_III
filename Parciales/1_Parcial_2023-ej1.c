/*	Utilizando Systick e interrupciones externas escribir un codigo en C que cuente indefinidamente de 0 a 9.
 * Un pulsador conectado e EINT0 reiniciará la cuenta a 0 y se mantendrá en ese valor mientras el pulsador se encuentre presionado.
 * Un pulsador conectado a EINT1 permitira detener o continuar la cuenta cada vez que sea presionado.
 * Un pulsador conectado e EINT2 permitira modificar la velocidad de incremento del contador.
 * En Este sentido, cada vez que se presione ese pulsador el contador pasara a incrementar su cuenta de cada 1 segundo a 1 milisegundo
 * y viceversa.
 * Considerar que el microcontrolador se encuentra funcionando con un relog (CCLK) de 16 MHz. *
 *
 *NOTA: Los retardos por systick estan calculados para 100 ms y 1 ms para la frecuencia del CCLK de 100 Mhz.
 */
#include "LPC17xx.h"
void configPin(void);
void configSystick (int k);
void configINT(void);
uint8_t pos =0;
uint32_t auxEINT1=0;
uint32_t auxEINT2=0;
uint16_t clkDiv=100;


int main(void) {
	configSystick(clkDiv);
	configPin();
	configINT();
uint8_t numeros[10]={0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x67};
    while(1){
    	LPC_GPIO0->FIOPIN=((LPC_GPIO0->FIOPIN & (numeros[pos])) | (numeros[pos]));
    	 	}
    return 0 ;
}
void SysTick_Handler(void){
	pos++;
	if(pos==10){
	pos= 0;
	}
	SysTick->CTRL &= SysTick->CTRL; //Clear flag
}
void EINT0_IRQHandler(void){		//Esta interrupcion resetea el contador
	pos=0;							
	configSystick(clkDiv);			// llamo a la funcion configSystick y reseteo la configuracion
	LPC_SC->EXTINT |=(1<<0);
	
	return;
}
void EINT1_IRQHandler(void){		//Esta interrupcion hace de play/pause al contador
	if(auxEINT1%2){ 				//auxEINT1 empieza en 0-> la division por modulo=0->false
		configSystick(clkDiv);		// llamo a la funcion configSystick y reseteo la configuracion
	}
	else{
		SysTick->CTRL=(0x4);		//deshabilito las interrupciones por systick -> la cuenta no se mueve
	}
	auxEINT1++;
	LPC_SC->EXTINT |=(1<<1);		
	return;
}
void EINT2_IRQHandler(void){		//Esta interrupcion cambia la velocidad del contador
	if(auxEINT2%2){
		clkDiv=100;
	}
	else{
		clkDiv=1000;
	}
	configSystick(clkDiv);			//llamo a la funcion configSystic con el nuevo valor de clkDiv
	auxEINT2++;
	LPC_SC->EXTINT |=(1<<1);
	return;
}
void configPin(void){
    LPC_PINCON -> PINSEL0 &=~(0x3FFF); 		//P0.0 a P0.6 as GPIO
    LPC_GPIO0 -> FIODIR |=(0x7F);			//P0.0 - P0.6 as output
    LPC_GPIO0 ->FIOMASK =(0xFFFFFF80);		//mascara para modif. solo pines seleccionados

    LPC_PINCON -> PINSEL4 |= (1<<20);		//P2.10 as EINT0
    LPC_PINCON -> PINSEL4 |= (1<<22);		//P2.11 as EINT1
    LPC_PINCON -> PINSEL4 |= (1<<24);		//P2.12 as EINT2

    LPC_PINCON->PINMODE4 |=(0x3<<20);		// P2.10 pull-down
    LPC_PINCON->PINMODE4 |=~(0xF<<22);		//P2.11-12 pull-up
return;
}
void configSystick (int k){
	SysTick->LOAD = ((SystemCoreClock/k)-1);
	SysTick->CTRL=(0x7);
	SysTick->VAL = 0;
	NVIC_SetPriority(SysTick_IRQn,5);
	return;
}
void configINT(void){
	LPC_SC ->EXTMODE &=~(1<<0);			//EINT0 as level sensitive
	LPC_SC ->EXTMODE |=(0x3<<1); 		//EINT1 - EINT2 as edge sensitive

	LPC_SC ->EXTPOLAR |=(1<<0); 		//EINT0 as high level
	LPC_SC ->EXTPOLAR &=~(0x3<<1); 		//EINT1 - EINT2 as falling edge

	LPC_SC ->EXTINT |=(0x7); 			//clear flags

	NVIC_SetPriority(EINT0_IRQn,0);
	NVIC_SetPriority(EINT1_IRQn,1);
	NVIC_SetPriority(EINT1_IRQn,2);

	NVIC_EnableIRQ(EINT0_IRQn);
	NVIC_EnableIRQ(EINT1_IRQn);
	NVIC_EnableIRQ(EINT2_IRQn);
return;
}
