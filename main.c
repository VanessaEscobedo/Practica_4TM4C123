#include "lib/include.h"

extern void int_to_char(uint16_t dato) //función para convertir un dato tipo int a un dato tipo char
{
    char string[5];
    int i = 3;
    int j = 0; 
    int n_digit;  
    while (i >= 0)
    {
        n_digit = (int)dato / (int)(pow(10, i));
        string[j] = n_digit + '0';
        dato = dato - (n_digit * pow(10, i)); 
        i = i - 1; 
        j++;
    }
    string[4] = '\n';
    printString(string); //se envia el valor (char) con el UART
}

int main(void)
{
    uint16_t Result[6]; //variables
    uint16_t valor1; 
    uint16_t valor2;
    uint16_t valor3;
    uint16_t valor4;
    uint16_t valor5;
    uint16_t valor6;
    
    Configurar_PLL(_20MHZ); //configuracion de la velocidad de reloj 
    Configura_Reg_ADC(); //configuracion del modulo ADC
    Configurar_UART0(); //configuración del UART4

    //Configurar_GPIO();  
    
    while(1) 
    {
        ADC0_InSeq1(Result); //llamada a la conversion 
        ADC1_InSeq2(Result);
        ADC1_InSeq3(Result);
        
        valor1 = (uint16_t)(Result[0]); //se almacenan los valores de los 6 canales en Result[n]
        valor2 = (uint16_t)(Result[1]);
        valor3 = (uint16_t)(Result[2]);
        valor4 = (uint16_t)(Result[3]);
        valor5 = (uint16_t)(Result[4]);
        valor6 = (uint16_t)(Result[5]);

        //una vez que se guardaron los valores, hay que enviarlos
        int_to_char(valor1); //convertir a tipo char cada valor
        int_to_char(valor2); 
        int_to_char(valor3); 
        int_to_char(valor4);
        int_to_char(valor5);
        int_to_char(valor6);

        /*valor1=(float)(((Result[0]))*3.3)/4096;
        valor2=(float)(((Result[1]))*3.3)/4096;
        valor3=(float)(((Result[2]))*3.3)/4096;

        ADC1_InSeq2(Result); 
        valor4=(float)(((Result2[0]))*3.3)/4096;
        valor5=(float)(((Result2[1]))*3.3)/4096;
        valor6=(float)(((Result2[2]))*3.3)/4096;*/
    }
}
