#ifndef JUEGO_FSM_H
#define JUEGO_FSM_H

#include "macros.h"
#include "pieza.h"
#include "tablero.h"

// Estados posibles de la Máquina de Estados Finita
typedef enum {
    ESTADO_INICIO,      // Esperando botón de start
    ESTADO_GENERAR,     // Crear nueva pieza
    ESTADO_BAJANDO,     // Pieza cayendo (usuario puede mover)
    ESTADO_FIJAR,       // Pieza tocó suelo, se fija y se buscan líneas
    ESTADO_GAMEOVER     // Juego terminado
} EstadoJuego_t;

// Estructura principal del Juego (Contexto)
typedef struct {
    EstadoJuego_t estadoActual;
    Tablero_t tablero;      // Instancia del tablero
    Pieza_t piezaActual;    // Instancia de la pieza activa
    uint32_t puntuacion;
    uint32_t velocidadCaida; // Tiempo en ms para caída automática
} Juego_t;

// --- MÉTODOS PÚBLICOS ---

// Configura Timers y llama a los Init de los otros módulos
void Juego_Init(Juego_t *me);

// Función principal del bucle (se llama en el while(1) del main)
// Aquí reside el switch(me->estadoActual)
void Juego_EjecutarMaquinaEstados(Juego_t *me);

// Esta función se llama desde la Interrupción del Timer (cada X ms)
// Activa una bandera para forzar la caída de la pieza por gravedad
void Juego_TimerTick(void);

#endif // JUEGO_FSM_H
