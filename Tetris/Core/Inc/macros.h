#ifndef MACROS_H
#define MACROS_H
#pragma once

#include <stdint.h>
#include <stdbool.h>

#define TABLERO_ALTO 20      // Tablero lógico (memoria)
#define TABLERO_ANCHO 8      // Ancho estándar
#define VISIBLE_ALTO 16      // Lo que realmente muestra la matriz LED
#define SPAWN_ZONA (TABLERO_ALTO - VISIBLE_ALTO) // Las 4 filas de arriba

// Definición de direcciones para el Joystick y movimientos
typedef enum {
    NINGUNA = 0,
    ARRIBA,
    ABAJO,
    IZQUIERDA,
    DERECHA,
	ROTAR
} Direccion_t;

#endif // MACROS_H
