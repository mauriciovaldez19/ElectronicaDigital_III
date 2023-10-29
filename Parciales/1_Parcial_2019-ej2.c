#include "LPC17xx.h"

void cfgPin(void);
void cfgGPIOInt(void);

uint8_t valor=0;
uint8_t condicion=0;

int main(void){
    cfgPin();
    cfgGPIOInt();

    while(1){};

    return 0;
}
void cfgPin(void){
    LPC_PINCON->PINSEL0 &=~(0x3FFFF);
    LPC_PINCON->PINSEL2 &=~(0xFFFF);
    LPC_GPIO0->FIODIR &=~(0x1FF);
    LPC_GPIO1->FIODIR |=(0xFF);

    return;
}
void cfgGPIOInt(void){
    LPC_GPIOINT->IO0IntEnF |=(1<<8);
    LPC_GPIOINT->IO0IntClr |=(1<<8);
    NVIC_EnableIRQ(EINT3_IRQn);
    return;
}
void EINT3_IRQHandler(void){
    if(SysTic_Config(10000000)){};
    while(condicion==0){};
    valor=((LPC_GPIO0->FIOPIN &(0xFF))|LPC_GPIO0->FIOPIN);
    if(SysTick_Config(10000000)){};
    while(condicion==1){};
    valor=(valor*4);
    if(valor>(0xFF)){
        valor=(0xFF);
    }
    LPC_GPIO1->FIOPIN = valor;
    LPC_GPIOINT->IO0IntClr |=(1<<8);

}
void SysTick_Handler(void){
    condicion++;
}