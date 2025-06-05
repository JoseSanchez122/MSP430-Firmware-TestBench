#ifndef VOL_CURR_TEST_H_
#define VOL_CURR_TEST_H_

#include <GPIO.h>
#include <msp430.h>
#include <Quectel.h>
#include <UART.h>
#include <stdbool.h>
#include <Timers.h>
#define Times_to_wake_up 10

extern void Voltage_Test();
extern void Current_Test();
extern void GO_TO_SLEEP_FOR(int _1s_per_tick);
extern volatile int times_waken_up;

#endif /* VOL_CURR_TEST_H_ */