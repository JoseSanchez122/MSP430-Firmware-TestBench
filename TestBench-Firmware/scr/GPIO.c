#include <GPIO.h>

/*--------------------------------
Macro: gpio_input_en
Parametros de entrada: puerto y pin 
Parametros de salida: N/A
Descripcion general: esta Macro configura un pin de un 
puerto como entrada
--------------------------------*/
#define gpio_input_en(port, bit) do {\
            port##DIR &= ~(bit); } while(0);

/*--------------------------------
Macro: gpio_pull_down_res
Parametros de entrada: puerto y pin 
Parametros de salida: N/A
Descripcion general: esta Macro habilita las resistencias
de pulldown del pin deseado
--------------------------------*/
#define gpio_pull_down_res(port, bit) do {\
            port##REN |= (bit); \
            port##OUT &= ~(bit); } while(0);

/*--------------------------------
Macro: gpio_output_en
Parametros de entrada: puerto y pin 
Parametros de salida: N/A
Descripcion general: esta Macro habilita el pin como salida 
--------------------------------*/
#define gpio_output_en(port, bit) do {\
            port##DIR |= (bit);} while(0);

/*--------------------------------
Macro: gpio_write
Parametros de entrada: puerto, pin, y estado 
Parametros de salida: N/A
Descripcion general: esta Macro pone un pin previamente 
habilitado como salida en alto o en bajo dependiendo si se 
recive HIGH o LOW 
--------------------------------*/
#define gpio_write(port, bit, level)  do { \
            if (level) port##OUT |= (bit); \
            else port##OUT &= ~(bit); \
        } while(0);

#define gpio_read(port, bit) ((port##IN) & (bit))

/*--------------------------------
Funcion: GPIO_INPUT_EN
Parametros de entrada: puerto, pin
Parametros de salida: N/A
Descripcion general: esta funcion usa la macro gpio_input_en para habilitar un pin 
como entrada dependiendo del puerto
--------------------------------*/
void GPIO_INPUT_EN(port_var port, unsigned int bit) {
    switch (port) {
        case P1: gpio_input_en(P1, bit); break;
        case P2: gpio_input_en(P2, bit); break;
        case P3: gpio_input_en(P3, bit); break;
        case P4: gpio_input_en(P4, bit); break;
        case P5: gpio_input_en(P5, bit); break;
        case P6: gpio_input_en(P6, bit); break;
        case P7: gpio_input_en(P7, bit); break;
        case P8: gpio_input_en(P8, bit); break;
        case P9: gpio_input_en(P9, bit); break;
    }
}

/*--------------------------------
Funcion: GPIO_PULL_DOWN_RES
Parametros de entrada: puerto, pin
Parametros de salida: N/A
Descripcion general: esta funcion usa la macro gpio_pull_down_res para habilitar la 
resistencia de pulldown dependiendo del puerto
--------------------------------*/
void GPIO_PULL_DOWN_RES(port_var port, unsigned int bit) {
    switch (port) {
        case P1: gpio_pull_down_res(P1, bit); break;
        case P2: gpio_pull_down_res(P2, bit); break;
        case P3: gpio_pull_down_res(P3, bit); break;
        case P4: gpio_pull_down_res(P4, bit); break;
        case P5: gpio_pull_down_res(P5, bit); break;
        case P6: gpio_pull_down_res(P6, bit); break;
        case P7: gpio_pull_down_res(P7, bit); break;
        case P8: gpio_pull_down_res(P8, bit); break;
        case P9: gpio_pull_down_res(P9, bit); break;
    }
}

/*--------------------------------
Funcion: GPIO_OUTPUT_EN
Parametros de entrada: Puerto y pin
Parametros de salida: N/A
Descripcion general: esta funcion usa la macro gpio_output_en para habilitar un pin 
como salida dependiendo del puerto
--------------------------------*/
void GPIO_OUTPUT_EN(port_var port, unsigned int bit) {
    switch (port) {
        case P1: gpio_output_en(P1, bit); break;
        case P2: gpio_output_en(P2, bit); break;
        case P3: gpio_output_en(P3, bit); break;
        case P4: gpio_output_en(P4, bit); break;
        case P5: gpio_output_en(P5, bit); break;
        case P6: gpio_output_en(P6, bit); break;
        case P7: gpio_output_en(P7, bit); break;
        case P8: gpio_output_en(P8, bit); break;
        case P9: gpio_output_en(P9, bit); break;
    }
}

/*--------------------------------
Funcion: GPIO_WRITE
Parametros de entrada: Puerto, pin y estado
Parametros de salida: N/A
Descripcion general: esta funcion usa la macro gpio_write para cambiar el estado
de un pin de alto a bajo dependiendo del puerto
--------------------------------*/
void GPIO_WRITE(port_var port, unsigned int bit, unsigned int level) {
    switch (port) {
        case P1: gpio_write(P1, bit, level); break;
        case P2: gpio_write(P2, bit, level); break;
        case P3: gpio_write(P3, bit, level); break;
        case P4: gpio_write(P4, bit, level); break;
        case P5: gpio_write(P5, bit, level); break;
        case P6: gpio_write(P6, bit, level); break;
        case P7: gpio_write(P7, bit, level); break;
        case P8: gpio_write(P8, bit, level); break;
        case P9: gpio_write(P9, bit, level); break;
    }
}

/*--------------------------------
Funcion: GPIO_READ
Parametros de entrada: N/A
Parametros de salida: estado del pin
Descripcion general: esta funcion usa la macro gpio_read 
para leer el puerto y regresar su estado
--------------------------------*/
unsigned int GPIO_READ(port_var port, unsigned int bit) {
    switch (port) {
        case P1: return gpio_read(P1, bit);
        case P2: return gpio_read(P2, bit);
        case P3: return gpio_read(P3, bit);
        case P4: return gpio_read(P4, bit);
        case P5: return gpio_read(P5, bit);
        case P6: return gpio_read(P6, bit);
        case P7: return gpio_read(P7, bit);
        case P8: return gpio_read(P8, bit);
        case P9: return gpio_read(P9, bit);
        default: return 0;
    }
}