#include <msp430.h>
#include <Quectel.h>
#include <Timers.h>
#include <UART.h>
#include <LCD.h>
#include <stdbool.h>
#include <Ultrasonic_Sensor_Test.h>
#include <Vol_Curr_Test.h>

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;               // Stop WDT
    PM5CTL0 &= ~LOCKLPM5;

    UART0_INIT();           //inicializa UART0
    UART3_INIT();           //inicializa UART3
    __enable_interrupt();   //Activa interrupciones de manera global 
    CFG_TB0_1ms();          //Activa interrupciones de timerTB0 cada millisegundo
        
    while(1){
        if(UART0_MSG_RECEIVED == 1){
            UART0_MSG_RECEIVED = 0;
/*-------------------FUNCION PARA PRUEBA VISUAL DE PANTALLA--------------------*/
            if(READ_UART0_BUFF("START LCD ANALYSIS\n")){
                uartA0_print("STARTING LCD TEST\n");
                LCDTesting(get_delay_ms(Digits()));
                LCD_OFF();
                uartA0_print("FINISHING LCD TEST\n");
            }
/*-------------------FUNCION PARA PRUEBA ELECTRICA DE PINES DE PANTALLA--------------------*/
            if(READ_UART0_BUFF("START LCD ELECTRICAL TEST\n")){
                uartA0_print("STARTING LCD ELECTRICAL TEST\n");
                LCD_pins_test();
                uartA0_print("FINISHING LCD ELECTRICAL TEST\n");
            }
/*-------------------FUNCION PARA PRUEBA SENSORES ULTRASONICOS--------------------*/
            if(READ_UART0_BUFF("START USS TEST\n")) {
                uartA0_print("STARTING USS TEST\n");
                USS_INIT();
                Uss_measure();
                uartA0_print("FINISHING USS TEST\n");
            }
/*-------------------FUNCION PARA ACTIVAR VOLTAjES Y MEDIRLOS CON ESP32--------------------*/
            if(READ_UART0_BUFF("START VOLTAGE TEST\n")){
                uartA0_print("STARTING VOLTAGE TEST\n");
                Voltage_Test();
                uartA0_print("FINISHING VOLTAGE TEST\n");
            }
/*-------------------FUNCION PARA MEDIR CORRIENTE CON INA219--------------------*/
            if(READ_UART0_BUFF("START CURRENT TEST\n")){
                USS_INIT_FOR_CURRENT_TEST();
                uartA0_print("STARTING CURRENT TEST\n");
                while(!READ_UART0_BUFF("START\n"));
                while(times_waken_up < Times_to_wake_up)  Current_Test();
                times_waken_up = 0;
                uartA0_print("FINISHING CURRENT TEST\n");
            }
/*-------------------FUNCION PARA PROBAR QUECTEL--------------------*/
            if(READ_UART0_BUFF("START QUECTEL ANALYSIS\n")){
                TransmitModeStateMachine(TURN_ON_QUECTEL);
            }
            CLEAR_UART0BUFF();
        }
    }
}
