#include "lib/include.h"

int main(void)
{
    uint16_t Result1[3]; //variables
    uint16_t Result2[3];
    float valor1;
    float valor2;
    float valor3;
    float valor4;
    float valor5;
    float valor6;
    
    Configurar_PLL(_20MHZ); //configuracion de la velocidad de reloj ************
    Configura_Reg_ADC(); //configuracion del modulo ADC
    Configurar_UART0(); //configuración del UART 

    //Configurar_GPIO();  //falta configuración GPIO
    
    while(1) 
    {
        ADC0_InSeq1(Result1); //llamada a la conversion por procesador
        valor1=(float)(((Result1[0]))*3.3)/4096;
        valor2=(float)(((Result1[1]))*3.3)/4096;
        valor3=(float)(((Result1[2]))*3.3)/4096;

        ADC1_InSeq2(Result2); 
        valor4=(float)(((Result2[0]))*3.3)/4096;
        valor5=(float)(((Result2[1]))*3.3)/4096;
        valor6=(float)(((Result2[2]))*3.3)/4096;
    }
}
