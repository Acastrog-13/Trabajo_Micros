#ifndef CONTROL_H
#define CONTROL_H

#include "macros.h"
#include "stm32f4xx_hal.h"

#define ZONA_MUERTA 800

typedef struct {
    ADC_HandleTypeDef* hadc;

    uint16_t centro;

} Control_t;

//Detecta al iniciar el centro del Joystick y calcula la zona muerta
void Control_Ini(Control_t* me, ADC_HandleTypeDef* hadc);


//Calcula la dirección según proceda
Direccion_t Control_updateDir(Control_t* me, uint8_t* flag_button, uint8_t* flag_adc, uint8_t* flag_rotar, uint16_t adc_val);

#endif
