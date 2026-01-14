#ifndef TABLERO_H
#define TABLERO_H

#include "macros.h"
#include "pieza.h"

// Estructura del Tablero
typedef struct {
    // Matriz que guarda 1 si está ocupado, 0 si está libre
    uint8_t matriz[TABLERO_ALTO][TABLERO_ANCHO];
} Tablero_t;


// Limpia el tablero
void Tablero_Init(Tablero_t *me);

// Verifica si la pieza, en su posición actual, choca con bordes o bloques existentes
bool Tablero_VerificarColision(Tablero_t *me, Pieza_t *pieza);

// Copia los bloques de la pieza a la matriz del tablero
void Tablero_FijarPieza(Tablero_t *me, Pieza_t *pieza);

// Escanea el tablero, elimina líneas completas y baja las superiores.
// Retorna el número de líneas eliminadas
int Tablero_EliminarLineasCompletas(Tablero_t *me);

#endif
