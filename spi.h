#ifndef _SPI_H__
#define _SPI_H__

#include "nrf51.h"
#include "nrf_gpio.h"
#include "gpio.h"

#define SPI_WRITE_DATA			0x00
#define SPI_READ_DATA			0x80
#define SPI_SINGLE_TRANS		0x00
#define SPI_MULTI_TRANS			0x40

#define SPI0_MOSI				SPI_MOSI_PIN
#define SPI0_MISO				SPI_MISO_PIN
#define SPI0_SCK				SPI_SCK_PIN

#define ADXL362_SPI_CS			ADXL362_SPI_CS_PIN
#define MLX90393_SPI_CS			MLX90393_SPI_CS_PIN	//Mag
#define LSM6DSL_SPI_CS			LSM6DSL_SPI_CS_PIN	//Gyro & Accel


#define SPI_DATARATE_125kbps	0x02000000
#define SPI_DATARATE_250kbps 	0x04000000 
#define SPI_DATARATE_500kbps	0x08000000 
#define SPI_DATARATE_1Mbps		0x10000000 
#define SPI_DATARATE_2Mbps	 	0x20000000 
#define SPI_DATARATE_4Mbps 		0x40000000 
#define SPI_DATARATE_8Mbps 		0x80000000 		// this is not advisable

#define SPI_ENABLE 				0x00000001

extern volatile uint8_t g_spi_cs_pin;

void initSPI0(uint32_t );
uint8_t rxtxSPI0(uint16_t transfer_size, const uint8_t *tx_data, uint8_t *rx_data, uint8_t spi_cs_pin);
uint8_t rxtxSPI02(const uint8_t* txData, uint16_t txSize, uint8_t* rxData, uint16_t rxSize, uint8_t spiCsPin);
void sleepSPIO();
#endif