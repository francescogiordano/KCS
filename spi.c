#include "spi.h"
#include "usart.h"

volatile uint8_t g_spi_cs_pin;

void initSPI0(uint32_t freq)
{/// init SPI0 module
	NRF_TWI0->ENABLE = 0x00;											// disable TWI0 since it shares some registers with SPI0 
 
	NRF_GPIO->PIN_CNF[SPI0_SCK] = 0x00000001;							// set as output
	NRF_GPIO->PIN_CNF[SPI0_MOSI] = 0x00000001;							// set as output
	NRF_GPIO->PIN_CNF[SPI0_MISO] = 0x00000000;							// set as input, sense off, drive S0S1, pull-down
	
	
	NRF_GPIO->PIN_CNF[LSM6DSL_SPI_CS] = 0x00000001;					// set as output
	NRF_GPIO->OUTSET = (1<< LSM6DSL_SPI_CS);							// set CS to high state

    NRF_GPIO->PIN_CNF[ADXL362_SPI_CS] = 0x00000001;						// set as output
    NRF_GPIO->OUTSET = (1<<ADXL362_SPI_CS);								// set CS to high state    

    NRF_GPIO->PIN_CNF[MLX90393_SPI_CS] = 0x00000001;						// set as output
    NRF_GPIO->OUTSET = (1<< MLX90393_SPI_CS);

	NRF_SPI0->PSELSCK  = SPI0_SCK;										
	NRF_SPI0->PSELMOSI = SPI0_MOSI;
	NRF_SPI0->PSELMISO = SPI0_MISO;
	
    NRF_SPI0->FREQUENCY = freq;											// set SPI clock rate
	NRF_SPI0->CONFIG  = 0x00;											// Tx MSB first, CPHA_Leading, CPOL_ActiveHigh
    NRF_SPI0->EVENTS_READY = 0x00;										// clear the status flags
    NRF_SPI0->ENABLE = (SPI_ENABLE);									// enable SPI0

	//uint32_t temp = freq;
	//printUSART0("SPI FRE: [%h]\n", &temp);
}

uint8_t rxtxSPI0(uint16_t transfer_size, const uint8_t* tx_data, uint8_t* rx_data, uint8_t spi_cs_pin)
{
	uint32_t counter = 0;
    uint16_t number_of_txd_bytes = 0;
    //uint32_t SEL_SS_PINOUT;
    uint32_t TIMEOUT_COUNTER = 50000;
	bool errorFlag = 0;

	g_spi_cs_pin = spi_cs_pin;

	//SEL_SS_PINOUT = SPI0_NSS;

    /* enable slave (slave select active low) */
    //nrf_gpio_pin_clear(SEL_SS_PINOUT);
	NRF_GPIO->OUTCLR = (1<<g_spi_cs_pin);
    while(number_of_txd_bytes < transfer_size) {
		NRF_SPI0->TXD = (uint32_t)(tx_data[number_of_txd_bytes]);

        /* Wait for the transaction complete or timeout (about 10ms - 20 ms) */
		counter = 0;
        while ((NRF_SPI0->EVENTS_READY == 0) && (counter < TIMEOUT_COUNTER)) {
            counter++;
        }

        if (counter >= TIMEOUT_COUNTER) {
            /* timed out, disable slave (slave select active low) and return with error */
            //nrf_gpio_pin_set(SEL_SS_PINOUT);
            //NRF_GPIO->OUTSET = (1<<g_spi_cs_pin);
            //return false;
			number_of_txd_bytes = transfer_size;
			errorFlag = 1;

			printUSART0("SPI Error\n", 0);
        }
        else {   /* clear the event to be ready to receive next messages */
            NRF_SPI0->EVENTS_READY = 0U;
        }

		//NRF_SPI0->EVENTS_READY = 0;
        rx_data[number_of_txd_bytes] = (uint8_t)NRF_SPI0->RXD;
        number_of_txd_bytes++;
    }

    /* disable slave (slave select active low) */
    //nrf_gpio_pin_set(SEL_SS_PINOUT);
    NRF_GPIO->OUTCLR = (1<<g_spi_cs_pin);
	NRF_GPIO->OUTSET = (1<<g_spi_cs_pin);

    return errorFlag;
}
uint8_t rxtxSPI02(const uint8_t* txData, uint16_t txSize, uint8_t* rxData, uint16_t rxSize, uint8_t spiCsPin)
{
	uint32_t counter = 0;
	uint16_t numTxBytes = 0;
	uint16_t numRxBytes = 0;
	//uint32_t SEL_SS_PINOUT;
	uint32_t TIMEOUT_COUNTER = 50000;
	bool errorFlag = 0;

	/* enable slave (slave select active low) */
	NRF_GPIO->OUTCLR = (1 << spiCsPin);
	while (numTxBytes < txSize) {
		NRF_SPI0->TXD = (uint32_t)(txData[numTxBytes]);

		/* Wait for the transaction complete or timeout (about 10ms - 20 ms) */
		counter = 0;
		while ((NRF_SPI0->EVENTS_READY == 0) && (counter < TIMEOUT_COUNTER)) {
			counter++;
		}
		if (counter >= TIMEOUT_COUNTER) {
			/* timed out, disable slave (slave select active low) and return with error */
			//nrf_gpio_pin_set(SEL_SS_PINOUT);
			numTxBytes = txSize;
			errorFlag = 1;

			printUSART0("SPI Error2\n", 0);
		}
		else {   /* clear the event to be ready to receive next messages */
			NRF_SPI0->EVENTS_READY = 0U;
		}
	}
	while (numRxBytes < rxSize) {
		
		/* Wait for the transaction complete or timeout (about 10ms - 20 ms) */
		counter = 0;
		while ((NRF_SPI0->EVENTS_READY == 0) && (counter < TIMEOUT_COUNTER)) {
			counter++;
		}

		if (counter >= TIMEOUT_COUNTER) {
			/* timed out, disable slave (slave select active low) and return with error */
			//nrf_gpio_pin_set(SEL_SS_PINOUT);
			numRxBytes = rxSize;
			//return false;
			errorFlag = 1;
		}
		else {   /* clear the event to be ready to receive next messages */
			NRF_SPI0->EVENTS_READY = 0U;
		}

		//NRF_SPI0->EVENTS_READY = 0;
		rxData[numRxBytes] = (uint8_t)NRF_SPI0->RXD;
		numRxBytes++;
	}

	/* disable slave (slave select active low) */
	//nrf_gpio_pin_set(SEL_SS_PINOUT);
	NRF_GPIO->OUTCLR = (1 << spiCsPin);
	NRF_GPIO->OUTSET = (1 << spiCsPin);

	return errorFlag;
}

void sleepSPIO() {

	nrf_gpio_pin_set(ADXL362_SPI_CS_PIN);
	nrf_gpio_pin_set(MLX90393_SPI_CS_PIN);
	nrf_gpio_pin_set(LSM6DSL_SPI_CS_PIN);

	nrf_gpio_cfg_output(SPI0_SCK);
	nrf_gpio_pin_clear(SPI0_SCK);

	nrf_gpio_cfg_output(SPI0_MOSI);
	nrf_gpio_pin_clear(SPI0_MOSI);

	nrf_gpio_cfg_output(SPI0_MISO);
	nrf_gpio_pin_clear(SPI0_MISO);
}
