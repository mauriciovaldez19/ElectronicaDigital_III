#include "LPC17xx.h"

void cfgSystick(void);
void cfgPin(void);
void cfgGPIOInt(void);
uint8_t arreglo[11]={1,0,1,0,1,0,1,0,0,0,0};
uint8_t index=0;

int main(void){
    cfgPin();
    cfgGPIOInt();
    while(1){}
    return 0;
}
void SysTick_IRQHandler(
    index=(index)%11;
    LPC_GPIO2->FIOPIN = (arreglo[index]<<5);
    index++;
    SysTick->CTRL &= SysTick->CTRL;
)
void EINT3_IRQHandler(void){
    index=0;
    configSystick();
    LPC_GPIOINT->IO2IntClr |= (1<<4);
}
void cfgSystick(void){
    SysTick->LOAD=((SystemCoreClock/100)-1);
    SysTick->CTRL= (1<<0) | (1<<1) | (1<<2);
    SysTick->VAL=0;
    SysTick->CTRL &= SysTick->CTRL;
    NVIC_SetPriority(SysTick_IRQn,1);
}
void cfgGPIOInt(void){
    LPC_GPIOINT->IO2IntEnR |= (1<<4);
    LPC_GPIOINT->IO2IntClr |= (1<<4);
    NVIC_EnableIRQ(EINT3_IRQn);
    NVIC_SetPriority(EINT3_IRQn,0);
    return;
}
void cfgPin(void){
    /*considero que al salir del reset, los pines estan en la funcion GPIO por defecto
    y dejo resistencia de pull-up por defecto en P2.5.
    Configuro resistencia de pull-down para P2.4
    */
    LPC_PINCON->PINMODE4 |= (3<<8);
    //Configuro P2.4 como entrada digital y P2.5 como salida
    LPC_GPIO2->FIODIR  &= ~(1<<4);
    LPC_GPIO2->FIODIR  |=  (1<<5);
    // enmascaro los pines que no uso del puerto
    LPC_GPIO2->FIOMASK |= ~((1<<4)|(1<<5));
    return;
}