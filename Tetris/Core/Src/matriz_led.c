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

// Define esto arriba en tu .c o en el .h
// Significa: "Cada 2400 puntos la barra se llena completa y se reinicia".
// Como hay 24 LEDs, cada 100 puntos se encenderá 1 LED nuevo.
#define PUNTOS_PARA_LLENAR 2400

void MatrizLED_Puntuacion(uint32_t puntuacion) {

    // 1. Mapeo de Puntuación a LEDs (0 a 24)
    // Usamos el operador % (módulo) para que la barra sea cíclica.
    // Fórmula: (PuntosActuales % Tope) * NumLeds / Tope
    uint8_t nivelCarga = (puntuacion % PUNTOS_PARA_LLENAR) * 24 / PUNTOS_PARA_LLENAR;

    // Si quieres que cuando la puntuación sea 0 la barra esté vacía, esto ya lo hace.
    // Si prefieres que la barra se quede llena al llegar al máximo en vez de reiniciar,
    // quita el operador % y pon un if (nivelCarga > 24) nivelCarga = 24;

    uint8_t filaBarra[3] = {0, 0, 0};

    // 2. Distribución de los LEDs en las 3 filas (Igual que antes)
    // Llenamos de abajo (i=2) hacia arriba (i=0)
    for (int i = 0; i < 3; i++) {
        int ledsAEncender = 0;

        if (i == 2) { // Fila de abajo (Registro 8)
            if (nivelCarga >= 8) ledsAEncender = 8;
            else ledsAEncender = nivelCarga;
        }
        else if (i == 1) { // Fila del medio (Registro 7)
            if (nivelCarga >= 16) ledsAEncender = 8;
            else if (nivelCarga > 8) ledsAEncender = nivelCarga - 8;
        }
        else if (i == 0) { // Fila de arriba (Registro 6)
            if (nivelCarga >= 24) ledsAEncender = 8;
            else if (nivelCarga > 16) ledsAEncender = nivelCarga - 16;
        }

        // Convertir número a bits (alineado a la izquierda)
        if (ledsAEncender > 0) {
            filaBarra[i] = (0xFF << (8 - ledsAEncender));
        }
    }

    // 3. Enviar a la Matriz 2
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




// Array aumentado a 36 para alojar las letras más grandes
// Cada letra ocupa 5 filas + 1 fila vacía de separación
static const uint8_t BITMAP_RAIN[36] = {
    // --- T ---
    0x3C, // ..****.. (Techo)
    0x18, // ...**... (Palo)
    0x18, // ...**...
    0x18, // ...**...
    0x18, // ...**... (Base)
    0x00, // (Espacio)

    // --- E ---
    0x3C, // ..****.. (Techo)
    0x20, // ..*..... (Pared)
    0x3C, // ..****.. (Medio)
    0x20, // ..*..... (Pared)
    0x3C, // ..****.. (Suelo)
    0x00, // (Espacio)

    // --- T ---
    0x3C, // ..****..
    0x18, // ...**...
    0x18, // ...**...
    0x18, // ...**...
    0x18, // ...**...
    0x00, // (Espacio)

    // --- R ---
    0x38, // ..***... (Curva arriba)
    0x24, // ..*..*.. (Hueco)
    0x38, // ..***... (Cierre)
    0x28, // ..*.*... (Pata bajando)
    0x24, // ..*..*.. (Pata final)
    0x00, // (Espacio)

    // --- I ---
    0x3C, // ..****.. (Serif arriba)
    0x18, // ...**...
    0x18, // ...**...
    0x18, // ...**...
    0x3C, // ..****.. (Serif abajo)
    0x00, // (Espacio)

    // --- S ---
    0x1C, // ...***.. (Curva arriba)
    0x20, // ..*.....
    0x18, // ...**... (Cruce)
    0x04, // .....*..
    0x38, // ..***... (Curva abajo)
    0x00  // (Espacio)
};
// Flecha apuntando a la derecha (>>) para la matriz de abajo
static const uint8_t BITMAP_FLECHA[8] = {
    0x00, 0x00, 0x11, 0x0A, 0x04, 0x11, 0x0A, 0x04
};

void MatrizLED_PantallaInicio(void) {
    // --- VARIABLES DE TIEMPO (Estáticas para no perder el valor) ---
    static uint32_t timerScroll = 0;
    static uint32_t timerBlink = 0;

    // --- VARIABLES DE ESTADO ---
    static int offsetRain = 0;     // Posición de la lluvia
    static uint8_t mostrarFlecha = 1; // Para el parpadeo

    uint32_t ahora = HAL_GetTick();

    // ------------------------------------------
    // PARTE 1: ACTUALIZACIÓN DE LÓGICA (TIMERS)
    // ------------------------------------------

    // A) Control del Scroll INVERTIDO (Sube en vez de bajar)
        if (ahora - timerScroll > 200) {
            offsetRain--; // <--- CAMBIO: Restamos para ir hacia atrás

            // Si baja de 0, damos la vuelta y lo ponemos al final (23)
            if (offsetRain < 0) offsetRain = 35;

            timerScroll = ahora;
        }

        // B) Control del Parpadeo de la Flecha (Se deja igual)
        if (ahora - timerBlink > 500) {
            mostrarFlecha = !mostrarFlecha;
            timerBlink = ahora;
        }

    // ------------------------------------------
    // PARTE 2: DIBUJADO EN MATRICES
    // ------------------------------------------

    // --- MATRICES 0 y 1: EFECTO LLUVIA TETRIS ---
    // Recorremos las 16 filas visibles de las dos pantallas superiores
    for (int i = 0; i < 16; i++) {
        // Calculamos qué fila del bitmap toca pintar.
        // Usamos modulo % 24 para que cuando el offset mueva la imagen,
        // lo que salga por abajo vuelva a entrar por arriba.
        int indiceBitmap = (i - offsetRain + 36) % 36;

        uint8_t data = BITMAP_RAIN[indiceBitmap];

        if (i < 8) {
            MAX7219_Send(0, i + 1, data);      // Matriz Superior
        } else {
            MAX7219_Send(1, (i - 8) + 1, data); // Matriz Media
        }
    }

    // --- MATRIZ 2: FLECHA PARPADEANTE ---
    for (int i = 0; i < 8; i++) {
        uint8_t data = 0x00;

        if (mostrarFlecha) {
            data = BITMAP_FLECHA[i];
        }

        // Enviamos a la tercera matriz (Filas 1-8 locales)
        MAX7219_Send(2, i + 1, data);
    }
}
