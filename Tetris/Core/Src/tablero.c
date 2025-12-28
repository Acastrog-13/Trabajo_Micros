#include "tablero.h"
#include <string.h>

// El tablero real es más alto que la pantalla para el "spawn" fluido
// Definido en macros.h: TABLERO_ALTO (ej. 20) vs VISIBLE_ALTO (16)

void Tablero_Init(Tablero_t *me) {
    // Limpia todo el tablero, incluyendo la zona oculta
    for (int i = 0; i < TABLERO_ALTO; i++) {
        for (int j = 0; j < TABLERO_ANCHO; j++) {
            me->matriz[i][j] = 0;
        }
    }
}

bool Tablero_VerificarColision(Tablero_t *me, Pieza_t *pieza) {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            // Solo comprobamos si en la matriz 4x4 de la pieza hay un bloque
            if (Pieza_GetBloque(pieza, i, j)) {
                int realX = pieza->x + j;
                int realY = pieza->y + i;

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
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (Pieza_GetBloque(pieza, i, j)) {
                int realX = pieza->x + j;
                int realY = pieza->y + i;

                // Se "quema" la pieza en la matriz del tablero
                if (realY >= 0 && realY < TABLERO_ALTO && realX >= 0 && realX < TABLERO_ANCHO) {
                    me->matriz[realY][realX] = 1;
                }
            }
        }
    }
}

int Tablero_EliminarLineasCompletas(Tablero_t *me) {
    int lineasEliminadas = 0;

    for (int i = TABLERO_ALTO - 1; i >= 0; i--) {
        bool filaLlena = true;
        for (int j = 0; j < TABLERO_ANCHO; j++) {
            if (me->matriz[i][j] == 0) {
                filaLlena = false;
                break;
            }
        }

        if (filaLlena) {
            lineasEliminadas++;
            // Bajamos las filas superiores manualmente
            for (int k = i; k > 0; k--) {
                for (int col = 0; col < TABLERO_ANCHO; col++) {
                    me->matriz[k][col] = me->matriz[k-1][col];
                }
            }
            // Limpiamos la fila de arriba del todo
            for (int col = 0; col < TABLERO_ANCHO; col++) {
                me->matriz[0][col] = 0;
            }
            i++; // Volvemos a revisar esta fila por si la que bajó también está llena
        }
    }
    return lineasEliminadas;
}
