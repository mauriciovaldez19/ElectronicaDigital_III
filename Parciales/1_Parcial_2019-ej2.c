#include "LPC17xx.h"

void cfgPin(void);
void cfgGPIOInt(void);
void cfgSystick(void);

uint8_t inputVal=0;

int main(void){
    cfgPin();
    cfgGPIOInt();
    cfgSystick();

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
void cfgSystick(void){
    SysTick -> LOAD = 9999999;
    SysTick -> CTRL = 1 | 1<<1 | 1<<2;
    SysTick -> VAL  = 0;
    
    SysTick -> CTRL &= SysTick -> CTRL;

}
void EINT3_IRQHandler(void){
    cfgSystick();
    LPC_GPIOINT->IO0IntClr |=(1<<8);

}
void SysTick_Handler(void){
static uint8_t condicion = 0;
    if(condicion==0){
        inputVal=((LPC_GPIO0->FIOPIN &(inputVal))|LPC_GPIO0->FIOPIN);
    }
    if(condicion==1){
        inputVal = (inputVal*4);
        if(inputVal>(0xFF)){
            inputVal=(0xFF);
            }
    LPC_GPIO1->FIOPIN = inputVal;
    SysTick -> CTRL &= ~(1);
    condicion=0;
    }
condicion++;
SysTick -> CTRL &= SysTick -> CTRL;
}