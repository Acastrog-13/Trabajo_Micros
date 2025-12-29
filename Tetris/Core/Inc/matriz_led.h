#ifndef MATRIZ_LED_H
#define MATRIZ_LED_H

#include "macros.h"
#include "tablero.h"
#include "pieza.h"

#define NUM_MATRIZ 2

// Direcciones
#define MAX7219_REG_NOOP        0x00
#define MAX7219_REG_DIGIT0      0x01
#define MAX7219_REG_DECODEMODE  0x09
#define MAX7219_REG_INTENSITY   0x0A
#define MAX7219_REG_SCANLIMIT   0x0B
#define MAX7219_REG_SHUTDOWN    0x0C
#define MAX7219_REG_DISPLAYTEST 0x0F

extern void MAX7219_Send(int target_module, uint8_t reg, uint8_t data);


// Configura GPIOs y SPI (si se usa) para la matriz
void MatrizLED_Ini(void);

// Recibe el tablero y la pieza actual para combinarlos y dibujarlos
void MatriszED_Actualizar(Tablero_t *tablero, Pieza_t *piezaActual);

// Efecto visual para Game Over
void MatrixLED_MostrarGameOver(void);

#endif // MATRIZ_LED_H
