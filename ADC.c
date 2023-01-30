#include "lib/include.h"

extern void Configura_Reg_ADC(void)
{
    /*Usando el modulo 0 y 1 , configurar la tarjeta a la frecuencia asignada,
    * para adquirir 6 señales analógicas a una velocidad de 1 millón de muestras 
    * por segundo, por los canales asignados  y guardar los valores en un arreglo 
    * para ser enviados con un botones  externos asociado al gpio D a través del protocolo de 
    * comunicación asíncrona a una velocidad todo esto usando interrupciones.
    * b) 
    * canales->2,4,6,7,10,1 
    * fclk->20MHZ 
    * Baud-rate->57600 
    * Secuenciadores->sec3, sec1, sec2*/
    
    //modulo 0->sec1
    //modulo 1->sec2
    //modulo 1->sec3

    //p. 801 esquema
    //1->PE2, 2->PE1, 4->PD3, 6->PD1, 7->PD0, 10->PB4 (pines de los canales)

    SYSCTL->RCGCADC = (1<<0)|(1<<1); //inicializar el modulo 0 y 1 de reloj del adc (RCGCADC) p. 352 
    //                     F     E      D       C      B     A      
    SYSCTL->RCGCGPIO |= (1<<5)|(1<<4)|(1<<3)|(0<<2)|(1<<1)|(1<<0); //habilitar reloj puertos A, B, D, E y F p. 340, 801
    
    //p. 663 (GPIODIR) habilita los pines como I/O 
    GPIOE->DIR &= ~(1<<2); //PE2 y PE1
    GPIOE->DIR &= ~(1<<1);
    GPIOB->DIR &= ~(1<<4); //PB4
    GPIOD->DIR &= ~(1<<3); //PD3, PD1 y PD0
    GPIOD->DIR &= ~(1<<1);
    GPIOD->DIR &= ~(1<<0);
    //1->salida
    //0->entrada
    
    //habilitar funciones alternativas, para que el modulo analógico tenga control de esos pines (GPIOAFSEL) p. 672 
    GPIOE->AFSEL = (1<<2) | (1<<1); //PE2 y PE1
    GPIOD->AFSEL = (1<<3) | (1<<1) | (1<<0); //PD3, PD1 y PD0
    GPIOB->AFSEL = (1<<4); //PB4
    
    GPIOE->DEN = (0<<2) | (0<<1);  //deshabilita el modo digital (GPIODEN) p. 683
    GPIOD->DEN = (0<<3) | (0<<1) | (0<<0); 
    GPIOB->DEN = (0<<4); 
    
    //registro combinado con el GPIOAFSEL y la tabla p. 1351, p. 688 (GPIOPCTL) 
    GPIOE->PCTL = GPIOE->PCTL & (0xFFFFF00F); //la posición indica el pin, el 0 que es ADC
    GPIOD->PCTL = GPIOD->PCTL & (0xFFFF0F00);
    GPIOB->PCTL = GPIOB->PCTL & (0xFFF0FFFF);
    
    GPIOE->AMSEL = (1<<2) | (1<<1); //habilitar analogico (GPIOAMSEL) p. 687 
    GPIOD->AMSEL = (1<<3) | (1<<1) | (1<<0); 
    GPIOB->AMSEL = (1<<4); 
    
    //el registro (ADCPC) establece la velocidad de conversión por segundo p. 891
    ADC0->PC = (1<<2)|(1<<1)|(1<<0); //para config. a 1 millones de muestra/s
    ADC1->PC = (1<<2)|(1<<1)|(1<<0);
    
    //este registro (ADCSSPRI) configura la prioridad de los secuenciadores p. 841
    ADC0->SSPRI = 0x3102; //mayor prioridad sec1
    ADC1->SSPRI = 0x1023; //mayor prioridad sec2, segunda prioridad sec3
    
    //(ADCACTSS) este registro controla la activación de los secuenciadores p. 821
    ADC0->ACTSS  = (0<<3) | (0<<2) | (0<<1) | (0<<0); //primero se desactivan para config.
    ADC1->ACTSS  = (0<<3) | (0<<2) | (0<<1) | (0<<0);
    
    //este registro (ADCEMUX) selecciona el evento que activa la conversión (trigger) p. 834
    ADC0->EMUX = (0x0000);
    ADC1->EMUX = (0x0000);
    /*ADC0->EMUX  = (0x0040); //se config. con interrupciones sec1
    ADC1->EMUX  = (0x4400); //sec2, sec3*/
    
    //este registro (ADCSSMUX2) define las entradas analógicas con el canal y secuenciador seleccionado p. 867
    ADC0->SSMUX1 = 0x0246; //canal 2, 4 y 6 para modulo 0
    ADC1->SSMUX2 = 0x00A1; //canal 1 y 10 para modulo 1
    ADC1->SSMUX3 = 0x0007; //canal 7 para modulo 1
    
    //este registro (ADCSSCTL2), configura el bit de control de muestreo y la interrupción p. 868
    ADC0->SSCTL1 = 0x00000644; //3 muestras
    ADC1->SSCTL2 = 0x00000064; //2 muestras
    ADC1->SSCTL3 = 0x00000006; //1 muestra
    
    //*enable ADC Interrupt interrumpe la conversión p. 825
    ADC0->IM |= (1<<1); //* Unmask ADC0 sequence 2 interrupt2*
    ADC1->IM |= (1<<2);
    ADC1->IM |= (1<<3);
    //NVIC_PRI4_R = (NVIC_PRI4_R & 0xFFFFFF00) | 0x00000020;
    //NVIC_EN0_R = 0x00010000;
    
    ADC0->ACTSS |= (0<<3) | (0<<2) | (1<<1) | (0<<0); //activar secuenciadores p. 821
    ADC1->ACTSS |= (1<<3) | (1<<2) | (0<<1) | (0<<0);
    
    ADC0->PSSI |= (0<<1);
    ADC1->PSSI |= (0<<2) | (0<<3); //se habilita el registro cuando se configura por procesador 

    ADC0->ISC = 2; 
    ADC1->ISC = 12;
}

//adquisición 
extern void ADC0_InSeq1(uint16_t Result[6])
{
       ADC0->PSSI = 0x00000002; //se habilita el modo de configuración
       while((ADC0->RIS&0x02)==0){}; // espera al convertidor
       Result[0] = ADC0->SSFIFO1&0xFFF; // leer  el resultado almacenado en la pila
       Result[1] = ADC0->SSFIFO1&0xFFF;
       Result[2] = ADC0->SSFIFO1&0xFFF;
       ADC0->ISC = 0x0002; //conversion finalizada, limpio la conversión
}

extern void ADC1_InSeq2(uint16_t Result[6])
{
       ADC1->PSSI = 0x00000004;
       while((ADC1->RIS&0x04)==0){}; 
       Result[3] = ADC1->SSFIFO2&0xFFF; 
       Result[4] = ADC1->SSFIFO2&0xFFF; //en FIFO esta el valor de conversión
       ADC1->ISC = 0x0004;  
}

extern void ADC1_InSeq3(uint16_t Result[6])
{
       ADC1->PSSI = 0x00000008; 
       while((ADC1->RIS&0x08)==0){};
       Result[5] = ADC1->SSFIFO3&0xFFF; 
       ADC1->ISC = 0x0008;
}
