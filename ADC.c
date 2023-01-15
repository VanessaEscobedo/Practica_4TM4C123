#include "lib/include.h"

extern void Configura_Reg_ADC(void)
{
    /*
    PRACTICA ORIGINAL
    Habilitar el modulo 0 del ADC con dos canales analogicos 
    en el puerto E a una velocidad de conversion de 250ksps
    dandole la mayor prioridad al secuenciador 2 con evento
    de procesador 

    EXPERIMENTO
    Usando el modulo 0 y 1 , configurar la tarjeta a la frecuencia asignada,
    para adquirir 6 señales analógicas a una velocidad de 1 millón de muestras 
    por segundo, por los canales asignados  y guardar los valores en un arreglo 
    para ser enviados con un botones  externos asociado al gpio D a través del protocolo de 
    comunicación asíncrona a una velocidad todo esto usando interrupciones.
    b) canales->2,4,6,7,10,1 
    fclk->30MHZ 
    Baud-rate->57600 
    Secuenciadores->sec3, sec1 (el sec3 solo tiene 1 canal, se cambia por el sec2 para poder tener los 6 canales)
    
    modulo 0->sec1
    modulo 1->sec2
    */

    //p. 801 esquema
    //1->PE2, 2->PE1, 4->PD3, 6->PD1, 7->PD0, 10->PB4

    SYSCTL->RCGCADC = (1<<0)|(1<<1); //inicializar el modulo 0 y 1 de reloj del adc (RCGCADC) p. 352 
    //                     F     E      D       C      B     A      
    SYSCTL->RCGCGPIO |= (1<<5)|(1<<4)|(1<<3)|(0<<2)|(1<<1)|(1<<0); //habilitar reloj puertos A, B, D, E y F p. 340, 801
    
    //p. 663 (GPIODIR) habilita los pines como I/O 
    GPIOE_AHB->DIR = (0<<2) | (0<<1); //PE2 y PE1
    GPIOD_AHB->DIR = (0<<3) | (0<<1) | (0<<0); //PD3, PD1 y PD0
    GPIOB_AHB->DIR = (0<<4); //PB4
    //1->salida
    //0->entrada
    
    //habilitar funciones alternativas, para que el modulo analógico tenga control de esos pines (GPIOAFSEL) p. 672 
    GPIOE_AHB->AFSEL = (1<<2) | (1<<1); //PE2 y PE1
    GPIOD_AHB->AFSEL = (1<<3) | (1<<1) | (1<<0); //PD3, PD1 y PD0
    GPIOB_AHB->AFSEL = (1<<4); //PB4
    
    GPIOE_AHB->DEN = (0<<2) | (0<<1);  //deshabilita el modo digital (GPIODEN) p. 683
    GPIOD_AHB->DEN = (0<<3) | (0<<1) | (0<<0); 
    GPIOB_AHB->DEN = (0<<4); 
    
    //registro combinado con el GPIOAFSEL y la tabla p. 1351, p. 688 (GPIOPCTL) 
    GPIOE_AHB->PCTL = GPIOE_AHB->PCTL & (0xFFFFF00F); //la posición indica el pin, el 0 que es ADC
    GPIOD_AHB->PCTL = GPIOD_AHB->PCTL & (0xFFFF0F00);
    GPIOB_AHB->PCTL = GPIOB_AHB->PCTL & (0xFFF0FFFF);
    
    GPIOE_AHB->AMSEL = (1<<2) | (1<<1); //habilitar analogico (GPIOAMSEL) p. 687 
    GPIOD_AHB->AMSEL = (1<<3) | (1<<1) | (1<<0); 
    GPIOB_AHB->AMSEL = (1<<4); 
    
    //el registro (ADCPC) establece la velocidad de conversión por segundo p. 891
    ADC0->PC = (1<<2)|(1<<1)|(1<<0); //para config. a 1 millones de muestra/s
    ADC1->PC = (1<<2)|(1<<1)|(1<<0);
    
    //este registro (ADCSSPRI) configura la prioridad de los secuenciadores p. 841
    ADC0->SSPRI = 0x3102; //mayor prioridad sec1
    ADC1->SSPRI = 0x3021; //mayor prioridad sec2
    
    //(ADCACTSS) este registro controla la activación de los secuenciadores p. 821
    ADC0->ACTSS  =   (0<<3) | (0<<2) | (0<<1) | (0<<0); //primero se desactivan para config.
    ADC1->ACTSS  =   (0<<3) | (0<<2) | (0<<1) | (0<<0);
    
    //este registro (ADCEMUX) selecciona el evento que activa la conversión (trigger) p. 834
    ADC0->EMUX  = (0x0040); //se config. con interrupciones sec1
    ADC1->EMUX  = (0x0400); //sec2
    
    //este registro (ADCSSMUX2) define las entradas analógicas con el canal y secuenciador seleccionado p. 867
    ADC0->SSMUX1 = 0x0246; //canal 2, 4 y 6 para modulo 0
    ADC1->SSMUX2 = 0x07A1; //canal 1, 7 y 10 para modulo 1
    
    //este registro (ADCSSCTL2), configura el bit de control de muestreo y la interrupción p. 868
    ADC0->SSCTL1 = (1<<6) | (1<<5) |  (1<<2) | (1<<1) | (1<<10) | (1<<9);
    ADC1->SSCTL2 = (1<<6) | (1<<5) |  (1<<2) | (1<<1) | (1<<10) | (1<<9);
    
    //*enable ADC Interrupt interrumpe la conversión p. 825
    ADC0->IM |= (1<<1); //* Unmask ADC0 sequence 2 interrupt2*
    ADC1->IM |= (1<<2);
    //NVIC_PRI4_R = (NVIC_PRI4_R & 0xFFFFFF00) | 0x00000020;
    //NVIC_EN0_R = 0x00010000;
    
    //p. 821 (ADCACTSS) Este registro controla la activación de los secuenciadores
    ADC0->ACTSS = (0<<3) | (0<<2) | (1<<1) | (0<<0);
    ADC1->ACTSS = (0<<3) | (1<<2) | (1<<1) | (0<<0);
    //ADC0->PSSI |= (1<<2); se habilita el registro cuando se configura por procesador 
}

//adquisición 
extern void ADC0_InSeq1(uint16_t *Result1)
{
       //***
       while((ADC0->RIS&0x02)==0){}; // espera al convertidor
       Result1[1] = ADC0->SSFIFO1&0xFFF; //  Leer  el resultado almacenado en la pila2
       Result1[0] = ADC0->SSFIFO1&0xFFF;
       Result1[2] = ADC0->SSFIFO1&0xFFF;
       printChar('A');
       ADC0->ISC = 0x0002;  //Conversion finalizada
}

extern void ADC1_InSeq2(uint16_t *Result2)
{
    //ADC Processor Sample Sequence Initiate (ADCPSSI)
       //ADC0->PSSI = 0x00000004;
       while((ADC1->RIS&0x04)==0){}; // espera al convertidor
       Result2[1] = ADC1->SSFIFO2&0xFFF; //  Leer  el resultado almacenado en la pila2
       Result2[0] = ADC1->SSFIFO2&0xFFF; //en FIFO esta el valor de conversión
       Result2[2] = ADC1->SSFIFO2&0xFFF;
       printChar('B');
       ADC1->ISC = 0x0004;  //Conversion finalizada, limpio la conversión
}


