/*
Configurar el pin P0.4 como entrada digital con resistencia de pull down
y utilizarlo para decidir si el valor representado por los pines
P0.0 al P0.3 van a ser sumados o restados al valor guardado en
la variable "acumulador".El valor inicial de "acumulador" es 0.

*/

# include "LPC17xx.h"

void configGPIO(void);
int suma(int valor1, int valor2);
int resta(int valor1, int valor2);

int main(void)
{
	configGPIO();
	int numVarios[5]={0x1,0x3,0x7,0x5,0x4};
	int acumulador=0;

	while(1){

		if((LPC_GPIO0 -> FIOPIN & (0x1<<4))== 1){
			for(int i=0;i<5;i++){
				LPC_GPIO0->FIOCLR |= (0xF);
				acumulador = suma(acumulador,LPC_GPIO0->FIOSET |= numVarios[i]);
								}
			}
		else {

			for(int j=0;j<5;j++){
				LPC_GPIO0->FIOCLR |= (0xF);
				acumulador = resta(acumulador,LPC_GPIO0->FIOSET |= numVarios[j]);
								}

			}
			}


	return 0;
}

void configGPIO(void)
{
	LPC_PINCON -> PINSEL0 &= ~(0x3FF); // 0000000000
	LPC_PINCON -> PINMODE0 |=(0x3<<8); //01100000000
	LPC_GPIO0 -> FIODIR |= (0xF); //P0.0-P0.3 salida
	LPC_GPIO0 -> FIODIR &= ~(0x1<<4); //P0.4 entrada

}
int suma(int valor1, int valor2)
{
	int a=0;
	int b=0;
	a=valor1;
	b=valor2;
return a+b;
}
int resta(int valor1, int valor2)
{
	int a=0;
	int b=0;
	a=valor1;
	b=valor2;
	return a-b;
}
