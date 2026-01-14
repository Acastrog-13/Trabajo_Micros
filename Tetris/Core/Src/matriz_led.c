#include "matriz_led.h"
#include "main.h"

void MatrizLED_Ini(void) {
    // Inicializamos TODOS los módulos
    for (uint8_t i = 0; i < NUM_MATRIZ; i++) {
        MAX7219_Send(i, MAX7219_REG_SHUTDOWN, 0x01);
        MAX7219_Send(i, MAX7219_REG_DISPLAYTEST, 0x00);
        MAX7219_Send(i, MAX7219_REG_DECODEMODE, 0x00);
        MAX7219_Send(i, MAX7219_REG_SCANLIMIT, 0x07);
        MAX7219_Send(i, MAX7219_REG_INTENSITY, 0x02); // Brillo suave

        // Limpiar pantalla
        for(uint8_t row=1; row<=8; row++) {
            MAX7219_Send(i, row, 0x00);
        }
    }
}

void MatrizLED_Actualizar(Tablero_t *tablero, Pieza_t *piezaActual) {
    
    for (uint8_t f = 0; f < 16; f++) {
        uint8_t filaBits = 0;
      
        for (uint8_t c = 0; c < TABLERO_ANCHO; c++) {
            bool encendido = false;

            // Bloque del tablero
            if (tablero->matriz[f][c] != 0)
                encendido = true;

            // Posición pieza
            uint8_t relX = c - piezaActual->x;
            uint8_t relY = f - piezaActual->y;

            // Se verifica que la coordenada esté dentro del tablero
            if (relX >= 0 && relX < 4 && relY >= 0 && relY < 4)
                if (Pieza_GetBloque(piezaActual, relY, relX)) 
                    encendido = true;

            // Se enciende el led
            if (encendido) filaBits |= (0x80>>c);
        }

       //Envío de información
       if (f < 8) MAX7219_Send(0, f + 1, filaBits);
       else MAX7219_Send(1, (f - 8) + 1, filaBits);

    }
}

void MatrizLED_Limpiar(int target_module){
	for(int i=0; i<8;i++){
		MAX7219_Send(target_module, i+1, 0x00);
	}
}

void MatrizLED_MostrarSiguiente(Pieza_t *piezaSiguiente) {
    int offsetX = piezaSiguiente->x;

    for (int f = 0; f < 4; f++) {
        uint8_t filaBits = 0;
        int filaPieza = f;

		for (int c = 0; c < 4; c++)
			if (Pieza_GetBloque(piezaSiguiente, filaPieza, c))
				filaBits |= (0x80 >> (c + offsetX));

        MAX7219_Send(2, f + 1, filaBits);
    }
}

#define PUNTOS_PARA_LLENAR 2400

void MatrizLED_Puntuacion(uint32_t puntuacion) {
    uint8_t nivelCarga = (puntuacion % PUNTOS_PARA_LLENAR) * 24 / PUNTOS_PARA_LLENAR;
    uint8_t filaBarra[3] = {0, 0, 0};

    for (int i = 0; i < 3; i++) {
        int ledsAEncender = 0;

        if (i == 2) {
            if (nivelCarga >= 8) ledsAEncender = 8;
            else ledsAEncender = nivelCarga;
        }
        else if (i == 1) {
            if (nivelCarga >= 16) ledsAEncender = 8;
            else if (nivelCarga > 8) ledsAEncender = nivelCarga - 8;
        }
        else if (i == 0) {
            if (nivelCarga >= 24) ledsAEncender = 8;
            else if (nivelCarga > 16) ledsAEncender = nivelCarga - 16;
        }

        // Convertir número a bits (alineado a la izquierda)
        if (ledsAEncender > 0) {
            filaBarra[i] = (0xFF << (8 - ledsAEncender));
        }
    }

    // Enviar a la Matriz 2
    MAX7219_Send(2, 6, filaBarra[0]); // Arriba
    MAX7219_Send(2, 7, filaBarra[1]); // Medio
    MAX7219_Send(2, 8, filaBarra[2]); // Abajo
}

void MatrizLED_MostrarGameOver(void) {
    // Efecto Llenado
    for (uint8_t f = 0; f < 16; f++) {

        if (f < 8) MAX7219_Send(0, f + 1, 0xFF);
        else MAX7219_Send(1, (f - 8) + 1, 0xFF);

        HAL_Delay(50);
    }

    HAL_Delay(500);

    // Efecto Vaciado
    for (uint8_t f = 0; f < 16; f++) {

        if (f < 8) MAX7219_Send(0, f + 1, 0x00);
        else MAX7219_Send(1, (f - 8) + 1, 0x00);

        HAL_Delay(30);
    }
}

static const uint8_t BITMAP_RAIN[36] = {
    // --- T ---
    0x3C, // ..****..
    0x18, // ...**...
    0x18, // ...**...
    0x18, // ...**...
    0x00, //

    // --- E ---
    0x3C, // ..****..
    0x20, // ..*.....
    0x3C, // ..****..
    0x20, // ..*.....
    0x3C, // ..****..
    0x00, //

    // --- T ---
    0x3C, // ..****..
    0x18, // ...**...
    0x18, // ...**...
    0x18, // ...**...
    0x18, // ...**...
    0x00, //
    // --- R ---
    0x38, // ..***...
    0x24, // ..*..*..
    0x38, // ..***...
    0x28, // ..*.*...
    0x24, // ..*..*..
    0x00, //

    // --- I ---
    0x3C, // ..****..
    0x18, // ...**...
    0x18, // ...**...
    0x18, // ...**...
    0x3C, // ..****..
    0x00, //

    // --- S ---
    0x1C, // ...***..
    0x20, // ..*.....
    0x18, // ...**...
    0x04, // .....*..
    0x38, // ..***...
    0x00  //
};

// Flecha apuntando a la derecha (>>) para la matriz de abajo
static const uint8_t BITMAP_FLECHA[8] = {
    0x00, 0x00, 0x11, 0x0A, 0x04, 0x11, 0x0A, 0x04
};

void MatrizLED_PantallaInicio(void) {
    static uint32_t timerScroll = 0;
    static uint32_t timerBlink = 0;

    static int offsetRain = 0;
    static uint8_t mostrarFlecha = 1;

    uint32_t ahora = HAL_GetTick();


        if (ahora - timerScroll > 200) {
            offsetRain--;

            if (offsetRain < 0) offsetRain = 35;

            timerScroll = ahora;
        }


        if (ahora - timerBlink > 500) {
            mostrarFlecha = !mostrarFlecha;
            timerBlink = ahora;
        }

    for (int i = 0; i < 16; i++) {

        int indiceBitmap = (i - offsetRain + 36) % 36;

        uint8_t data = BITMAP_RAIN[indiceBitmap];

        if (i < 8) {
            MAX7219_Send(0, i + 1, data);      // Matriz Superior
        } else {
            MAX7219_Send(1, (i - 8) + 1, data); // Matriz Media
        }
    }

    for (int i = 0; i < 8; i++) {
        uint8_t data = 0x00;

        if (mostrarFlecha) {
            data = BITMAP_FLECHA[i];
        }

        // Enviamos a la tercera matriz (Filas 1-8 locales)
        MAX7219_Send(2, i + 1, data);
    }
}
