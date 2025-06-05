#ifndef QUECTEL_H_
#define QUECTEL_H_

#include <msp430.h>

/*----------------------------- COMANDOS ---------------------------------*/
#define ATE0            "ATE0\r\n"
#define ATI             "ATI\r\n"
#define AT_CPIN         "AT+CPIN?\r\n"      //consulta si existe SIM conectada
#define AT_GSN          "AT+GSN\r\n"        //obtener el IMEI
#define QLTS            "AT+QLTS=2\r\n"     //obtener la hora local y fecha 
#define CREG            "AT+CREG?\r\n"      //consultar y configurar el estado de registro en la red
#define CSQ             "AT+CSQ\r\n"        //consulta de intensidad de señal
#define QICSGP          "AT+QICSGP=1,1,\"ott.iot.attmex.mx\",\"\",\"\",1\r\n"         //APN IOT AT&T
#define QPOWD           "AT+QPOWD=1\r\n"    //apagar el módulo de manera segura. 

/*------------------------------ RESPUESTAS ---------------------------------*/
#define RDY             "\r\nRDY\r\n\r\nAPP RDY\r\n"    //respuesta al iniciar quectel 
#define ATE0_ANSWER     "\nATE0\r\nOK\r\n"              //respuesta de ATE0
#define OK              "\r\nOK\r\n"
#define ATI_ANSWER      "\r\nQuectel\r\nBG95-M3\r\nRevision: BG95M3LAR02A03\r\n\r\nOK\r\n"
#define CPIN_ANSWER     "\r\n+CPIN: READY\r\n\r\nOK\r\n"//respuesta de cpin
#define CSQ_ANSWER      "\r\n+CSQ:"                     //respuesta para intensidad de señal 
#define POWER_DOWN      "\r\nOK\r\n\r\nPOWERED DOWN\r\n"//respuesta de apagado

#define ReadPort8Bit(bit) ((P8IN  &   bit)==bit)
#define STATUS0 ReadPort8Bit(BIT0)

unsigned char UART3RAM[512];

typedef enum{
    APAGA = 0,
    RESET,
    PRENDE,
    APAGA_NOW,
    SET_ATE0,
    GET_IMEI,
    CHK_CPIN,
    GMR,
    SET_CFUNON,
    SET_QCFG_band,
    SET_QCFG_IOT,
    SET_NWSCANMODE,
    SET_NWSCANSEQ,
    SET_CFUNOFF,
    COPS_,
    COPS__,
    COPS___,
    PotBulk,
    Antenna_info,   //Agregado para localizaci�n por transmisi�n
    CHK_CREG,
    CHK_QCSQ,
    GET_TIMEDAT,
    APN_CFG,
    CFG_Context, //No utilizado. UPDATE 29/01 Ya se utiliza par aconfigurar APN de AT&T. Diego Gomez
    Deact_Context, //No utilizado
    REC_CMGL, //No utilizado
    CFG_HTTP,
    Act_Context,
    Ask_Context,
    CFG_URL,
    CFG_Post_URL,
    CFG_Login_URL,
    CFG_Mark_URL,
    SET_URL,
    HTTP_GET,
    HTTP_READ,
    CHK_CSQ,
    CHK_GMI,
    LOGIN,
    SET_VAR,
    SAVE_TOKEN,
    POST_FAIL,
    POST,
    POST_Answ,
    POST_CONFIG,
    Logger,
    End_Of_Transmition,
    POWER_Off,
    Repeat_Transmition,
    Abort_Transmition,
    POWER_SAVE,
    DefaultValue=0xFF
}SchedulerCELTasks;

typedef enum{
    TURN_ON_QUECTEL,
    TURN_OFF_QUECTEL,
    QUECTEL_STATUS_ON,
    WaitForStatusOn,
    WaitForEndOfTransmition,
    DisableTransmitHardwareBlock,
    ExitTransmitMode,
    STARTCOMMUNICATION,  
    WaitForStatusOff,
    WaitForRestart,
    WaitForRepeatTransmition,
    TRANSMITING,
    FINISHED,
    WAIT_FOR_APP_RDY

}TransmitModeStates;

extern TransmitModeStates QUECTEL_INIT();
extern void TransmitModeStateMachine(TransmitModeStates estado);
extern void quectel_simulation();

#endif /* QUECTEL_H_ */