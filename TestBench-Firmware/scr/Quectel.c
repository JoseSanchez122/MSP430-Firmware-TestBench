#include "msp430fr6047.h"
#include <msp430.h>
#include <Timers.h>
#include <Quectel.h>
#include <UART.h>
#include <stdbool.h>
#include <GPIO.h>

volatile static char INIT_COUNTING = 'F';
volatile static unsigned int previoustime;

volatile static SchedulerCELTasks Comands_Keys[64]; //arreglo para almacenar oden de funciones para verificar comandos
volatile static char IMEI_buff[15];                 //arreglo para almacenar IMEI
volatile static char DATE[10];                      //arreglo para almacenar fecha
volatile static char TIME[10];                      //arreglo para almacenar hora

volatile static unsigned char Posicion=0;           //indice para posicion de Comands_Keys
volatile static unsigned char TaskState=0;          //variable para maquina de estados dentro de las funciones que verifican comandos
volatile static unsigned char TaskTry=0;            //variable para contar el numero de intentos de cada funcion

volatile static TransmitModeStates EstadoTransmisionActual; //variable para manejar los estados en los que se encuentra la prueba de quectel 

void Start_Connection();
TransmitModeStates  QUECTEL_INIT();
void TransmitModeStateMachine(TransmitModeStates estado);
void func_wait_for_app_rdy(void);
void Start_Connection();
void SCH_Act();
void Fcn_GET_IMEI(void);
void Fcn_CHK_PIN(void);
void Fcn_SET_ATE0(void);
void ChgPos(void);
char time_passed(int time);
int countDigits(const char IMEI_buff[15]);
void Fcn_GET_TIMEDAT(void);
void Fcn_CHK_REG(void);
void Fcn_CHK_CSQ(void);
void Fcn_APN_CFG(void);
void Quectel_off(void);

/*--------------------------------
Funcion: QUECTEL_INIT
Parametros de entrada: N/A
Parametros de salida: TransmitModeStates
Descripcion general: inicializa quectel activando fuentes de MOS_ON VCEL 
y el pin Powerkey por 500 ms
--------------------------------*/
TransmitModeStates QUECTEL_INIT(){
    GPIO_INPUT_EN(P8, BIT0);
    GPIO_PULL_DOWN_RES(P8, BIT0);
    if(INIT_COUNTING == 'F'){
        RESET_UART3_BUFF();
        CLEAR_UART3BUFF();
        INIT_COUNTING = 'T';
    //declarar p7.6, p7.5 y p7.4 como salida y apagar para MOS ON
        P7DIR |= BIT6 | BIT4 | BIT5;
        P7OUT &= ~(BIT6);

    //encender pin p7.4 para fuente del quectel
        P7OUT |= (BIT4);

    //encender pin p7.5 para cambiador de voltage
        P7OUT |= (BIT5);

    //PWRKEY Y APAGAR P8.5 POR 500 ms
        P8DIR |= (BIT5);
        P8OUT |= (BIT5);
        Ticks_TB0 = 0;
    }

    if(Ticks_TB0 > 500){
        INIT_COUNTING = 'F';
        P8OUT &= ~(BIT5);
        return WAIT_FOR_APP_RDY;
    }

    return TURN_ON_QUECTEL;
        
}

/*--------------------------------
Funcion: TransmitModeStateMachine
Parametros de entrada: TransmitModeStates
Parametros de salida: N/A
Descripcion general: inicializa el quectel y menvia los comandos
necesarios para verificar el funcionamiento del quectel, registrando las 
respuestas y enviandolas por UART, al final siempre apaga el quectel
--------------------------------*/
void TransmitModeStateMachine(TransmitModeStates estado){
    EstadoTransmisionActual = estado;
    while(1){
        switch(EstadoTransmisionActual){
            case TURN_ON_QUECTEL:
                EstadoTransmisionActual = QUECTEL_INIT();
                break;
            case WAIT_FOR_APP_RDY:
                func_wait_for_app_rdy();
                break;
            case STARTCOMMUNICATION:
                Start_Connection();
                EstadoTransmisionActual = TRANSMITING;
                break;
            case TRANSMITING:
                SCH_Act();
                break;
            case TURN_OFF_QUECTEL:
                Quectel_off();
                break;
            case FINISHED:
                uartA0_print("Quectel finished\n");
                Posicion=0;
                return;          
        }
    }      
}

/*--------------------------------
Funcion: Start_Connection
Parametros de entrada: N/A
Parametros de salida: N/A
Descripcion general: le asigna valores de enumeraciones al arreglo
Comands_Keys estos valores indican el orden en que se verifican 
los comandos enviados al quectel
--------------------------------*/
void Start_Connection(){
    unsigned char i = 0;
    Comands_Keys[i++] = SET_ATE0;
    Comands_Keys[i++] = GET_IMEI; 
    Comands_Keys[i++] = CHK_CPIN;
    Comands_Keys[i++] = APN_CFG;
    Comands_Keys[i++] = CHK_CREG;
    Comands_Keys[i++] = GET_TIMEDAT;
    Comands_Keys[i++] = CHK_CSQ; 
    Comands_Keys[i++] = POWER_Off; 
}

/*--------------------------------
Funcion: SCH_Act
Parametros de entrada: N/A
Parametros de salida: N/A
Descripcion general: dependiendo del indice de Comands_Keys se ejecutan
las diferentes funciones para verificar las respuestas del quectel 
--------------------------------*/
void SCH_Act(){
    switch(Comands_Keys[Posicion]){
        case SET_ATE0: // si no se logra desactivar el eco no se ejecutan el resto de las funciones
            Fcn_SET_ATE0(); // quitar eco
            break;
        case GET_IMEI: // si no se obtiene el IMEI solo se advierte y continua
            Fcn_GET_IMEI(); //obtener IMEI
            break;
        case CHK_CPIN: // si no se encuentra la SIM incertada no se ejecutan las demas funciones
            Fcn_CHK_PIN(); // verificar que la SIM este incertada
            break;
        case APN_CFG: // si el APN no se logra verificar solo se advierte y continua
            Fcn_APN_CFG(); // configurar usuario y contraseña de APN
            break;
        case CHK_CREG: // si no se obtiene acceso a la red no se ejecutan las demas funciones
            Fcn_CHK_REG(); // 
            break;
        case GET_TIMEDAT: // si no se obtiene fecha y hora solo se advierte y continua
            Fcn_GET_TIMEDAT();
            break;
        case CHK_CSQ: // si no se obtiene la calidad de la señal solo se advierte y continua
            Fcn_CHK_CSQ(); // verificar calidad de señal
            break;
        case POWER_Off:
            Quectel_off();
            break;
    }
}

/*--------------------------------
Funcion: func_wait_for_app_rdy
Parametros de entrada: N/A
Parametros de salida: N/A
Descripcion general: una ves se inicio el quectel verifica el pin
STATUS0 se encuentre en alto y espera a que el quectel conteste con
APP RDY para comenzar a enviar comandos
--------------------------------*/
void func_wait_for_app_rdy(void){
    char tiempo;
    char bool_var = 'T';
    int i;
  
    switch(TaskState){
        case 0:
            TaskTry = 20;
            uartA0_print("WAITING FOR APP_RDY\n");
            TaskState = 1;
            break;
        case 1:
            tiempo = time_passed(700);
            if(tiempo != 'F'){
                TaskState = 2;
            }
            break;
        case 2:
            bool_var = READ_UART3_BUFF(RDY, 18);
            if(TaskTry <= 0){
                uartA0_print("ERROR: APP RDY WAS NOT RECEIVED\n");
                print_status();
                EstadoTransmisionActual = TURN_OFF_QUECTEL;
                break;
            }

            if(bool_var == 'T'){
                uartA0_print("APP_RDY RECIVIDO\n");
                print_status();
                EstadoTransmisionActual = STARTCOMMUNICATION;
                TaskState = 0;
                TaskTry = 4;
            }
            else{
                TaskState = 1;
                TaskTry--;
            }
            break;
    }
}

/*--------------------------------
Funcion: print_status
Parametros de entrada: N/A
Parametros de salida: N/A
Descripcion general: envia por UART el estado del pin 
STATUS0
--------------------------------*/
void print_status(){
    uartA0_print("STATUS0: ");
    if(GPIO_READ(P8, BIT0))
        uartA0_print("HIGH\n");
    else uartA0_print("LOW\n");
}

/*--------------------------------
Funcion: Fcn_SET_ATE0
Parametros de entrada: N/A
Parametros de salida: N/A
Descripcion general: Envia el comando ATE0 esperando que el quectel conteste con OK
si el quectel no contesta con OK no se ejecutan las demas funciones ya que no se 
desactivo el eco
--------------------------------*/
void Fcn_SET_ATE0(void){
    char tiempo;
    char CMD_ATE0_SENT = 'T';
    switch (TaskState) {
        case 0: //mandar un primer ATE0
            DMA3_Transfer(ATE0);
            TaskState = 1;
            break;
        case 1: //mandar un segundo ATE0 despues de 500ms
            tiempo = time_passed(500);
            if(tiempo != 'F'){
                DMA3_Transfer(ATE0);
                TaskState = 2;
            }
            break;
        case 2: //Verificar si llego OK
            tiempo = time_passed(500);
            if(tiempo != 'F'){
                CMD_ATE0_SENT = READ_UART3_BUFF(OK,5);
                //si nos quedamos sin intentos mandar error
                if(TaskTry<=0){
                    uartA0_print("ERROR: ECHO WAS NOT DEACTIVATED\n");
                    EstadoTransmisionActual = TURN_OFF_QUECTEL;
                    break;
                }

                //verificar si ATE0 funciono, si no restar un intento
                if(CMD_ATE0_SENT == 'T'){
                    uartA0_print("ECHO DEACTIVATED\n");
                    ChgPos();
                }
                else{
                    uartA0_print("ECHO WAS NOT DEACTIVATED RETRYING...");
                    TaskState = 1;
                    TaskTry--;
                }
            }
            break;
    }

}

/*--------------------------------
Funcion: Fcn_GET_IMEI
Parametros de entrada: N/A
Parametros de salida: N/A
Descripcion general: Envia el comando AT+GSN esperando que el quectel conteste
com el IMEI registrado, si no contesta solo continua con las demas funciones
--------------------------------*/
void Fcn_GET_IMEI(void){
    char CMD_IMEI = 'F';
    char tiempo;
    int i=0;
    int length=0;

    switch (TaskState) {
        case 0:
            DMA3_Transfer(AT_GSN);
            TaskState = 1;
            break;
        case 1:
            tiempo = time_passed(500);
            if(tiempo == 'T'){
                for(i=2; i<17; i++){
                    IMEI_buff[i-2] = UART3BUFF[i];
                }
                length = countDigits(IMEI_buff);
                if(TaskTry<=0){
                    uartA0_print("ERROR: IMEI WAS NOT RECEIVED\n");
                    ChgPos();
                    break;
                }
                if(length == 15){
                    uartA0_print("IMEI RECEIVED: ");
                    int i = 0;
                    while(i < 15){
                        while(!(UCA0IFG&UCTXIFG));
                        UCA0TXBUF = IMEI_buff[i++];
                    }
                    uartA0_print_1DIG('\n');
                    ChgPos();
                }
                else{
                    uartA0_print("IMEI WAS NOT RECEIVED. RETRYING...\n");
                    TaskState = 0;
                    TaskTry--;
                }
            }
            break;
    }
}

/*--------------------------------
Funcion: Fcn_CHK_PIN
Parametros de entrada: N/A
Parametros de salida: N/A
Descripcion general: verifica si la tarjeta SIM funciona enviando el comando
AT_CPIN, en caso de que no funcione las demas funciones no se ejecutan 
--------------------------------*/
void Fcn_CHK_PIN(void){
    char CMD_AT_CPIN = 'F';
    char tiempo;
    switch (TaskState) {
        case 0:
            DMA3_Transfer(AT_CPIN);
            TaskState = 1;
            break;
        case 1:
            tiempo = time_passed(500);
            if(tiempo != 'F'){
                CMD_AT_CPIN = READ_UART3_BUFF(CPIN_ANSWER, 22);
                if(TaskTry<=0){
                    uartA0_print("ERROR: SIM NOT WORKING\n");
                    EstadoTransmisionActual = TURN_OFF_QUECTEL;
                    break;
                }

                if(CMD_AT_CPIN == 'T'){
                    uartA0_print("SIM WORKING\n");
                    ChgPos();
                }
                else{
                    uartA0_print("SIM IS NOT WORKING. RETRYING...\n");
                    TaskState = 0;
                    TaskTry--;
                }
            }
            break;
    }
}

/*--------------------------------
Funcion: Fcn_APN_CFG
Parametros de entrada: N/A
Parametros de salida: N/A
Descripcion general: registra el APN IOT AT&T, en caso de que no se logre
regiatrar solo se indica por UART y se continua con las demas funciones
--------------------------------*/
void Fcn_APN_CFG(void){
    char tiempo;
    char APN_cmd; 
    switch (TaskState) {
        case 0:
            DMA3_Transfer(QICSGP);
            TaskState = 1;
            break;
        case 1:
            tiempo = time_passed(500);
            if(tiempo == 'T'){
                APN_cmd = READ_UART3_BUFF(OK,5);
        
                if(TaskTry<=0){
                    uartA0_print("ERROR: APN WAS NOT CONFIGURED\n");
                    ChgPos();
                    break;
                }

                if(APN_cmd == 'T'){
                    uartA0_print("APN CONFIGURED\n");
                    ChgPos();
                }
                else{
                    uartA0_print("APN WAS NOT CONFIGURED. RETRYING...\n");
                    TaskState = 0;
                    TaskTry--;
                }  
            }
            break;
    }
}

/*--------------------------------
Funcion: Fcn_CHK_REG
Parametros de entrada: N/A
Parametros de salida: N/A
Descripcion general: verifica que el quectel logre registrarse en la red local
usando la SIM, se imprime a la salida los significados de cada respuesta del 
quectel, en caso de que no se logre registrar no se continua con las demas
funcioes 
--------------------------------*/
void Fcn_CHK_REG(void){
    const char *ERROR = "ERROR: NOT CONECTED TO NETWORK\n";
    char modo;
    char stat;
    char tiempo;
    const char *mode_0 = "MODE: No Notifications\n";
    const char *mode_1 = "MODE: Basic Notifications\n";
    const char *mode_2 = "MODE: Detailed Notifications\n";

    const char *stat_0 = "STATUS: Not registered, not searching for network.\n";
    const char *stat_1 = "STATUS: Registered, home network.\n";
    const char *stat_2 = "STATUS: Not registered, searching for network.\n";
    const char *stat_3 = "STATUS: Registration denied.\n";
    const char *stat_4 = "STATUS: Unknown.\n";
    const char *stat_5 = "STATUS: Registered, roaming.\n"; 

    switch (TaskState) {
        case 0:
            DMA3_Transfer(CREG);
            TaskState = 1;
            break;
        case 1:
            tiempo = time_passed(500);
            if(tiempo == 'T'){
                modo = UART3BUFF[9];
                stat = UART3BUFF[11];

                if(TaskTry<=0){
                    uartA0_print(ERROR);
                    EstadoTransmisionActual = TURN_OFF_QUECTEL;
                    break;
                }

                switch (modo) {
                    case '0': uartA0_print(mode_0); break;
                    case '1': uartA0_print(mode_1); break;
                    case '2': uartA0_print(mode_2); break;
                }

                switch (stat) {
                    case '0': 
                        uartA0_print(stat_0); 
                        TaskState = 0;
                        TaskTry--;
                        break;
                    case '1': 
                        uartA0_print(stat_1); 
                        ChgPos();
                        break;
                    case '2': 
                        uartA0_print(stat_2); 
                        TaskState = 0;
                        TaskTry--;
                        break;
                    case '3': 
                        uartA0_print(stat_3); 
                        TaskState = 0;
                        TaskTry--;
                        break;
                    case '4': 
                        uartA0_print(stat_4); 
                        TaskState = 0;
                        TaskTry--;
                        break;
                    case '5': 
                        uartA0_print(stat_5);
                        ChgPos();
                        break;
                }

            }

            break;
    }

}

/*--------------------------------
Funcion: Fcn_GET_TIMEDAT
Parametros de entrada: N/A
Parametros de salida: N/A
Descripcion general: si se logro conectar el quectel a una red local 
se manda el comando AT+QLTS=2 para obtener la hora y fecha local 
--------------------------------*/
void Fcn_GET_TIMEDAT(void){
    char TIME_DATE_buff[20];
    char tiempo;
    int i;

    switch (TaskState) {
        case 0: //inicializar los intentos
            TaskTry = 3;
            TaskState = 1;
            break;
        case 1:
            DMA3_Transfer(QLTS);
            TaskState = 2;
            break;
        case 2:
            tiempo = time_passed(500);
            if(tiempo == 'T'){
                for(i=10; i<29; i++){
                    TIME_DATE_buff[i-10] = UART3BUFF[i];
                }

                if(TaskTry<=0){
                    uartA0_print("ERROR: NO RESPONSE FOR DATE AND TIME\n");
                    ChgPos();
                    break;
                }

                for(i =0; i<19; i++){
                    if(i<10)
                        DATE[i] = TIME_DATE_buff[i];
                    if(i>10)
                        TIME[i-11] = TIME_DATE_buff[i];
                }

                if(DATE[4] == '/' && DATE[7] == '/'){
                    uartA0_print("DATE: ");
                    for(i =0; i<10; i++){
                        while(!(UCA0IFG&UCTXIFG));
                        UCA0TXBUF = DATE[i];
                    }
                    uartA0_print_1DIG('\n');

                    uartA0_print("TIME: ");
                    for(i =0; i<8; i++){
                        while(!(UCA0IFG&UCTXIFG));
                        UCA0TXBUF = TIME[i];
                    }
                    uartA0_print_1DIG('\n');
                    ChgPos();
                }
                else{
                    uartA0_print("DATE AND TIME WERE NOT RECEIVED. RETRYING...\n");
                    TaskState = 1;
                    TaskTry--;
                }
            }
            break;
    }
}

/*--------------------------------
Funcion: Fcn_CHK_CSQ
Parametros de entrada: N/A
Parametros de salida: N/A
Descripcion general: se verifica la intensidad de la señal y se 
imprime por UART
--------------------------------*/
void Fcn_CHK_CSQ(void){
    char tiempo;
    char CSQ_cmd;

    switch (TaskState) {
        case 0:
            DMA3_Transfer(CSQ);
            TaskState = 1;
            break;
        case 1:
            tiempo = time_passed(500);
            if(tiempo == 'T'){
                CSQ_cmd = READ_UART3_BUFF(CSQ_ANSWER, 7);

                if(TaskTry<=0){
                    uartA0_print("ERROR: QUALITY NOT RECEIVED\n");
                    ChgPos();
                    break;
                }

                if(CSQ_cmd == 'T'){
                    uartA0_print("signal_quality: "); 
                    uartA0_print_1DIG(UART3BUFF[8]); 
                    uartA0_print_1DIG(UART3BUFF[9]);
                    uartA0_print_1DIG('\n');
                    ChgPos();
                }
                else{
                    uartA0_print("SIGNAL QUALITY WAS NOT RECEIVED. RETRYING...\n");
                    TaskState = 0;
                    TaskTry--;
                }
            }
            break;
    }
}

/*--------------------------------
Funcion: Quectel_off
Parametros de entrada: N/A
Parametros de salida: N/A
Descripcion general: esta funcion siempre se ejecuta, funcione o no 
la prueba al final siempre se apaga el quectel para que no quede consumiendo
energia
--------------------------------*/
void Quectel_off(void){
    TaskTry = 4;
    DMA3_Transfer(QPOWD); // comando para apagar  
    char quectel_off_cmd = 'F';

    while(1){
        Ticks_TB0 = 0;
        while(Ticks_TB0 < 500);
        if(TaskTry<=0){
            P7OUT &= ~BIT4;
            P8OUT &= ~(BIT5);
            P7OUT &= ~BIT5;
            uartA0_print("ERROR: QUECTEL WASN'T TURNED OFF\n");
            EstadoTransmisionActual = FINISHED;
            TaskState = 0;
            break;
        }
        quectel_off_cmd = READ_UART3_BUFF(POWER_DOWN, 22);
        if(quectel_off_cmd == 'T'){   
            P7OUT &= ~BIT4;
            P8OUT &= ~(BIT5);
            P7OUT &= ~BIT5;
            uartA0_print("QUECTEL WAS TURNED OFF SUCCESSFULLY\n");
            EstadoTransmisionActual = FINISHED;
            TaskState = 0;
            break;
        }
        else{
            DMA3_Transfer(QPOWD);
            uartA0_print("QUECTEL WASN'T TURNED OFF RETRYING...\n");
            TaskTry--;
        }
    }    
}

/*--------------------------------
Funcion: ChgPos
Parametros de entrada: N/A
Parametros de salida: N/A
Descripcion general: va cambiando las posiciones del arreglo Comands_Keys 
el cual tiene dentro las enumeraciones que indican que funcion ejecutar
tambien resetea los intentos para que todas las funciones empiezen con 4
intentos y el TaskState para que todas empiezen desde el estado inicial
--------------------------------*/
void ChgPos(void){
    TaskTry = 4;
    TaskState = 0;
    Posicion++;
}

/*--------------------------------
Funcion: time_passed
Parametros de entrada: tiempo en ms
Parametros de salida: T o F
Descripcion general: la funcion regresa T si ha pasado el tiempo que recivio
como parametro, si no regresa F, esta funcion se puede ingresar y salir y seguira contando
usa una variable que va aumentando el timerBO con interrupciones para contar tiempo 
--------------------------------*/
char time_passed(int time){
    char bool_var = 'F';
    
    if(INIT_COUNTING == 'F'){
        INIT_COUNTING = 'T';
        Ticks_TB0 = 0;
        previoustime = Ticks_TB0;
    }

    int timenow = Ticks_TB0;
    
    if(timenow - previoustime > time){
        INIT_COUNTING = 'F';
        bool_var = 'T';
        return bool_var;
    }

    return bool_var;
}

/*--------------------------------
Funcion: countDigits
Parametros de entrada: arreglo char de tamaño 15
Parametros de salida: numero entero
Descripcion general: se utiliza con la funcion de IMEI para contar
los digitos que se recivieron, devuelve el numero de numeros enteros
contados en el arreglo
--------------------------------*/
int countDigits(const char IMEI_buff[15]){
    int count = 0;
    int i;

    for (i = 0; i < 15; i++) {
        if (IMEI_buff[i] >= '0' && IMEI_buff[i] <= '9') {
            count++; // Incrementar el contador si es un número
        }
    }

    return count; // Devolver la cantidad de números encontrados
}

/*--------------------------------
Funcion: quectel_simulation
Parametros de entrada: N/A
Parametros de salida: N/A
Descripcion general: esta funcion solo se utilizo para pruebas
para simular que el quectel responde
--------------------------------*/
void quectel_simulation(){
    uartA0_print("WAITING FOR APP_RDY\n"); delay_ms_2(3000);
    uartA0_print("APP_RDY RECIVIDO\n");delay_ms_2(500);
    uartA0_print("ATE0 ACTIVADO\n");delay_ms_2(500);
    uartA0_print("IMEI RECEIVED: 864593050463198\n");delay_ms_2(500);
    uartA0_print("SIM WORKING\n");delay_ms_2(500);
    uartA0_print("APN CONFIGURED\n");delay_ms_2(500);
    uartA0_print("MODE: No Notifications\n");delay_ms_2(500);
    uartA0_print("STATUS: Registered, home network.\n");delay_ms_2(500);
    uartA0_print("DATE: 2025/01/31\n");delay_ms_2(500);
    uartA0_print("TIME: 11:47:27\n");delay_ms_2(500);
    uartA0_print("signal_quality: 18\n");delay_ms_2(500);
    uartA0_print("Quectel finished\n");delay_ms_2(500);
}