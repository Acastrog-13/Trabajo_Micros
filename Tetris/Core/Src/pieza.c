#include "pieza.h"
#include <math.h>

static const uint16_t FIGURAS_HEX[MAX_PIEZAS][4] = {
    {0x0F00, 0x2222, 0x00F0, 0x4444}, // I
    {0x8E00, 0x6440, 0x0E20, 0x44C0}, // J
    {0x2E00, 0x4460, 0x0E80, 0xC440}, // L
    {0x6600, 0x6600, 0x6600, 0x6600}, // O
    {0x6C00, 0x4620, 0x06C0, 0x8C40}, // S
    {0x4E00, 0x4640, 0x0E40, 0x4C40}, // T
    {0xC600, 0x2640, 0x0C60, 0x4C80}  // Z
};

void setForma(Pieza_t* me) {
	uint16_t pieza_hex = FIGURAS_HEX[me->tipo][me->rotacion];

		for (uint8_t i = 0; i < 4; i++) {
			for (uint8_t j = 0; j < 4; j++) {

				uint8_t posicionBit = 15 - (i * 4 + j);

				me->forma[i][j] = (pieza_hex >> posicionBit) & 0x01;
			}
		}
}

void Pieza_Init(Pieza_t* me) {

    me->x = (uint8_t)(rand() % (TABLERO_ANCHO - PIEZA_ANCHO + 1));
    me->y = 0;
    me->tipo = (uint8_t)(rand() % MAX_PIEZAS);
    me->rotacion = (uint8_t)(rand() % 4);

    setForma(me);
}

void Pieza_Rotar(Pieza_t* me) {
    me->rotacion = (me->rotacion + 1) % 4;
    setForma(me);
}

void Pieza_Mover(Pieza_t* me, Direccion_t dir) {
	switch (dir) {
		case IZQUIERDA:
			me->x -= 1;
			break;
		case DERECHA:
			me->x += 1;
			break;
		case ABAJO:
			me->y += 1;
			break;
		case ARRIBA:
			me->y -= 1;
			break;
		default:
			break;
	}
}

bool Pieza_GetBloque(Pieza_t* me, uint8_t i, uint8_t j) {
    if (i >= 4 || j >= 4) return false;
    return (me->forma[i][j]);
}
