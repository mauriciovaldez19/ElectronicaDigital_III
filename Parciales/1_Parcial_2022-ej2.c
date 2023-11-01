#include "LPC17xx.h"

void cfgPin(void);
void cfgSystick(void);
void cfgEINT(void);
uint8_t status=0;
uint8_t values[8]={0};
float promedio=0;
uint8_t inputVal;
/*la entrada de datos no esta especificada en la consigna, por lo tanto solo dejo declarada la variable inputVal*/

int main(void){
    cfgPin();
    cfgEINT();
    cfgSystic();
    uint8_t suma=0;
    uint8_t index=0;
    
    while(1){
    suma -=values[index];
    values[index]=inputVal;
    suma +=inputVal;
    promedio = (float)suma/8;
    index= (index+1)%8;
    }
    return 0;
}
void Systick_Handler(void){
    LPC_GPIO0->FIOPIN = (LPC_GPIO0->FIOPIN ((0xFF) & promedio))) | promedio;
    SysTick->CTRL &= SysTick->CTRL;
}
void EINT1_IRQHandler(void){ //considero que al iniciar el programa, el contador comienza funcionando
    status++;
    if(status%2){ // si el modulo es 1 deshabilita; si es 0 habilita 
        SysTick->CTRL &= ~(1); //deshabilito el temporizador
    }else{
        cfgSystic(); //llamo a la funcion de configuracion del systick y reinicio el contador completo
    }
    LPC_SC->EXTINT |= (1<<1); //limpio el flag
}
void cfgSystick(void){
    SysTick->LOAD = (SystemCoreClock/100)-1;
    SysTick->CTRL = (1<<0) | (1<<1) | (1<<2); //habilito el contador; habilito interrupcion; selecciono cclk
    SysTick->VAL  = 0;
        SysTick->CTRL &=SysTick->CTRL; //leo y limpio el flag
    NVIC_SetPriority(SysTick_IRQn,0);
}
void cfgEINT(void){
    LPC_PINCON->PINSEL4  |=  (1<<22); //P2.11 como EINT1
    LPC_PINCON->PINMODE4 &= ~(3<<22); //resistencia pull-up
    LPC_SC->EXTMODE  |= (1<<1);// configuro por flanco
    LPC_SC->EXTPOLAR |= (1<<1);//rissing edge
    LPC_SC->EXTINT   |= (1<<1);//limpio el flag
    NVIC_SetPriority(EINT1_IRQn,3);
    NVIC_EnableIRQ(EINT1_IRQn);
    return;
}
void cfgPin(void){
    LPC_PINCON->PINSEL0 &= ~(0xFFFF); //P0.0 - P0.7 funcion GPIO
    LPC_PINCON->PINMODE0 &=~(0xFFFF); // resistencias pull-up
    LPC_GPIO0->FIODIR |=(0xFF); //config como salida
    return;
}
