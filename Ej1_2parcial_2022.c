#include "LPC17xx.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_adc.h"
#include "lpc17xx_timer.h"


static void init_ADC_PIN2(void);
static void init_ADC_PIN4(void);
void ADC_conf(void);
void timer0(void);

int ADC_Val1;
int ADC_Val2;
float v1[20];
float v2[20];


int main(void){
	init_ADC_PIN2();
	init_ADC_PIN4();
	ADC_conf();
	timer0();

	while(1){
	}
	return 0;
}


static void init_ADC_PIN2(void){
	//Configuro el AD0.2

	PINSEL_CFG_Type PinCfg;
	PinCfg.Portnum   = 0;
	PinCfg.Pinnum    = 25;
	PinCfg.Funcnum   = 1;
	PinCfg.Pinmode 	 = PINSEL_PINMODE_TRISTATE;
	PinCfg.OpenDrain = 0;
	PINSEL_ConfigPin(&PinCfg);
	return;
}
static void init_ADC_PIN4(void){
	//Configuro el AD0.4
	PINSEL_CFG_Type PinCfg;
	PinCfg.Portnum   = 1;
	PinCfg.Pinnum    = 30;
	PinCfg.Funcnum   = 1;
	PinCfg.Pinmode 	 = PINSEL_PINMODE_TRISTATE;
	PinCfg.OpenDrain = 0;
	PINSEL_ConfigPin(&PinCfg);
	return;
}
void ADC_conf(void){

	ADC_Init(LPC_ADC, 200000); //Inicializo el adc con 200Khz de frec de muestreo
	LPC_ADC->ADGDR &= ~(1<<31); //limpio el flag DONE
	ADC_ChannelCmd(LPC_ADC,ADC_CHANNEL_2,ENABLE); // habilito el canal2
	ADC_ChannelCmd(LPC_ADC,ADC_CHANNEL_4,ENABLE);// y el 4

	return;
}
void timer0(void){
// prescaler
	TIM_TIMERCFG_Type timCfg;
	timCfg.PrescaleOption=TIM_PRESCALE_TICKVAL;
	timCfg.PrescaleValue=1;
	//parametreos match
	TIM_MATCHCFG_Type matchCfg;
	matchCfg.MatchChannel=0; //match0.0
	matchCfg.IntOnMatch= ENABLE; //que interrumpa
	matchCfg.StopOnMatch=DISABLE;// no se detenga
	matchCfg.ResetOnMatch=ENABLE;
	matchCfg.ExtMatchOutputType=TIM_EXTMATCH_NOTHING;//no haga nada por el pin del match
	matchCfg.MatchValue=624;

	TIM_Init(LPC_TIM0,TIM_TIMER_MODE,(void*)&timCfg);
	TIM_ConfigMatch(LPC_TIM0,&matchCfg);
	TIM_Cmd(LPC_TIM0,ENABLE);
	NVIC_EnableIRQ(TIMER0_IRQn);

	/* el match value lo calcule de la siguiente manera:
	 * Pide digitalizar 2 señales de 10Khz, por lo tanto para cada señal necesitaria muestrear al menos a 20Khz.
	 * Como son 2 canales, entonces la frec de muestreo deberia ser 40Khz al menos.
	 * 40 Khz son 25[uS] entonces entonces
	 * CCLK =100 [Mhz]
	 * PCLK % 4
	 * PR=1
	 * MR=Match Value+1
	 *
	 * de la ecuacion-> Tiempo= 1/(CCLK/4) * PR* MR
	 * MR=624
	 * */



}
void TIMER0_IRQHandler(void){
static uint8_t i=0;
	if(i==20){
		i=0;}

	ADC_StartCmd(LPC_ADC,ADC_START_NOW);// inicio la conversion

	while(!(ADC_ChannelGetStatus(LPC_ADC,ADC_CHANNEL_2,ADC_DATA_DONE)));
			ADC_Val1=ADC_ChannelGetData(LPC_ADC,ADC_CHANNEL_2);
			v1[i]=(ADC_Val1/4096)*3.3;

	while(!(ADC_ChannelGetStatus(LPC_ADC,ADC_CHANNEL_4,ADC_DATA_DONE)));
			ADC_Val2=ADC_ChannelGetData(LPC_ADC,ADC_CHANNEL_4);
			v2[i]=(ADC_Val2/4096)*3.3;
	i++;

	TIM_ClearIntPending(LPC_TIM0, TIM_MR0_INT);
}
