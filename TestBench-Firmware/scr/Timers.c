#include <msp430.h>
#include <UART.h>

volatile unsigned int Ticks_TB0;
volatile unsigned int Ticks_TA0=0;
volatile unsigned int waiting_time;

/*--------------------------------
Funcion: CFG_TB0_1ms
Parametros de entrada: N/A
Parametros de salida: N/A
Descripcion general: configura al timer BO para contar cada 
milisegundo, usando una interrupcion para aumentar una variable
usa a SMCLK como fuente de reloj
--------------------------------*/
void CFG_TB0_1ms(){
    // Configurar TB0 para generar una interrupción cada 1 ms con SMCLK a 8 MHz.

    TB0CTL = 0;                      // Limpiar configuración previa
    TB0CTL |= TBSSEL__SMCLK | ID__8; // Seleccionar SMCLK, dividir por 8 (1 MHz)
    TB0CCR0 = 1000 - 1;              // Configurar cuenta hasta 1000 (1 ms)
    TB0CCTL0 |= CCIE;                // Habilitar interrupción en CCR0
    TB0R = 0;                        // Reiniciar el contador del temporizador
    TB0CTL |= MC__UP;                // Activar el temporizador en modo "count-up"
}

/*--------------------------------
Funcion: Timer_B0_ISR
Parametros de entrada: N/A
Parametros de salida: N/A
Descripcion general: interrupcion del timer TBO
aumenta la variable Ticks_TB0 cada millisegundo para que otras 
funciones la puedan usar para tomar tiempos o generar retardos
--------------------------------*/
#pragma vector = TIMER0_B0_VECTOR
__interrupt void Timer_B0_ISR(void) {
    Ticks_TB0++;    
    if (Ticks_TB0 >= 65535) { // Reiniciar antes de que se desborde
        Ticks_TB0 = 0;
    }
}

/*--------------------------------
Funcion: Start_TimerT_A0_ACLK_1seg
Parametros de entrada: tiempo de retardo en segundos
Parametros de salida: N/A
Descripcion general: se inicializa el timerA0 usando como fuente de reloj el
cristal de 32.768 kHz y se habilitan las interrupciones para ocurrir cada segundo
--------------------------------*/
void Start_TimerT_A0_ACLK_1seg(int time){
    TA0CTL = 0;
    TA0CTL |= TASSEL__ACLK;  // Tikcs = 1/fACLK = 1/32.768 kHz = 30.51757813 ms
    TA0CCR0 = 32767;          // interrupt ocurrs every 1seg = 32768*30.51757813 ms
    TA0CCTL0 |= CCIE; 
    TA0R = 0;
    TA0CTL |= MC__UP; 
    waiting_time = time;
}

/*--------------------------------
Funcion: Timer_A0_ISR
Parametros de entrada: N/A
Parametros de salida: N/A
Descripcion general: funcion de interrupcion del timer A0, cuando se llega
al tiempo deseado de interrpcion se activa esta funcion, esta funcion aumenta 
la variable Ticks_TA0 cuando esta es igual al tiempo que se recivio en waiting_time
desabilita las interrupciones del timer y despierta al micro, ya que esta pensada 
para usarse para despertar micro
--------------------------------*/
#pragma vector = TIMER0_A0_VECTOR
__interrupt void Timer_A0_ISR(void) {
    ++Ticks_TA0;    
    if(Ticks_TA0 == waiting_time){
        __bic_SR_register_on_exit(LPM3_bits);
        Stop_TimerA();
        Ticks_TA0 = 0;
    }
        
    if (Ticks_TA0 >= 65535) // Reiniciar antes de que se desborde
        Ticks_TA0 = 0;
}

/*--------------------------------
Funcion: Stop_TimerA
Parametros de entrada: N/A
Parametros de salida: N/A
Descripcion general: desabilita las interrupciones del timer A0
--------------------------------*/
void Stop_TimerA(void){
    TA0CCTL0 &= ~CCIE;  // 🔕 Deshabilitar interrupción
    TA0CTL = MC__STOP;  // 🛑 Detener Timer_A
}

/*--------------------------------
Funcion: SMCLK_INIT
Parametros de entrada: N/A
Parametros de salida: N/A
Descripcion general: inicializa el SMCLK a 8MHZ
--------------------------------*/
void SMCLK_INIT(){
    CSCTL0_H = CSKEY_H;                     // Unlock CS registers
    CSCTL1 = DCOFSEL_3 | DCORSEL;           // Set DCO to 8MHz
    CSCTL2 = SELA__VLOCLK | SELS__DCOCLK | SELM__DCOCLK;
    CSCTL3 = DIVA__1 | DIVS__1 | DIVM__1;   // Set all dividers
    CSCTL0_H = 0;                           // Lock CS registers
    //CFG_TB0_1ms();
}

/*--------------------------------
Funcion: SMCLK_INIT_1MHZ
Parametros de entrada: N/A
Parametros de salida: N/A
Descripcion general: inicializa el SMCLK a 1MHZ
--------------------------------*/
void SMCLK_INIT_1MHZ(){
    CSCTL0_H = CSKEY_H;                     // Desbloquear registros de CS
    CSCTL1 = DCOFSEL_0 | DCORSEL;           // Configurar DCO a 1 MHz
    CSCTL2 = SELA__VLOCLK | SELS__DCOCLK | SELM__DCOCLK; // Fuente de reloj sin cambios
    CSCTL3 = DIVA__1 | DIVS__1 | DIVM__1;   // Sin divisores, mantiene 1 MHz
    CSCTL0_H = 0; 
}

