#include "LPC17xx.h"

void cfgPin(void);
void cfgGPIOInt(void);
void cfgSystick(uint32_t ticks);
uint32_t *ptr_toff = (uint32_t*)(0x10002010);
uint32_t *ptr_on   = (uint32_t*)(0x10002014);
uint32_t time_off  = 0;
uint32_t time_on   = 0;
uint32_t nInt=0;

int main(void){
    cfgPin();
    cfgGPIOInt();

    while(1){

    }

    return 0;
}
void SysTick_Handler(void){
    static uint8_t status=0; //status lo uso para saber el estado del led
    status++;
    switch (status%2){
        case 1:
        LPC_GPIO0->FIOCLR |=(1<<8);
        cfgSystick(time_off);
        break;

        case 0:
        LPC_GPIO0->FIOSET |=(1<<8);
        cfgSystick(time_on);
        break;
    }
    SysTick->CTRL &= SysTick->CTRL;
}
void EINT3_IRQHandler(void){
    nInt++;
    switch(nInt%3){
        case 1:
        time_off  = ((*ptr_off)*SystemCoreClock)/2;
        time_on   = ((*ptr_on)*SystemCoreClock)/2;
        break;
        case 2:
        time_off  = ((*ptr_off)*SystemCoreClock)/4;
        time_on   = ((*ptr_on)*SystemCoreClock)/4;
        break;
        case 0:
        time_off  = ((*ptr_off)*SystemCoreClock);
        time_on   = ((*ptr_on)*SystemCoreClock);
        break;
    }
    LPC_GPIO0->FIOSET |= (1<<8); //enciendo el led
    cfgSystick(time_on);
    LPC_GPIOINT-> IO0IntClr |=(1<<6);// limpio el flag

}
void cfgSystick(uint32_t ticks){
    SysTick->LOAD  = (ticks)-1;
    SysTick->CTRL  = (1) | (1<<1) | (1<<2); // habilito el contador; habilito la interrupcion ; uso el clock interno
    SysTick->VAL   = 0;
    SysTick->CTRL &= SysTick->CTRL;
    return;
}
void cfgGPIOInt(void){
    LPC_PINCON ->PINSEL0  &= ~(3<<12); //P0.6 como GPIO
    LPC_PINCON ->PINMODE0 &= ~(3<<12); //pull-up resistor
    LPC_GPIO0  ->FIODIR   &= ~(1<<6);  //como entrada
    LPC_GPIOINT->IO0IntEnF |= (1<<6); //configuro interrupcion por flanco de bajada
    LPC_GPIOINT->IO0IntClr |= (1<<6); // limpio el flag

    NVIC_EnableIRQ(EINT3_IRQn);
    return;
}
void cfgPin(void){
    LPC_PINCON ->PINSEL0  &= ~(3<<16); //P0.8 como GPIO
    LPC_PINCON ->PINMODE0 &= ~(3<<16); // pull-up resitor
    LPC_GPIO0 -> FIODIR   |=  (1<<8);  // como salida
    return;
}