#include "msp430fr60471.h"
#include <msp430.h>
#include <LCD.h>
#include <Timers.h>
#include <UART.h>
#include <GPIO.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

volatile static uint8_t TaskTry=5;

/*--------------------------------
Funcion: LCD_Init
Parametros de entrada: N/A
Parametros de salida: N/A
Descripcion general: Inicializa los pines de la pantalla 
--------------------------------*/
void LCD_Init(void){
    //Inicializa los pines COM
    P6DIR &= ~(BIT1 | BIT2 | BIT3 | BIT4 | BIT5 | BIT6 | BIT7);
    P6SEL0 = (BIT1 | BIT2 | BIT3 | BIT4 | BIT5 | BIT6 | BIT7);  //R03, R13, R23, COM 0-3
    P6SEL1 = (BIT1 | BIT2 | BIT3 | BIT4 | BIT5 | BIT6 | BIT7);
    P7DIR &= ~(BIT0 | BIT1 | BIT2 | BIT3);
    P7SEL0 = (BIT0 | BIT1 | BIT2 | BIT3);                       //COM 4-7
    P7SEL1 = (BIT0 | BIT1 | BIT2 | BIT3);

    // Inicializa los segmentos de LCD  0-32;
    LCDCPCTL0 = 0xFFFF;     //0-15
    LCDCPCTL1 = 0xFFFF;     //16-31
    LCDCPCTL2 = 0x001F;     //32-36

    // Inicializa LCD_C
    LCDCCTL0 = LCDDIV__10 | LCDPRE__2 | (LCDMX1+LCDMX0+LCDMX2+LCDSON) | LCDLP;
    // Charge pump disabled
    //Internal VLCD, external Rx3 pins, external contrast
    LCDCVCTL = (LCDREXT /*| R03EXT*/);
    //To use internal bias resistors: LCDCVCTL = 0;

    LCDCCPCTL = LCDCPCLKSYNC;               // Clock synchronization enabled

    LCDCMEMCTL = LCDCLRM;                   // Clear LCD memory
    LCDCCTL0 |= LCDON;                      //Enciende LCD

    LCDCCTL0 |= (LCDON);
    //LCDM25|=0x80; //emcemdejr punto 4 decimales

}

#define A 0x1
#define B 0x2
#define C 0x10
#define D 0x40
#define E 0x20
#define F 0x4
#define G 0x8
#define punto 0x80

//hace un loop para imprimir segmento por segmento de los displays de 
//7 segmentos que se tienen en la pantalla
#define Digits_Loop(Address, retardo) \
        Address |= A; delay_ms(retardo); \
        Address |= B; delay_ms(retardo); \
        Address |= C; delay_ms(retardo); \
        Address |= D; delay_ms(retardo); \
        Address |= E; delay_ms(retardo); \
        Address |= F; delay_ms(retardo); \
        Address |= G; delay_ms(retardo); \
        Address |= punto; delay_ms(retardo); 

/*--------------------------------
Funcion: LCDTesting
Parametros de entrada: retardo en ms 
Parametros de salida: N/A
Descripcion general: prende cada uno de los digitos de la pantalla,
uno por uno
--------------------------------*/
void LCDTesting(int retardo){
    LCD_Init();
    /*TOTALIZADOR EN 8s CON PUNTO DECIMAL*/
    Digits_Loop(A1, retardo);
    Digits_Loop(A2, retardo);
    Digits_Loop(A3, retardo);
    Digits_Loop(A4, retardo);
    Digits_Loop(A5, retardo);
    Digits_Loop(A6, retardo);
    Digits_Loop(A7, retardo);
    Digits_Loop(A8, retardo);
    Digits_Loop(A9, retardo);
   
    /*UNIDADES DEL TOTALIZADOR ENCENDIDAS*/
    LCDM19|=0x80; delay_ms(retardo);
    LCDM20|=0x80; delay_ms(retardo);
    LCDM21|=0x80; delay_ms(retardo);
    /*DIGITOS DE CADUAL EN 8s CON PUNTO DECIMAL Y SIGNO NEGATIVO*/
    Digits_Loop(B1, retardo);
    Digits_Loop(B2, retardo);
    Digits_Loop(B3, retardo);
    Digits_Loop(B4, retardo);
    
    LCDM1&=~0x80;
    /*UNIDADES DE CAUDAL ENCENDIDAS*/
    LCDM5|=0x04; delay_ms(retardo);
    LCDM5|=0x08; delay_ms(retardo);
    LCDM5|=0x10; delay_ms(retardo);
    /*SIMBOLOS SUPERIORES ENCENDIDOS*/
    LCDM6|=0x04; delay_ms(retardo);
    LCDM6|=0x10; delay_ms(retardo);
    LCDM6|=0x20; delay_ms(retardo);
    LCDM6|=0x40; delay_ms(retardo);
    LCDM6|=0x80; delay_ms(retardo);
    LCDM6|=0x08; delay_ms(retardo);
    /*SIMBOLOS DE ALARMAS ENCENDIDOS*/
    LCDM30|=0x40; delay_ms(retardo);
    LCDM30|=0x01; delay_ms(retardo);
    LCDM30|=0x02; delay_ms(retardo);
    LCDM30|=0x04; delay_ms(retardo);
    LCDM30|=0x08; delay_ms(retardo);
    LCDM30|=0x10; delay_ms(retardo);
    /*FLECHAS DE DIRECCI�N Y SENTIDO ENCENDIDAS*/
    LCDM30|=0x20; delay_ms(retardo);
    LCDM5|=0x02; delay_ms(retardo);
    LCDM6|=0x01; delay_ms(retardo);
    LCDM6|=0x02; delay_ms(retardo);
    LCDM5|=0x01; delay_ms(retardo);
    LCDM5|=0x40; delay_ms(retardo);
    LCDM5|=0x80; delay_ms(retardo);
    LCDM5|=0x20; delay_ms(retardo);
    /*DIGITOS DE TEMPERATURA 8s CON UNIDADES ENCENDIDAS y SIGNO*/
   //TempLCD(88.8, gC); delay_ms(2000);
    Digits_Loop(LCDM12, retardo);
    Digits_Loop(LCDM13, retardo);
    Digits_Loop(LCDM14, retardo);
    LCDM10|= 0x80; delay_ms(retardo);
    LCDM14|= ~0x80; delay_ms(retardo);
   /*NIVEL DE BATERIA ENCENDIDO*/
    LCDM7|=0x10; delay_ms(retardo);
    LCDM7|=0x20; delay_ms(retardo);
    LCDM7|=0x40; delay_ms(retardo);
    LCDM7|=0x80; delay_ms(retardo);
   /*NIVEL DE SE�AL ENCENDIDO*/
    LCDM7|=0x8; delay_ms(retardo);
    LCDM7|=0x4; delay_ms(retardo);
    LCDM7|=0x2; delay_ms(retardo);
    LCDM7|=0x1; delay_ms(retardo);
   /*FECHA Y HORA ENCENDIDAS EN 8s*/
   Digits_Loop(D1, retardo);
   Digits_Loop(D2, retardo);
   Digits_Loop(D3, retardo);
   Digits_Loop(D4, retardo);
   Digits_Loop(E1, retardo);
   Digits_Loop(E2, retardo);
   Digits_Loop(E3, retardo);
   Digits_Loop(E4, retardo);
   Digits_Loop(E5, retardo);
   Digits_Loop(E6, retardo);
   LCDM9 |= 0x80; delay_ms(retardo);
   LCDM16 |= 0x80; delay_ms(retardo);
   LCDM18 |= 0x80; delay_ms(retardo);
}

/*--------------------------------
Funcion: LCD_OFF
Parametros de entrada: N/A
Parametros de salida: N/A
Descripcion general: apaga todos los digitos de la pantalla
--------------------------------*/
void LCD_OFF(){
    /* TOTALIZADOR EN 8s CON PUNTO DECIMAL */
    A1 = 0x00;
    A2 = 0x00;
    A3 = 0x00;
    A4 = 0x00;
    A5 = 0x00;
    A6 = 0x00;
    A7 = 0x00;
    A8 = 0x00;
    A9 = 0x00;

    /* UNIDADES DEL TOTALIZADOR APAGADAS */
    LCDM19 = 0x00;
    LCDM20 = 0x00;
    LCDM21 = 0x00;

    /* DIGITOS DE CAUDAL EN 8s APAGADOS */
    B1 = 0x00;
    B2 = 0x00;
    B3 = 0x00;
    B4 = 0x00;
    LCDM1 = 0x00;

    /* UNIDADES DE CAUDAL APAGADAS */
    LCDM5 = 0x00;

    /* SÍMBOLOS SUPERIORES APAGADOS */
    LCDM6 = 0x00;

    /* SÍMBOLOS DE ALARMAS APAGADOS */
    LCDM30 = 0x00;

    /* FLECHAS DE DIRECCIÓN Y SENTIDO APAGADAS */
    LCDM5 = 0x00;
    LCDM6 = 0x00;

    /* DIGITOS DE TEMPERATURA APAGADOS */
    LCDM14 = 0x00;
    LCDM12 = 0x00;

    /* NIVEL DE BATERÍA APAGADO */
    LCDM13 = 0x00;
    LCDM11 = 0x00;

    /* NIVEL DE SEÑAL APAGADO */
    LCDM7 = 0x00;

    /* FECHA Y HORA APAGADAS */
    D1 = 0x00;
    D2 = 0x00;
    D3 = 0x00;
    D4 = 0x00;
    E1 = 0x00;
    E2 = 0x00;
    E3 = 0x00;
    E4 = 0x00;
    E5 = 0x00;
    E6 = 0x00;
    LCDM9 = 0x00;
    LCDM16 = 0x00;
    LCDM18 = 0x00;
}

/*--------------------------------
Funcion: LCD_pins_test
Parametros de entrada: N/A
Parametros de salida: N/A
Descripcion general: todos los pines de la pantalla se agrupan en parejas y configura a un pin 
como entrada con resistencia de pulldown y el otro pin como salida y lo pone en alto,
el pin como entrada lee el pin de salida, si esta en alto el pin funciona y se envia mensaje 
por UART indicando que el pin funciona, luego se intercambia el pin que era salida por el 
de entrada y se repite el mismo proceso para todas las parejas
--------------------------------*/
void LCD_pins_test(){
    int i=0;
    bool left_test;
    bool right_test;

    port_var left_ports[19] = {P6, P6, P6, P6, P7, P7, P7, P7, P3, P3, P3, P3, P3, P3, P3, P3, P2, P9, P9};
    port_var right_ports[19] = {P6, P5, P5, P5, P5, P5, P5, P5, P5, P4, P4, P4, P4, P4, P4, P4, P4, P9, P9};
    unsigned int left_bits[19] = {BIT4, BIT5, BIT6, BIT7, BIT0, BIT1, BIT2, BIT3, BIT0, BIT1, BIT2, BIT3, BIT4, BIT5, BIT6, BIT7, BIT7, BIT0, BIT1};
    unsigned int right_bits[19] = {BIT0, BIT7, BIT6, BIT5, BIT4, BIT3, BIT2, BIT1, BIT0, BIT7, BIT6, BIT5, BIT4, BIT3, BIT2, BIT1, BIT0, BIT3, BIT2};
    
    for(i=0; i<19; i++){
        if(test_GPIO(right_ports[i], right_bits[i], left_ports[i], left_bits[i]))
            print_GPIO_result(left_ports[i], left_bits[i], "WORKING");
        else print_GPIO_result(left_ports[i], left_bits[i], "IS DEFINITELY NOT WORKING");
        
        if(test_GPIO(left_ports[i], left_bits[i], right_ports[i], right_bits[i]))
            print_GPIO_result(right_ports[i], right_bits[i], "WORKING");
        else print_GPIO_result(right_ports[i], right_bits[i], "IS DEFINITELY NOT WORKING");
    }

}

/*--------------------------------
Funcion: test_GPIO
Parametros de entrada: puertos de los pines a verificar, y numero de los pines de los puertos
Parametros de salida: true o false
Descripcion general: configura a un pin como entrada con resistencia de pulldown
y el otro pin como salida y lo pone en alto, el pin como entrada lee el pin de 
salida, si esta en alto el pin regresa verdadero, si no lo vuelve a intentar 
5 veces, si nunca se puso en alto regresa false
--------------------------------*/
bool test_GPIO(port_var input_port, unsigned int input_pin, port_var output_port, unsigned int output_pin) {
    
    GPIO_INPUT_EN(input_port, input_pin); 
    GPIO_PULL_DOWN_RES(input_port, input_pin);

    GPIO_OUTPUT_EN(output_port, output_pin);
    GPIO_WRITE(output_port, output_pin, HIGH);
    delay_ms(100);

    if(GPIO_READ(input_port, input_pin)){TaskTry=5; return true;} 
    else{
        TaskTry--;
        print_GPIO_result(output_port, output_pin, "NOT WORKING RETRYING...");
        if(TaskTry == 0){
            TaskTry=5;
            return false;
        }
        test_GPIO(input_port, input_pin, output_port, output_pin);
    } 
}

/*--------------------------------
Funcion: print_GPIO_result
Parametros de entrada: puerto del pin, numero del pin, mensaje sobre funcionamiento del pin
Parametros de salida: N/A
Descripcion general: imprime por UART el resultado de la prueba de pines del puerto y numero 
de pin 
--------------------------------*/
void print_GPIO_result(port_var port, unsigned int pin, const char *cadena) {
    char buffer[50];  
    pin = get_bit_index(pin);  

    sprintf(buffer, "P%d.%d %s\n", port, pin, cadena);

    uartA0_print(buffer);
}

/*--------------------------------
Funcion: get_bit_index
Parametros de entrada: direccion del BIT del puerto 
Parametros de salida: numero entero del BIT
Descripcion general: recive la direccion en exadecimal de BIT0, BIT1, BIT3 etc..
y regresa el numero entero al que corresponde el bit
--------------------------------*/
unsigned int get_bit_index(unsigned int bit) {
    unsigned int index = 0;
    while (bit >>= 1) {
        index++;
    }
    return index;
}

/*--------------------------------
Funcion: delay_ms
Parametros de entrada: retardo en ms
Parametros de salida: N/A
Descripcion general: genera un retardo para usarlo entre 
las pruebas de los pines de la pantalla
--------------------------------*/
void delay_ms(unsigned int ms) {
    // Cada milisegundo equivale a 8000 ciclos de reloj a 8 MHz
    while (ms--) {
        __delay_cycles(8000); // 8000 ciclos = 1 ms
    }
}

/*--------------------------------
Funcion: get_delay_ms
Parametros de entrada: mensaje de UART de cuanto tiempo poner de retardo
Parametros de salida: numero entero del retardo en ms
Descripcion general: esta funcion es usada en la prueba visual de 
la LCD donde por UART se transforma el mensaje en char a entero
para saber cuanto tiempo se quiere poner en lo que se prende y se apaga la pantalla
--------------------------------*/
int get_delay_ms(int Digits){
    int result = 0; 
    int i;

    for (i = 19; i < (19 + Digits); i++) {
        if (UART0BUFF[i] >= '0' && UART0BUFF[i] <= '9') {
            result = result * 10 + (UART0BUFF[i] - '0');
        }
    }

    return result;
}

/*--------------------------------
Funcion: Digits
Parametros de entrada: N/A
Parametros de salida: nuermo entero con la cantidad de digitos que se quieren de retardo
Descripcion general: esta funcion se usa para la prueba visual de la pantalla
donde solo se extrae la cantidad de digitos que tiene el retardo que se quiere aplicar, 
esto se usa despues para extraer el numero exacto de retardo con delay_ms
--------------------------------*/
int Digits(){
    int count = 0;
    int i;

    for (i = 0; i < 30; i++) {
        if (UART0BUFF[i] >= '0' && UART0BUFF[i] <= '9') {
            count++; // Incrementar el contador si es un número
        }
    }

    return count; // Devolver la cantidad de números encontrados
}

void UpdateLCDTime(){
    //CleanLCDTime();
    D1 = (0x80&D1)|lcd_num[(RTCHOUR & 0xF0) >> 4];
    D2 = (0x80&D2)|lcd_num[(RTCHOUR & 0x0F)];
    D3 = (0x80&D3)|lcd_num[(RTCMIN & 0xF0) >> 4];
    D4 = (0x80&D4)|lcd_num[(RTCMIN & 0x0F)];

    E1 = (0x80&E1)|lcd_num[(RTCDAY & 0xF0) >> 4];
    E2 = (0x80&E2)|lcd_num[(RTCDAY & 0x0F)];
    E3 = (0x80&E3)|lcd_num[(RTCMON & 0xF0) >> 4];
    E4 = (0x80&E4)|lcd_num[(RTCMON & 0x0F)];
    E5 = (0x80&E5)|lcd_num[(RTCYEAR & 0x00F0) >> 4];
    E6 = (0x80&E6)|lcd_num[(RTCYEAR & 0x000F)];
    LCDM9 |= 0x80;
    LCDM16 |= 0x80;
    LCDM18 |= 0x80;
}

void float2LCD2(float value, unit_vol unit){

    //asumiendo que value es en litros
    switch (unit) {
        case L:
        LCDM19|=0x80;
        LCDM20&=~0x80;
        LCDM21&=~0x80;
        break;

        case M3:
          value *= 0.001;    //factor de conversion para metros cubicos
          LCDM19&=~0x80;
          LCDM20&=~0x80;
          LCDM21|=0x80;
          break;

        case G:
          value *= 0.264172; //factor de conversion para Galones estadounidense
          LCDM19&=~0x80;
          LCDM20|=0x80;
          LCDM21&=~0x80;

          break;


        default:
            break;
  }

  uint32_t integerPart = (uint32_t) value;
  uint16_t floatPart = (uint16_t) ((10000) * (value - integerPart));



  uint8_t digitCounter;


  for(digitCounter = 0; digitCounter < 5; digitCounter++){ //only 6 digits

      A_Digit(digitCounter + 4, integerPart % 10);
      integerPart /= 10;
  }

  for(digitCounter = 0; digitCounter < 4; digitCounter++){ //only 3 digits

      A_Digit(digitCounter, floatPart % 10);
        floatPart /= 10;

   }

}


