#include "matriz_led.h"
#include "main.h"

void MatrizLED_Init(void) {
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
