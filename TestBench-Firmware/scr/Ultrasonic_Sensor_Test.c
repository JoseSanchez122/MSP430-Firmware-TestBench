#include <Ultrasonic_Sensor_Test.h>

void handlePllUnlockEvent(void){
    // If USS PLL unlock event is detected rest USS Module and reconfigure
    // measurement
    uartA0_print("handlePllUnlockEvent\n");
    USS_resetUSSModule(&gUssSWConfig, true);
}

/*--------------------------------
Funcion: USS_INIT
Parametros de entrada: N/A
Parametros de salida: resultado de inicializacion 
Descripcion general: se configura la libreria para usar la medicion ultrasonica 
usando la estructura gUssSWConfig que ya se configuro previamente en el archivo 
userconfig, los resultados de la configuracion se imprimen por UART 
--------------------------------*/
bool USS_INIT(){
    USS_message_code code;

    USS_registerHSPLLInterruptCallback(USS_HSPLL_Interrupt_PLLUNLOCK,
                                        &handlePllUnlockEvent);

    code = USS_configureUltrasonicMeasurement(&gUssSWConfig);
    uartA0_print("configuracion Result: ");
    uartA0_print_code(code);

    code = USS_verifyHSPLLFrequency(&gUssSWConfig, &testResults);
    uartA0_print("HSPLL Frequency Results: ");
    uartA0_print_code(code);
    uartA0_print("expectedResult: "); 
    uartA0_print_code(testResults.expectedResult);
    uartA0_print("actualTestCount: "); 
    uartA0_print_code(testResults.actualTestCount);
    uartA0_print("difference: "); 
    uartA0_print_code(testResults.difference);

    code = USS_initAlgorithms(&gUssSWConfig);
    uartA0_print("USS initAlgorithms Result: ");
    uartA0_print_code(code);

    USS_configAppTimerPeriod(&gUssSWConfig, gUssSWConfig.systemConfig->measurementPeriod);
    return true;
}

/*--------------------------------
Funcion: USS_INIT_FOR_CURRENT_TEST
Parametros de entrada: N/A
Parametros de salida: N/A
Descripcion general: esta funcion realiza la misma funcion que USS_INIT solo que no 
imprime los resultados por UART ya que solo se quiere inicializar la medicion ultrasonica
para ejecutar una medicion cada segundo en la prueba de medicion de corriente 
--------------------------------*/
void USS_INIT_FOR_CURRENT_TEST(){
    USS_message_code code;
    USS_registerHSPLLInterruptCallback(USS_HSPLL_Interrupt_PLLUNLOCK,
                                        &handlePllUnlockEvent);

    code = USS_configureUltrasonicMeasurement(&gUssSWConfig);
    code = USS_verifyHSPLLFrequency(&gUssSWConfig, &testResults);
    code = USS_initAlgorithms(&gUssSWConfig);
    USS_configAppTimerPeriod(&gUssSWConfig, gUssSWConfig.systemConfig->measurementPeriod);
    return true;
}

/*--------------------------------
Funcion: Uss_measure
Parametros de entrada: N/A
Parametros de salida: N/A
Descripcion general: esta funcion realiza una medicion ultrasonica y regresa el toda la 
informacion obtenida de la medicion enviandola por la UART
--------------------------------*/
void Uss_measure(){
    USS_message_code codeInit;
    USS_message_code codeRun;
    USS_Algorithms_Results algResults;

    codeInit = USS_startLowPowerUltrasonicCapture(&gUssSWConfig);
    uartA0_print("start LowPower Ultrasonic Capture Result: ");
    uartA0_print_code(codeInit);

    codeRun = USS_runAlgorithms(&gUssSWConfig, &algResults);
    uartA0_print("USS runAlgorithms Result: ");
    uartA0_print_code(codeRun);

    algResults.messageCode = codeRun;
    uartA0_print("deltaTOF: ");
    uartA0_print_decimal_code(algResults.deltaTOF);
    uartA0_print("totalTOF UPS: ");
    uartA0_print_decimal_code(algResults.totalTOF_UPS);
    uartA0_print("totalTOF DNS: ");
    uartA0_print_decimal_code(algResults.totalTOF_DNS);
    uartA0_print("volumeFlowRate: ");
    uartA0_print_decimal_code(algResults.volumeFlowRate);
    uartA0_print("volumeFlowRateNoCalib: ");
    uartA0_print_decimal_code(algResults.volumeFlowRateNoCalib);
    uartA0_print("temperature: ");
    uartA0_print_decimal_code(algResults.temperature);

}

/*--------------------------------
Funcion: Uss_measure_for_current_test
Parametros de entrada: N/A
Parametros de salida: N/A
Descripcion general: esta funcion realiza una medicion ultrasonica 
pero no imprime los resultados por uart ya que solo se quiere realizar la 
medicion para la prueba de consumo de corriente
--------------------------------*/
void Uss_measure_for_current_test(){
    USS_message_code codeInit;
    USS_message_code codeRun;
    USS_Algorithms_Results algResults;
    codeInit = USS_startLowPowerUltrasonicCapture(&gUssSWConfig);
    codeRun = USS_runAlgorithms(&gUssSWConfig, &algResults);
    
}