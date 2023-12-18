// Blink led con Timer0 e Interrupcion por desborde con uso de Drivers

#include "LPC17xx.h"

#include "lpc17xx_pinsel.h"
#include "lpc17xx_gpio.h"
#include "lpc17xx_timer.h"
#include "lpc17xx_clkpwr.h"

#define PIN_22 (1<<22)

void configPin(void);
void configTimer(void);
uint8_t state=0;

int main (void)
{
	configPin();
	configTimer();
	while(1){

	}

return 0;
}
void configPin(void){
	//Configuro el Pin22 como salida
    PINSEL_CFG_Type PinCfg;
	PinCfg.Portnum = PINSEL_PORT_0;
	PinCfg.Pinnum =PINSEL_PIN_22;
	PinCfg.Funcnum = PINSEL_FUNC_0;
	PinCfg.OpenDrain = PINSEL_PINMODE_NORMAL;
	PinCfg.Pinmode = PINSEL_PINMODE_PULLUP;

	PINSEL_ConfigPin(&PinCfg);
	GPIO_SetDir(0,PIN_22,1);

	return;
}

void configTimer(void){
	//parametros PR
	TIM_TIMERCFG_Type timCfg;
	timCfg.PrescaleOption = TIM_PRESCALE_TICKVAL;
	timCfg.PrescaleValue = 1;

	//parametros match
	TIM_MATCHCFG_Type matchCfg;
	matchCfg.MatchChannel = 0;
	matchCfg.IntOnMatch = ENABLE;
	matchCfg.StopOnMatch = DISABLE;
	matchCfg.ResetOnMatch = ENABLE;
	matchCfg.ExtMatchOutputType = TIM_EXTMATCH_NOTHING;
	matchCfg.MatchValue = 25000000;

	TIM_Init(LPC_TIM0, TIM_TIMER_MODE, (void *) &timCfg); //configuro el timer
	TIM_ConfigMatch(LPC_TIM0, &matchCfg); //configuro el match
	TIM_Cmd(LPC_TIM0, ENABLE); //inicializo el timer

	// CLKPWR_SetPCLKDiv(CLKPWR_PCLKSEL_TIMER0,CLKPWR_PCLKSEL_CCLK_DIV_1);  //con esta linea puedo modificar la division del CCLK
	NVIC_EnableIRQ(TIMER0_IRQn);

	return;
}
void TIMER0_IRQHandler(void){
	if(state==0){
		GPIO_ClearValue(0,PIN_22);
		state=1;
	}
	else{
		GPIO_SetValue(0,PIN_22);
		state=0;
	}
	TIM_ClearIntPending(LPC_TIM0, TIM_MR0_INT);
}
