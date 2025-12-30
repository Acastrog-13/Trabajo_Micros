#ifndef PIEZA_H
#define PIEZA_H

#include "macros.h"

#define MAX_PIEZAS 7  //número máximo de piezas
//Dimensiones matrizque contiene a la pieza
#define PIEZA_ALTO 4
#define PIEZA_ANCHO 4

//Estructura de la pieza
typedef struct {
    int8_t x;              				// Posición X en el tablero
    int8_t y;              				    // Posición Y en el tablero
    uint8_t tipo;           				// Identificador de la forma
    uint8_t rotacion;       				// Estado de rotación
    uint8_t forma[PIEZA_ALTO][PIEZA_ANCHO]; //contiene la rotación actual de las piezas
} Pieza_t;


//Atualiza la forma de la pieza al rotar
void setForma(Pieza_t* me);

// Inicializa una pieza de tipo aleatorio en una nueva en la parte superior con un tipo aleatorio
void Pieza_Init(Pieza_t *me);

// Cambia la matriz interna 'forma' según la rotación siguiente
void Pieza_Rotar(Pieza_t *me);

// Mueve las coordenadas de la pieza
void Pieza_Mover(Pieza_t *me, Direccion_t dir);

// Retorna true si en esa coordenada relativa de la pieza (i, j) hay un bloque
bool Pieza_GetBloque(Pieza_t *me, uint8_t i, uint8_t j);

#endif
