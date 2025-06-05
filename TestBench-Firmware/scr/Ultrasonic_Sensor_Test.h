#ifndef ULTRASONIC_SENSOR_TEST_H_
#define ULTRASONIC_SENSOR_TEST_H_

#include "ussSwLib.h"
#include "USS_Config/USS_userConfig.h"
#include <stdio.h>
#include <UART.h>
#include "IQmathLib.h"
#include <stdbool.h>

USS_calibration_hspll_results testResults;
extern bool USS_INIT();
extern void USS_INIT_FOR_CURRENT_TEST();
extern void Uss_measure();
extern void Uss_measure_for_current_test();

#endif /*ULTRASONIC_SENSOR_TEST_H_*/