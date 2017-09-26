#include "gpio.h"
#include "nrf_gpio.h"
#include "nrf_drv_gpiote.h"
#include "nrf_gpiote.h"

#define GPIOTE_CHANNEL_0 0
#define GPIOTE_CHANNEL_1 1

/*
void GPIOTE_IRQHandler(void) {

	uint32_t temp = nrf_gpio_pin_read(ADXL362_INT_PIN);
	printUSART0("GPIOTE_IRQHandler: [%h]\n", &temp);

	if ((NRF_GPIOTE->EVENTS_IN[0] != 0)) {
		NRF_GPIOTE->EVENTS_IN[0] = 0;
	}

	if ((NRF_GPIOTE->EVENTS_IN[1] != 0)) {
		NRF_GPIOTE->EVENTS_IN[1] = 0;
	}
}
*/

void initGPIO(void) {

	nrf_gpio_cfg_sense_input(LSM6DSL_ACCEL_INT_PIN, NRF_GPIO_PIN_PULLDOWN, NRF_GPIO_PIN_SENSE_HIGH);

	nrf_gpio_cfg_input(MLX90393_INT_PIN, NRF_GPIO_PIN_PULLDOWN);
	nrf_gpio_cfg_input(ENABLE_TEST_PIN, NRF_GPIO_PIN_PULLDOWN);
}
void sleepGPIO(void) {
	nrf_gpio_cfg_output(MLX90393_INT_PIN);
	nrf_gpio_pin_clear(MLX90393_INT_PIN);
}

/*
nrf_drv_gpiote_init();

NVIC_DisableIRQ(GPIOTE_IRQn);
NVIC_ClearPendingIRQ(GPIOTE_IRQn);


NRF_GPIOTE->CONFIG[GPIOTE_CHANNEL_1] = (GPIOTE_CONFIG_POLARITY_LoToHi << GPIOTE_CONFIG_POLARITY_Pos)
| (ADXL362_INT_PIN << GPIOTE_CONFIG_PSEL_Pos)
| (GPIOTE_CONFIG_MODE_Event << GPIOTE_CONFIG_MODE_Pos);

//NRF_GPIOTE->INTENSET = GPIOTE_INTENSET_IN0_Set << GPIOTE_INTENSET_IN0_Pos;
NRF_GPIOTE->INTENSET = GPIOTE_INTENSET_IN1_Set << GPIOTE_INTENSET_IN1_Pos;

//NRF_GPIOTE->INTENSET = GPIOTE_INTENSET_IN0_Enabled;
NRF_GPIOTE->INTENSET = GPIOTE_INTENSET_IN1_Enabled;

NVIC_EnableIRQ(GPIOTE_IRQn); //Enable interrupts
*/