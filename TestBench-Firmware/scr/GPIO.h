#ifndef GPIO_H_
#define GPIO_H_

#include <msp430.h>

//estados de los pines
typedef enum {
    LOW = 0,
    HIGH = 1
} gpio_level_t;

//numeros de los puertos 
typedef enum {
    P0 = 0,
    P1 = 1,
    P2 = 2,
    P3 = 3,
    P4 = 4,
    P5 = 5,
    P6 = 6,
    P7 = 7,
    P8 = 8,
    P9 = 9
}port_var;

//funciones
extern void GPIO_INPUT_EN(port_var port, unsigned int bit);
extern void GPIO_PULL_DOWN_RES(port_var port, unsigned int bit);
extern void GPIO_OUTPUT_EN(port_var port, unsigned int bit);
extern void GPIO_WRITE(port_var port, unsigned int bit, unsigned int level);
extern unsigned int GPIO_READ(port_var port, unsigned int bit);

#endif /* GPIO_H_ */