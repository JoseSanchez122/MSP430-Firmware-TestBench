#ifndef UART_H_
#define UART_H_

#include "sys/cdefs.h"
#include <Ultrasonic_Sensor_Test.h>
#include <stdbool.h>

extern volatile int UART3_MSG_RECEIVED;
extern volatile int UART0_MSG_RECEIVED;
extern volatile unsigned int INDEX_UART3; 
extern volatile unsigned int INDEX_UART0; 
extern volatile unsigned char UART3BUFF[512];
extern volatile unsigned char UART0BUFF[512];

extern void UART3_INIT();
extern void UART0_INIT();
extern void UART0_INIT_9600_baud();
extern void uartA3_print(const char *cadena);
extern void uartA0_print(const char *cadena);
extern void uartA0_print_code(uint32_t code);
extern void uartA0_print_decimal_code(float code);
extern void uartA0_print_1DIG(char value);
extern unsigned int DMA3_Transfer(unsigned char *Buf);
extern void RESET_UART3_BUFF();
extern char READ_UART3_BUFF(const char *expected_result, int size);
extern bool READ_UART0_BUFF(const char *expected_result);
extern void CLEAR_UART0BUFF(void);

#endif /* UART_H_ */