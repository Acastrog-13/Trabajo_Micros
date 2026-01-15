#include <control.h>

void Control_Ini(Control_t* me, uint32_t lecturasJoystick[2]){

	me->centroY = lecturasJoystick[0];
	me->centroX = lecturasJoystick[1];
}


Direccion_t Control_updateDir(Control_t* me, uint8_t* flag_rotar, uint32_t lecturasJoystick[2]){

	//Leds de control
	HAL_GPIO_WritePin (GPIOD, GPIO_PIN_12,0);
	HAL_GPIO_WritePin (GPIOD, GPIO_PIN_13,0);
	HAL_GPIO_WritePin (GPIOD, GPIO_PIN_15,0);

	//Variables para antirrebotoes
	static uint32_t counter_r = 0;
	static uint8_t button_count_r = 0;

	//Rotación
	if (*flag_rotar) {
		if (HAL_GetTick() - counter_r >= 20) {
			counter_r = HAL_GetTick();

			if (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_2) == 1)
				button_count_r++;
			else {
				button_count_r = 0;
				*flag_rotar=0;
			}

			if (button_count_r >= 3) {
				button_count_r = 0;
				*flag_rotar = 0;

				return ROTAR;
			}
		}
	}

	//Dirección del Joystick
	if(lecturasJoystick[1] > me->centroX + ZONA_MUERTA) {
			HAL_GPIO_WritePin (GPIOD, GPIO_PIN_15, 1);
			return DERECHA;
	}
	else if (lecturasJoystick[1] < me->centroX - ZONA_MUERTA) {
		HAL_GPIO_WritePin (GPIOD, GPIO_PIN_13, 1);
		return IZQUIERDA;
	}

	if (lecturasJoystick[0] < me->centroY - ZONA_MUERTA) {
		return ABAJO;
	}


	return NINGUNA;
}
