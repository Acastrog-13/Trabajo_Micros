#ifndef MATRIZ_LED_H
#define MATRIZ_LED_H

#include "macros.h"
#include "tablero.h"
#include "pieza.h"

// Configura GPIOs y SPI (si se usa) para la matriz
void MatrixLED_Init(void);

// Función principal de dibujo.
// Recibe el tablero (fondo) y la pieza actual (movimiento) para combinarlos y dibujarlos.
void MatrixLED_Actualizar(Tablero_t *tablero, Pieza_t *piezaActual);

// (Opcional) Efecto visual para Game Over
void MatrixLED_MostrarGameOver(void);

#endif // MATRIZ_LED_H
