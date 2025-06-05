#include <Vol_Curr_Test.h>

void VCEL_ON();
bool _1V8_ON();
bool QUECTEL_OFF();
void GO_TO_SLEEP_FOR(int _100ms_per_tick);
void delay(unsigned int ms);
void Voltage_Test();

volatile int times_waken_up = 0;

/*--------------------------------
Funcion: Voltage_Test
Parametros de entrada: N/A
Parametros de salida: N/A
Descripcion general: funcion para pruebas de voltages, primero espera a recivir
la cadena TURN VCEL ON por UART, para encender a VCEL y que el ESP32 pueda
medir voltage, luego espera a recivir TURN 1V8 ON esto para activar el 
quectel y que este encienda su fuente de 1.8v y poder medir ese voltage, 
una ves realizadas estas pruebas se espera a recivir la cadena FINISH
para apagar el quectel y salir de la funcion, si se recive EXIT simplemente
se sale de la funcion sin hacer nada ni validar nada
--------------------------------*/
void Voltage_Test(){
    bool is_vcel_on = false;
    bool is_1v8_on = false;

    while(1){
//----------------------------------ENCENDER VECEL-----------------------------------//
        if(READ_UART0_BUFF("TURN VCEL ON\n")){
            is_vcel_on = true;
            VCEL_ON();
            CLEAR_UART0BUFF();
        }
//----------------------------------ENCENDER 1V8-----------------------------------//
        if(is_vcel_on && READ_UART0_BUFF("TURN 1V8 ON\n")){
            uartA0_print("TURNING 1V8 ON\n");
            is_1v8_on = _1V8_ON();
            if(is_1v8_on)
                uartA0_print("QUECTEL IS ON\n");
            else
                uartA0_print("QUECTEL DIDN'T TURN ON\n");
            CLEAR_UART0BUFF();
        }
//----------------------------------APAGAR FUENTES Y QUECTEL-----------------------------------//
        if(READ_UART0_BUFF("FINISH\n")){
            uartA0_print("FINISHING VOLTAGE TEST\n");
            if(QUECTEL_OFF()){
                uartA0_print("QUECTEL IS OFF\n");
                CLEAR_UART0BUFF();
                return;
            }
            else{
                uartA0_print("QUECTEL DIDN'T TURN OFF\n");
                CLEAR_UART0BUFF();
                return;
            }
        }
//----------------------------------SALIDA DE EMERGENCIA-----------------------------------//
        if(READ_UART0_BUFF("EXIT\n"))
            return;
    }
}

/*--------------------------------
Funcion: Current_Test
Parametros de entrada: N/A
Parametros de salida: N/A
Descripcion general: esta funcion solamente activa el modo de ahorro de energia
por 1 segundo, luego hace una medicion de ultrasonico y al final aumenta 
la variable times_waken_up para llevar la cuenta de cuantas veces se encendio 
--------------------------------*/
void Current_Test(){ // el promedio real de corriente que deberia practicamente es aprox 120uA
    GO_TO_SLEEP_FOR(1); // duerme por 1 seg y despierta 
    Uss_measure_for_current_test();
    times_waken_up++;
}

/*--------------------------------
Funcion: VCEL_ON
Parametros de entrada: N/A
Parametros de salida: N/A
Descripcion general: esta funcion configura y activa los pines que se usan 
para activar la alimentacion al quectel
--------------------------------*/
void VCEL_ON(){
    RESET_UART3_BUFF();
 //declarar p7.6, p7.5 y p7.4 como salida y apagar para MOS ON
    P7DIR |= BIT6 | BIT4 | BIT5;
    P7OUT &= ~(BIT6);

//encender pin p7.4 para fuente del quectel
    P7OUT |= (BIT4);

//encender pin p7.5 para cambiador de voltage
    P7OUT |= (BIT5);
}

/*--------------------------------
Funcion: _1V8_ON
Parametros de entrada: N/A
Parametros de salida: indica si encendio el quectel
Descripcion general: esta funcion enciende el quectel y se asegura de recivir
el mensaje APP RDY indicando que el quectel encendio correctamente, si
se recive el mensaje regresa true, si no regresa false
--------------------------------*/
bool _1V8_ON(){
    int TaskTry = 22;

//PWRKEY Y APAGAR P8.5 POR 500 ms
    P8DIR |= (BIT5);
    P8OUT |= (BIT5);
    delay(500);
    P8OUT &= ~(BIT5);

    while(1){
        if(TaskTry<=0)
            return false;
        if(READ_UART3_BUFF(RDY, 18) == 'T'){
            TaskTry = 5;
            while(1){
                DMA3_Transfer(ATE0);
                if(TaskTry<=0)
                    return false;
                if(READ_UART3_BUFF(OK,5) == 'T')
                    return true;
                else{
                    TaskTry--;
                    delay(500);
                }
            }
        }
        else{
            TaskTry--;
            delay(500);
        }
    }
}

/*--------------------------------
Funcion: QUECTEL_OFF
Parametros de entrada: N/A
Parametros de salida: indica si se apago el quectel 
Descripcion general: esta funcion apaga el quectel, primero intenta apagarlo enviando
el comando AT+QPOWD=1 si el quectel contesta con OK POWERED DOWN entonces apaga las
fuentes de alimentacion de quectel y regresa true, si no contesta el quectel 
de igual manera se apagan las fuentes de alimentacion y se regresa false 
--------------------------------*/
bool QUECTEL_OFF(){
    int TaskTry = 4;
    DMA3_Transfer(QPOWD);

    while (1) {
        if(TaskTry<=0){
            P7OUT &= ~BIT4;
            P8OUT &= ~(BIT5);
            P7OUT &= ~BIT5;
            return false;
        }

        if(READ_UART3_BUFF(POWER_DOWN, 22) == 'T'){
            P7OUT &= ~BIT4;
            P8OUT &= ~(BIT5);
            P7OUT &= ~BIT5;
            return true;
        }
        else{
            DMA3_Transfer(QPOWD);
            TaskTry--;
            delay(500);
        }
    }

}

/*--------------------------------
Funcion: GO_TO_SLEEP_FOR
Parametros de entrada: segundos que se quiere dormir el quectel 
Parametros de salida: N/A
Descripcion general: esta funcion activa el modo de ahorro de energia por los
segundos que reciva como parametro, antes de dormirlo activa el timerA0 para que
este empieze a contar ya que aunque este dormido este tipo de timer tiene como
fuente de reloj el ACLK y esta funciona aun cuando el micro duerme, por eso cuenta 
mientras duerme y al final por medio de una interrupcion lo despierta 
--------------------------------*/
void GO_TO_SLEEP_FOR(int _1s_per_tick){
    Start_TimerT_A0_ACLK_1seg(_1s_per_tick);
    __bis_SR_register(GIE+LPM3_bits);  
}

/*--------------------------------
Funcion: delay
Parametros de entrada: retardo en ms
Parametros de salida: N/A
Descripcion general: genera un retardo 
--------------------------------*/
void delay(unsigned int ms){
    // Cada milisegundo equivale a 8000 ciclos de reloj a 8 MHz
    while (ms--) {
        __delay_cycles(8000); // 8000 ciclos = 1 ms
    }
}