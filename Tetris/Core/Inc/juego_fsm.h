#ifndef JUEGO_FSM_H
#define JUEGO_FSM_H

#include "macros.h"
#include "pieza.h"
#include "tablero.h"
#include "control.h"
#include "matriz_led.h"
#include "stm32f4xx_hal.h"

// Estados de la Máquina de Estados Finita
typedef enum {
    ESTADO_INICIO,      // Esperando botón de start
    ESTADO_GENERAR,     // Crear nueva pieza
    ESTADO_BAJANDO,     // Pieza cayendo
    ESTADO_FIJAR,       // Pieza tocó suelo, se fija y se buscan líneas
    ESTADO_GAMEOVER     // Juego terminado
} EstadoJuego_t;

// Estructura del Juego
typedef struct {
    EstadoJuego_t estadoActual;
    Control_t control;
    Tablero_t tablero;
    Pieza_t piezaActual;
    Pieza_t piezaSiguiente;
    uint32_t puntuacion;
    uint32_t velocidadCaida;
} Juego_t;


// Configura Timers y llama a los Init de los otros módulos
void Juego_Init(Juego_t *me);

// Función maquina de estados
void Juego_EjecutarMaquinaEstados(Juego_t *me, ADC_HandleTypeDef* hadc,
		uint8_t* flag_caida, uint8_t* flag_rotar, uint8_t* flag_adc, uint8_t* flag_timer, uint16_t lecturasJoystick[2]);

#endif
