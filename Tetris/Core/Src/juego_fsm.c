#include "juego_fsm.h"
#include "input_driver.h"
#include "matriz_led.h"
#include <stdlib.h> // Para srand/rand si fuera necesario

 // Variable estática para contar el tiempo (ticks)
static volatile uint32_t s_tickCounter = 0;

void Juego_Init(Juego_t* me) {
    // 1. Inicializar submódulos
    Input_Init();          // Integrante 1
    Tablero_Init(&me->tablero); // Integrante 2
    MatrixLED_Init();      // Integrante 2

    // 2. Configurar estado inicial del juego
    me->estadoActual = ESTADO_INICIO;
    me->puntuacion = 0;
    me->velocidadCaida = 500; // Caída cada 500 ticks (ms)

    // Semilla aleatoria (idealmente leer ruido ADC, aquí simplificado para poder probar que funcione, pero siempre saldran las piezas en el mismo orden)
    srand(0);
}

void Juego_TimerTick(void) {
    // Se llama desde la interrupción del Timer (ej. cada 1ms)
    s_tickCounter++;
}

// Función auxiliar para revertir movimiento en caso de colisión
static void RevertirMovimiento(Pieza_t* p, Direccion_t dir) {
    if (dir == DERECHA) Pieza_Mover(p, IZQUIERDA);
    else if (dir == IZQUIERDA) Pieza_Mover(p, DERECHA);
    else if (dir == ABAJO) p->y -= 1; // Revertir caída //Sería mejor implementar el movimiento hacia arriba en Pieza_Mover Alex!!
}

void Juego_EjecutarMaquinaEstados(Juego_t* me) {

    switch (me->estadoActual) {

        // ---------------------------------------------------------
    case ESTADO_INICIO:
        // Esperamos pulsar botón de rotación para empezar
        MatrixLED_MostrarGameOver(); // Efecto visual de espera
        if (Input_BotonRotarPulsado()) {
            Input_ClearBotonRotar();
            Tablero_Init(&me->tablero); // Limpiar tablero visual
            me->puntuacion = 0;
            me->velocidadCaida = 500;
            me->estadoActual = ESTADO_GENERAR;
        }
        break;

        // ---------------------------------------------------------
    case ESTADO_GENERAR:
        // Crear nueva pieza arriba
        Pieza_Init(&me->piezaActual);

        // Verificar si nace en colisión (Game Over inmediato)
        if (Tablero_VerificarColision(&me->tablero, &me->piezaActual)) {
            me->estadoActual = ESTADO_GAMEOVER;
        }
        else {
            s_tickCounter = 0; // Reset timer gravedad
            me->estadoActual = ESTADO_BAJANDO;
        }
        break;

        // ---------------------------------------------------------
    case ESTADO_BAJANDO:
        // 1. Gestión de Rotación (Input)
        if (Input_BotonRotarPulsado()) {
            Input_ClearBotonRotar();
            Pieza_Rotar(&me->piezaActual);
            // Si al rotar choca, deshacemos la rotación (3 rotaciones más = posición original)
            if (Tablero_VerificarColision(&me->tablero, &me->piezaActual)) {
                Pieza_Rotar(&me->piezaActual);
                Pieza_Rotar(&me->piezaActual);
                Pieza_Rotar(&me->piezaActual);
            }
        }

        // 2. Gestión de Movimiento Lateral / Caída rápida (Input)
        Direccion_t dir = Input_LeerJoystick();
        if (dir == IZQUIERDA || dir == DERECHA) {
            Pieza_Mover(&me->piezaActual, dir);
            if (Tablero_VerificarColision(&me->tablero, &me->piezaActual)) {
                RevertirMovimiento(&me->piezaActual, dir);
            }
        }
        else if (dir == ABAJO) {
            // Caída rápida manual
            me->velocidadCaida = 50; // Acelerar temporalmente
        }
        else {
            // Restablecer velocidad normal si soltó el joystick
            me->velocidadCaida = 500;
        }

        // 3. Gravedad (Timer)
        if (s_tickCounter >= me->velocidadCaida) {
            s_tickCounter = 0;
            Pieza_Mover(&me->piezaActual, ABAJO);

            // Si choca al bajar, significa que tocó suelo/bloque
            if (Tablero_VerificarColision(&me->tablero, &me->piezaActual)) {
                RevertirMovimiento(&me->piezaActual, ABAJO); // Volver a subir 1
                me->estadoActual = ESTADO_FIJAR;
            }
        }

        // 4. Actualizar Display
        MatrixLED_Actualizar(&me->tablero, &me->piezaActual);
        break;

        // ---------------------------------------------------------
    case ESTADO_FIJAR:
        // Fijar pieza en la matriz del tablero
        Tablero_FijarPieza(&me->tablero, &me->piezaActual);

        // Verificar líneas completas
        int lineas = Tablero_EliminarLineasCompletas(&me->tablero);
        if (lineas > 0) {
            me->puntuacion += (lineas * 100); // Lógica simple de puntos
            // Opcional: Aumentar dificultad reduciendo me->velocidadCaida
        }

        me->estadoActual = ESTADO_GENERAR;
        break;

        // ---------------------------------------------------------
    case ESTADO_GAMEOVER:
        MatrixLED_MostrarGameOver();
        // Reiniciar con botón
        if (Input_BotonRotarPulsado()) {
            Input_ClearBotonRotar();
            me->estadoActual = ESTADO_INICIO;
        }
        break;
    }
}
