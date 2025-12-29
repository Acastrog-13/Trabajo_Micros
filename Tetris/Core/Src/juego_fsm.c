#include "juego_fsm.h"


void Juego_Init(Juego_t* me) {

    Tablero_Init(&me->tablero);
    MatrizLED_Init();

    // Se configura estado inicial del juego
    me->estadoActual = ESTADO_INICIO;
    me->puntuacion = 0;
    me->velocidadCaida = 500;

}

static void RevertirMovimiento(Pieza_t* p, Direccion_t dir) {
    if (dir == DERECHA) 		Pieza_Mover(p, IZQUIERDA);
    else if (dir == IZQUIERDA) 	Pieza_Mover(p, DERECHA);
    else if (dir == ABAJO) 		Pieza_Mover(p, ARRIBA);
}

void Juego_EjecutarMaquinaEstados(Juego_t *me, ADC_HandleTypeDef* hadc,
		uint8_t* flag_caida, uint8_t* flag_rotar, uint8_t* flag_adc, uint8_t* flag_timer, uint16_t joystick_val) {

    switch (me->estadoActual) {

    case ESTADO_INICIO:
        MatrizLED_MostrarGameOver();
        if(*flag_rotar) {
			*flag_rotar = 0;
			me->estadoActual = ESTADO_GENERAR;
		}
        break;

    case ESTADO_GENERAR:
        // Se crea pieza arriba
        Pieza_Init(&me->piezaActual);

        if (Tablero_VerificarColision(&me->tablero, &me->piezaActual))
            me->estadoActual = ESTADO_GAMEOVER;
        else {
        	MatrizLED_Actualizar(&me->tablero, &me->piezaActual);
			Control_Ini(&me->control, hadc); //Se inicializa la interrupción del Joystick
			me->estadoActual = ESTADO_BAJANDO;
        }
        break;

    case ESTADO_BAJANDO:
    	Direccion_t dir = Control_updateDir(&me->control, flag_caida, flag_adc, flag_rotar, joystick_val);
		static uint8_t orden=0;

		if (dir==NINGUNA) orden=0;

		//Movimientos laterales
		if (orden==0 && (dir == IZQUIERDA || dir == DERECHA)) {
			orden =1;
			Pieza_Mover(&me->piezaActual, dir);
			if (Tablero_VerificarColision(&me->tablero, &me->piezaActual))
				RevertirMovimiento(&me->piezaActual, dir);
		}

		//Rotación
		if (dir==ROTAR){
			Pieza_Rotar(&me->piezaActual);
			if (Tablero_VerificarColision(&me->tablero, &me->piezaActual)) {
				// Revertir rotación
				Pieza_Rotar(&me->piezaActual);
				Pieza_Rotar(&me->piezaActual);
				Pieza_Rotar(&me->piezaActual);
			}
		}

		//Caída
		if (*flag_timer || dir == ABAJO) {
			*flag_timer = 0;

			Pieza_Mover(&me->piezaActual, ABAJO);

			if (Tablero_VerificarColision(&me->tablero, &me->piezaActual)) {
				RevertirMovimiento(&me->piezaActual, ABAJO);
				me->estadoActual = ESTADO_FIJAR;
			}
		}

		MatrizLED_Actualizar(&me->tablero, &me->piezaActual);
		break;

    case ESTADO_FIJAR:
    	Tablero_FijarPieza(&me->tablero, &me->piezaActual);
		uint16_t lineas = Tablero_EliminarLineasCompletas(&me->tablero);
		me->puntuacion += (lineas * 100);
		MatrizLED_Actualizar(&me->tablero, &me->piezaActual);
		me->estadoActual = ESTADO_GENERAR;
		break;

    case ESTADO_GAMEOVER:
    	MatrizLED_MostrarGameOver();
    	Juego_Init(me);
        break;
    }
}
