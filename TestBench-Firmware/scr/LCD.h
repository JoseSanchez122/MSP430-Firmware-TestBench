/*
  ------------------------------------------------------------------------|
  |  Archivo: LCD.h                                                       |
  |  Descripcion: Definiciones para uso de LCD                            |
  |    Autor: IMTECH DESARROLLOS (INGENIERIA DE FIRMWARE)                 |
  |-----------------------------------------------------------------------|
*/
#ifndef LCD_H_
#define LCD_H_
#include <msp430.h>
#include <stdint.h>

//Digitos Grandes
#define A9  LCDM21  //Digit 9
#define A8  LCDM22  //Digit 8
#define A7  LCDM23  //Digit 7
#define A6  LCDM24  //Digit 6
#define A5  LCDM25  //Digit 5
#define A4  LCDM26  //Digit 4
#define A3  LCDM27  //Digit 3
#define A2  LCDM28  //Digit 2
#define A1  LCDM29  //Digit 1

//Flujo
#define B1  LCDM1   // Digit 1
#define B2  LCDM2   // Digit 2
#define B3  LCDM3   // Digit 3
#define B4  LCDM4   // Digit 4
//Tempertura
#define C1  LCDM12  // Digit 0
#define C2  LCDM13  // Digit 0
#define C3  LCDM14  // Digit 0
//Hora
#define D1  LCDM8   // Hora 0
#define D2  LCDM9   // Hora 1
#define D3  LCDM10  // Min 0
#define D4  LCDM11  // Min 1
//Fecha
#define E1  LCDM15  // Dia 0
#define E2  LCDM16  // Dia 1
#define E3  LCDM17  // Mes 0
#define E4  LCDM18  // Mes 1
#define E5  LCDM19  // A�o 0
#define E6  LCDM20  // A�o 1

//Precion
#define F1  LCDM20  // Digit 1
#define F2  LCDM21  // Digit 2
#define F3  LCDM22  // Digit 3


extern void LCDTesting(int retardo);
extern int get_delay_ms(int Digits);
extern int get_delay_ms(int Digits);
extern void LCD_OFF();
extern void LCD_pins_test();

#endif /* LCD_H_ */
