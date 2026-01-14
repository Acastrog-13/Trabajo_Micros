#include "tablero.h"
#include <string.h>


void Tablero_Init(Tablero_t *me) {
    for (int8_t i = 0; i < TABLERO_ALTO; i++) {
        for (int8_t j = 0; j < TABLERO_ANCHO; j++) {
            me->matriz[i][j] = 0;
        }
    }
}

bool Tablero_VerificarColision(Tablero_t *me, Pieza_t *pieza) {
    for (int8_t i = 0; i < 4; i++) {
        for (int8_t j = 0; j < 4; j++) {
            // Solo comprobamos si en la matriz 4x4 de la pieza hay un bloque
            if (Pieza_GetBloque(pieza, i, j)) {
                int8_t realX = pieza->x + j;
                int8_t realY = pieza->y + i;

                // Límites laterales
                if (realX < 0 || realX >= TABLERO_ANCHO) return true;

                // Límite inferior
                if (realY >= TABLERO_ALTO) return true;

                // Colisión con bloques fijos (solo si realY es positivo)
                if (realY >= 0 && me->matriz[realY][realX] != 0) {
                    return true;
                }
            }
        }
    }
    return false;
}

void Tablero_FijarPieza(Tablero_t *me, Pieza_t *pieza) {
    for (int8_t i = 0; i < 4; i++) {
        for (int8_t j = 0; j < 4; j++) {
            if (Pieza_GetBloque(pieza, i, j)) {
                int8_t realX = pieza->x + j;
                int8_t realY = pieza->y + i;

                if (realY >= 0 && realY < TABLERO_ALTO && realX >= 0 && realX < TABLERO_ANCHO) {
                    me->matriz[realY][realX] = 1;
                }
            }
        }
    }
}

int Tablero_EliminarLineasCompletas(Tablero_t *me) {
    int8_t lineasEliminadas = 0;

    for (int8_t i = TABLERO_ALTO - 1; i >= 0; i--) {
        bool filaLlena = true;
        for (int8_t j = 0; j < TABLERO_ANCHO; j++) {
            if (me->matriz[i][j] == 0) {
                filaLlena = false;
                break;
            }
        }

        if (filaLlena) {
            lineasEliminadas++;
            // Se bajan las filas superiores manualmente
            for (int8_t k = i; k > 0; k--) {
                for (int8_t col = 0; col < TABLERO_ANCHO; col++) {
                    me->matriz[k][col] = me->matriz[k-1][col];
                }
            }
            // Se limpia la fila de arriba del todo
            for (int8_t col = 0; col < TABLERO_ANCHO; col++) {
                me->matriz[0][col] = 0;
            }
            i++; // Volvemos a revisar esta fila por si la que bajó también está llena
        }
    }
    return lineasEliminadas;
}
