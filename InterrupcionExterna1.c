/*Configurar la interrupción externa
 * EINT1 para que interrumpa por flanco de bajada
 * y la interrupción EINT2 para que interrumpa por flanco de subida.
 * En la interrupción por flanco de bajada
 *  configurar el systick para desbordar cada 25 mseg,
 *  mientras que en la interrupción por flanco de subida
 *  configurarlo para que desborde cada 60 mseg.
 *  Considerar que EINT1 tiene mayor prioridad que EINT2.
 */
#include "LPC17xx.h"

void configPin(void);
void configInt(void);
void configSystick(int);

int main(void){
	configPin();
	configInt();

	while(1);

	return 0;
}
void EINT1_IRQHandler(void){
	configSystick(1);
	LPC_SC->EXTINT |=(1<<1);
	return;
}
void EINT2_IRQHandler(void){
	configSystick(0);
	LPC_SC->EXTINT |=(1<<2);
	return;
}
void configPin(void){
	LPC_PINCON->PINSEL4 |= (1<<22);
	LPC_PINCON->PINSEL4 &= ~(1<<23);
	LPC_PINCON->PINSEL4 |= (1<<24);
	LPC_PINCON->PINSEL4 &= ~(1<<25);
return;
}
void configInt(void){
	LPC_SC-> EXTMODE |= (0x6); //configuro para flancos
	LPC_SC->EXTPOLAR &=~(1<<1);// flanco bajada
	LPC_SC->EXTPOLAR |= (0x4);// flanco subida
	LPC_SC->EXTINT |= (0x6);
	NVIC_SetPriority(EINT1_IRQn,0);
	NVIC_SetPriority(EINT2_IRQn,1);
	NVIC_EnableIRQ(EINT1_IRQn);
	NVIC_EnableIRQ(EINT2_IRQn);
return;
}
void configSystick(int aux){
	if(aux==1){
		SysTick->LOAD=249999;
		SysTick->CTRL=1;
	}
	else{
		SysTick->LOAD=5999999;
		SysTick->CTRL=1;
	}

}
