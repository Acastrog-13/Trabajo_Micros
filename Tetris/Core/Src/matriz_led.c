#define REG_SCAN_LIMIT  0x0B
#define REG_SHUTDOWN    0x0C
#define REG_DISPLAY_TEST 0x0F

// Suponemos que el Integrante 3 define estas constantes en macros.h
// VISIBLE_ALTO = 16 (2 matrices de 8x8)
// TABLERO_ALTO = 20 (Incluye zona de spawn oculta)

// Función que debes implementar con tu HAL de SPI (depende de tus pines)
// Debe poner CS en bajo, enviar 2 bytes, y poner CS en alto.
extern void MAX7219_Enviar(uint8_t registro, uint8_t dato);

void MatrixLED_Init(void) {
    // Inicializamos las 2 matrices que forman la pantalla de 16x8
    // Se envían los comandos dos veces (una para cada chip en cascada)

    // 1. Modo Shutdown: Normal (Encendido)
    MAX7219_Enviar(REG_SHUTDOWN, 0x01);

    // 2. Modo Decodificador: Sin decodificación (matriz de puntos)
    MAX7219_Enviar(REG_DECODE_MODE, 0x00);

    // 3. Intensidad: Nivel medio (0x00 a 0x0F)
    MAX7219_Enviar(REG_INTENSITY, 0x05);

    // 4. Límite de escaneo: Todas las filas (0-7)
    MAX7219_Enviar(REG_SCAN_LIMIT, 0x07);

    // 5. Test: Apagado
    MAX7219_Enviar(REG_DISPLAY_TEST, 0x00);

    // Limpiar pantalla inicial
    for(int i = 1; i <= 8; i++) {
        MAX7219_Enviar(i, 0x00);
    }
}

void MatrixLED_Actualizar(Tablero_t *tablero, Pieza_t *piezaActual) {
    // Procesamos las 16 filas que se ven físicamente
    for (int f = 0; f < 16; f++) {
        uint8_t filaBits = 0;

        // Calculamos la fila real en el tablero lógico (0-19)
        // Si f=0 (arriba de la pantalla), leemos la fila 4 del tablero (zona spawn)
        int filaLogica = f + (TABLERO_ALTO - 16);

        for (int c = 0; c < TABLERO_ANCHO; c++) {
            bool encendido = false;

            // Capa 1: Bloques fijos en el tablero
            if (tablero->matriz[filaLogica][c] == 1) {
                encendido = true;
            }

            // Capa 2: Pieza que está cayendo (bloques móviles)
            // Calculamos posición relativa de la columna/fila actual respecto a la pieza
            int relX = c - piezaActual->x;
            int relY = filaLogica - piezaActual->y;

            // Si la coordenada (c, filaLogica) está dentro del cuadrado 4x4 de la pieza
            if (relX >= 0 && relX < 4 && relY >= 0 && relY < 4) {
                if (Pieza_GetBloque(piezaActual, relY, relX)) {
                    encendido = true;
                }
            }

            // Si hay un bloque en cualquiera de las dos capas, prendemos el bit
            if (encendido) {
                filaBits |= (1 << (7 - c)); // El bit 7 es la columna 0 (izquierda)
            }
        }

        // Envío al hardware:
        // El MAX7219 usa registros 1 a 8 para las filas.
        // Si f < 8, enviamos a la matriz superior; si f >= 8, a la inferior.
        // (Nota: La lógica de envío en cascada real puede requerir enviar un NOOP
        // al otro chip, esto depende de cómo esté implementada MAX7219_Enviar)
        MAX7219_Enviar((f % 8) + 1, filaBits);
    }
}

void MatrixLED_MostrarGameOver(void) {
    // Efecto simple: Llenar la pantalla de arriba a abajo y luego vaciarla
    for (int i = 1; i <= 8; i++) {
        MAX7219_Enviar(i, 0xFF); // Todos los LEDs de la fila encendidos
    }
    // Aquí el Integrante 3 debería poner un pequeño delay
}
