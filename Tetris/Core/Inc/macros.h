#ifndef MACROS_H
#define MACROS_H
#pragma once

#include <stdint.h>
#include <stdbool.h>

#define TABLERO_ALTO 16
#define TABLERO_ANCHO 8

// Definición de direcciones para el Joystick y movimientos
typedef enum {
    NINGUNA = 0,
    ARRIBA,
    ABAJO,
    IZQUIERDA,
    DERECHA,
	ROTAR
} Direccion_t;

#endif
