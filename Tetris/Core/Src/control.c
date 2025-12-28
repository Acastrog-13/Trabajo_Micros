#include <control.h>

void Control_Ini(Control_t* me, ADC_HandleTypeDef* hadc){
	me->hadc = hadc;

	//Detección del centro por polling
	HAL_ADC_Start(me->hadc);
	HAL_ADC_PollForConversion(me->hadc, HAL_MAX_DELAY);
	me->centro=HAL_ADC_GetValue(me->hadc);
	HAL_ADC_Stop(me->hadc);

	//Se reanuda conversión por interrupciones
	HAL_ADC_Start_IT(me->hadc);
}


Direccion_t Control_updateDir(Control_t* me, uint8_t* flag_caida, uint8_t* flag_adc, uint8_t* flag_rotar, uint16_t adc_val){

	//Leds de control
	HAL_GPIO_WritePin (GPIOD, GPIO_PIN_12,0);
	HAL_GPIO_WritePin (GPIOD, GPIO_PIN_13,0);
	HAL_GPIO_WritePin (GPIOD, GPIO_PIN_15,0);

	//Variables para antirrebotoes
	static uint32_t counter_c = 0;
	static uint8_t button_count_c = 0;
	static uint32_t counter_r = 0;
	static uint8_t button_count_r = 0;

	//Caida rápida
	if (*flag_caida) {
		if (HAL_GetTick() - counter_c >= 20) {
			counter_c = HAL_GetTick();

			if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_1) == 1)
				button_count_c++;
			else {
				button_count_c = 0;
				*flag_caida=0;
			}


			if (button_count_c >= 3) {
				button_count_c = 0;
				*flag_caida = 0;

				HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_SET);
				return ABAJO;
			}
		}
	}

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
	if (*flag_adc){
		*flag_adc=0;
		HAL_ADC_Start_IT(me->hadc);

		if(adc_val > me->centro+ZONA_MUERTA) {
			HAL_GPIO_WritePin (GPIOD, GPIO_PIN_15,1);
			return DERECHA;
		}
		else if (adc_val< me->centro-ZONA_MUERTA) {
			HAL_GPIO_WritePin (GPIOD, GPIO_PIN_13,1);
			return IZQUIERDA;
		}
	}

	return NINGUNA;
}
