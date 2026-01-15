#ifndef CONTROL_H
#define CONTROL_H

#include "macros.h"
#include "stm32f4xx_hal.h"

#define ZONA_MUERTA 800

typedef struct {

    uint32_t centroX;
    uint32_t centroY;

} Control_t;

//Detecta al iniciar el centro del Joystick y calcula la zona muerta
void Control_Ini(Control_t* me, uint32_t lecturasJoystick[2]);


//Calcula la dirección según proceda
Direccion_t Control_updateDir(Control_t* me, uint8_t* flag_rotar, uint32_t lecturasJoystick[2]);

#endif
