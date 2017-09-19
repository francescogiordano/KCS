#ifndef _GPIO_H_
#define _GPIO_H_

#define ADXL362_SPI_CS_PIN			25	//Pin PO.25
#define ADXL362_INT_PIN				24	//Pin PO.24

#define MLX90393_SPI_CS_PIN			15	//Pin PO.15 Mag
#define MLX90393_INT_PIN			29	//Pin PO.29

#define LSM6DSL_SPI_CS_PIN			8	//Pin PO.08 Gyro & Accel
#define LSM6DSL_GYRO_INT_PIN		9	//Pin PO.09
#define LSM6DSL_ACCEL_INT_PIN		16	//Pin PO.16

#define UART_TX_PIN					21	//Pin PO.21
#define UART_RX_PIN					22	//Pin PO.22

#define SPI_MOSI_PIN				10	//Pin PO.10
#define SPI_MISO_PIN				11	//Pin PO.11
#define SPI_SCK_PIN					12	//Pin PO.12

#define ADC_PIN						2	//Pin PO.02/AN3	Bat Voltage
#define ENABLE_TEST_PIN				23	//Pin PO.23

void initGPIO(void);
void sleepGPIO(void);

#endif
