#ifndef _SENSORS_H_
#define _SENSORS_H_

#include <stdbool.h>
#include <stdint.h>
#include <math.h>
#include "spi.h"
#include "usart.h"
#include "lsm6dsl.h"
#include "mlx90393.h"
#include "adxl362.h"

#define EN_SPI_ADXL362		g_spi_cs_pin = (ADXL362_SPI_CS)
#define EN_SPI_LSM6DSL		g_spi_cs_pin = (LSM6DSL_SPI_CS)
#define EN_SPI_MLX90393		g_spi_cs_pin = (MLX90393_SPI_CS)

#define SENSOR_READY2READ					0x00
#define SENSOR_NOT_READY2READ				0x01


extern volatile uint8_t CharDiagInfoData[6];
extern volatile uint8_t CharFirmVerData[3];
extern volatile uint8_t CharRockNumData[3];

bool getAccelData(uint8_t* );
bool getGyroData(uint8_t* );
bool getMagData(uint8_t* );
bool getTempData(uint8_t*);

uint8_t initSensors(void);
void getDataSENSOR(uint8_t* );

void calibrationAxis(void);
void loadCalibration(void);

void initTIMER2(void);
void startTIMER2(void);
void stopTIMER2(void);

uint8_t getSleepSensorErrorFlag(void);

#endif
