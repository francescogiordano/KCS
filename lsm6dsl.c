#include "lsm6dsl.h"
#include "spi.h"
#include "usart.h"
#include "delay.h"

static uint8_t serialType = LSM6DSL_SERIAL_PORT_SPI;
static uint8_t lsm6dslSpiCsPin = 0;
static bool initializedFlag = 0;

bool initLsm6dsl(uint8_t type, uint8_t spiCsPin) {
	uint8_t tx_data[2];
	uint8_t	rx_data[2];
	bool errorFlag = 0;

	uint32_t temp = 0;
	
	serialType = type;
	lsm6dslSpiCsPin = spiCsPin;

	if (serialType == LSM6DSL_SERIAL_PORT_SPI) {

		//Reset Device
		tx_data[0] = (LSM6DSL_CTRL3_C | SPI_WRITE_DATA);
		tx_data[1] = LSM6DSL_SW_RESET;
		if (rxtxSPI0(2, tx_data, rx_data, lsm6dslSpiCsPin)) {		//Check if SPI communication
			errorFlag = 1;
		}
		delay_ms(5);	

		tx_data[0] = (LSM6DSL_WHO_AM_I | SPI_READ_DATA);
		tx_data[1] = 0x00;
		if (rxtxSPI0(2, tx_data, rx_data, lsm6dslSpiCsPin)) {		//Check if SPI communication
			errorFlag = 1;
		}		
		if (rx_data[1] != LSM6DSL_WHO_AM_I_VALUE) {		//Check if SPI Chip Valid
			errorFlag = 1;								//value = 0x00;

			temp = rx_data[1];
			printUSART0("LSM6DSL_WHO_AM_I FAIL: [%h]\n", &temp);
		}

		//Enable LPF1 For Gyro
		tx_data[0] = (LSM6DSL_CTRL4_C | SPI_WRITE_DATA);
		tx_data[1] = LSM6DSL_LPF1_SEL_G;
		if (rxtxSPI0(2, tx_data, rx_data, lsm6dslSpiCsPin)) {		//Check if SPI communication
			errorFlag = 1;
		}

		//Set Normal/Low Power Mode Gyroscope
		tx_data[0] = (LSM6DSL_CTRL7_G | SPI_WRITE_DATA);
		tx_data[1] = (LSM6DSL_G_HM_MODE_DISABLE);
		if (rxtxSPI0(2, tx_data, rx_data, lsm6dslSpiCsPin)) {		//Check if SPI communication
			errorFlag = 1;
		}

		//Set 416Hz Gyroscope & Set To Scale 2000dps - 4.375 mdps/LSB - Normal/Low power mode noise 75 mdps
		tx_data[0] = (LSM6DSL_CTRL2_G | SPI_WRITE_DATA);
		tx_data[1] = (LSM6DSL_ODR_G_416HZ | LSM6DSL_FS_G_1000DPS);
		if (rxtxSPI0(2, tx_data, rx_data, lsm6dslSpiCsPin)) {		//Check if SPI communication
			errorFlag = 1;
		}

		//Set Normal/Low Power Mode Gyroscope & Set LPF1 Value For Gyro
		tx_data[0] = (LSM6DSL_CTRL6_C | SPI_WRITE_DATA);
		tx_data[1] = (LSM6DSL_XL_HM_MODE_DISABLE | LSM6DSL_FTYPE_3);
		if (rxtxSPI0(2, tx_data, rx_data, lsm6dslSpiCsPin)) {		//Check if SPI communication
			errorFlag = 1;
		}

		//Set 416Hz Accelerometer & Set To 4G Scale - .122 mg/LSB - Normal/Low power mode noise 2.0 mg
		tx_data[0] = (LSM6DSL_CTRL1_XL | SPI_WRITE_DATA);
		//tx_data[1] = (LSM6DSL_ODR_XL_12HZ | LSM6DSL_FS_XL_2G);
		tx_data[1] = (LSM6DSL_ODR_XL_416HZ | LSM6DSL_FS_XL_4G | LSM6DSL_LPF1_BW_SEL);
		if (rxtxSPI0(2, tx_data, rx_data, lsm6dslSpiCsPin)) {		//Check if SPI communication
			errorFlag = 1;
		}

		//Linear acceleration LPF ODR/50
		tx_data[0] = (LSM6DSL_CTRL8_XL | SPI_WRITE_DATA);
		tx_data[1] = (LSM6DSL_LPF2_XL_EN | LSM6DSL_HPCF_XL_0 | LSM6DSL_INPUT_COMPOSITE);
		if (rxtxSPI0(2, tx_data, rx_data, lsm6dslSpiCsPin)) {		//Check if SPI communication
			errorFlag = 1;
		}
	}
	else {
		errorFlag = 1;
	}

	if (errorFlag == 0) {
		initializedFlag = 1;
	}

	//temp = initializedFlag;
	//printUSART0("LSM6DSL initialized?: [%h]\n", &temp);

	return errorFlag;
}
bool sleepLsm6dsl(void) {
	uint8_t tx_data[2];
	uint8_t rx_data[2];
	bool errorFlag = 0;

	if (initializedFlag) {
		if (serialType == LSM6DSL_SERIAL_PORT_SPI) {

			//Set 26Hz Accelerometer & Set To Lowest Scale 2G
			tx_data[0] = (LSM6DSL_CTRL1_XL | SPI_WRITE_DATA);
			tx_data[1] = (LSM6DSL_ODR_XL_26HZ | LSM6DSL_FS_XL_2G );
			if (rxtxSPI0(2, tx_data, rx_data, lsm6dslSpiCsPin)) {		//Check if SPI communication
				errorFlag = 1;
			}

			//INT1 pin function routing
			tx_data[0] = (LSM6DSL_MD1_CFG | SPI_WRITE_DATA);
			tx_data[1] = (LSM6DSL_INT1_WU);
			if (rxtxSPI0(2, tx_data, rx_data, lsm6dslSpiCsPin)) {		//Check if SPI communication
				errorFlag = 1;
			}

			//Wake up threshold
			tx_data[0] = (LSM6DSL_WAKE_UP_THS | SPI_WRITE_DATA);
			tx_data[1] = (LSM6DSL_WK_THS_1_PER);
			if (rxtxSPI0(2, tx_data, rx_data, lsm6dslSpiCsPin)) {		//Check if SPI communication
				errorFlag = 1;
			}

			//Wake up duration
			tx_data[0] = (LSM6DSL_WAKE_UP_DUR | SPI_WRITE_DATA);
			tx_data[1] = (LSM6DSL_WAKE_DUR_3);
			if (rxtxSPI0(2, tx_data, rx_data, lsm6dslSpiCsPin)) {		//Check if SPI communication
				errorFlag = 1;
			}

			//Enable interrupt and inactivity functions
			tx_data[0] = (LSM6DSL_TAP_CFG | SPI_WRITE_DATA);
			tx_data[1] = (LSM6DSL_INTERRUPTS_ENABLE | LSM6DSL_INACT_EN_3);
			if (rxtxSPI0(2, tx_data, rx_data, lsm6dslSpiCsPin)) {		//Check if SPI communication
				errorFlag = 1;
			}
		}
		else {
			errorFlag = 1;
		}
	}
	else {
		errorFlag = 1;
	}

	return errorFlag;
}
bool sleep2Lsm6dsl(void) {
	uint8_t tx_data[2];
	uint8_t rx_data[2];
	bool errorFlag = 0;

	if (initializedFlag) {
		if (serialType == LSM6DSL_SERIAL_PORT_SPI) {

			//Power Down gyro
			tx_data[0] = (LSM6DSL_CTRL2_G | SPI_WRITE_DATA);
			tx_data[1] = (0x00);
			if (rxtxSPI0(2, tx_data, rx_data, lsm6dslSpiCsPin)) {		//Check if SPI communication
				errorFlag = 1;
			}

			//Set 26Hz Accelerometer & Set To Lowest Scale 2G
			tx_data[0] = (LSM6DSL_CTRL1_XL | SPI_WRITE_DATA);
			tx_data[1] = (LSM6DSL_ODR_XL_26HZ | LSM6DSL_FS_XL_2G);
			if (rxtxSPI0(2, tx_data, rx_data, lsm6dslSpiCsPin)) {		//Check if SPI communication
				errorFlag = 1;
			}

			//INT1 pin function routing
			tx_data[0] = (LSM6DSL_MD1_CFG | SPI_WRITE_DATA);
			tx_data[1] = (LSM6DSL_INT1_TILT);
			if (rxtxSPI0(2, tx_data, rx_data, lsm6dslSpiCsPin)) {		//Check if SPI communication
				errorFlag = 1;
			}

			tx_data[0] = (LSM6DSL_CTRL10_C | SPI_WRITE_DATA);
			tx_data[1] = (LSM6DSL_TILT_EN | LSM6DSL_FUNC_EN);
			if (rxtxSPI0(2, tx_data, rx_data, lsm6dslSpiCsPin)) {		//Check if SPI communication
				errorFlag = 1;
			}
			
		}
		else {
			errorFlag = 1;
		}
	}
	else {
		errorFlag = 1;
	}

	return errorFlag;
}
bool sleep3Lsm6dsl(void) {
	uint8_t tx_data[2];
	uint8_t rx_data[2];
	bool errorFlag = 0;

	if (initializedFlag) {
		if (serialType == LSM6DSL_SERIAL_PORT_SPI) {

			//Set 26Hz Accelerometer & Set To Lowest Scale 2G
			tx_data[0] = (LSM6DSL_CTRL1_XL | SPI_WRITE_DATA);
			tx_data[1] = (LSM6DSL_ODR_XL_26HZ | LSM6DSL_FS_XL_2G);
			if (rxtxSPI0(2, tx_data, rx_data, lsm6dslSpiCsPin)) {		//Check if SPI communication
				errorFlag = 1;
			}

			//INT1 pin function routing
			tx_data[0] = (LSM6DSL_MD1_CFG | SPI_WRITE_DATA);
			tx_data[1] = (LSM6DSL_INT1_DOUBLE_TAP);
			if (rxtxSPI0(2, tx_data, rx_data, lsm6dslSpiCsPin)) {		//Check if SPI communication
				errorFlag = 1;
			}

			//Wake up threshold
			tx_data[0] = (LSM6DSL_WAKE_UP_THS | SPI_WRITE_DATA);
			tx_data[1] = (0x80);
			if (rxtxSPI0(2, tx_data, rx_data, lsm6dslSpiCsPin)) {		//Check if SPI communication
				errorFlag = 1;
			}

			//Wake up duration
			tx_data[0] = (LSM6DSL_TAP_THS_6D | SPI_WRITE_DATA);
			tx_data[1] = (LSM6DSL_TAP_THS_30_PER);
			if (rxtxSPI0(2, tx_data, rx_data, lsm6dslSpiCsPin)) {		//Check if SPI communication
				errorFlag = 1;
			}

			//Enable interrupt and inactivity functions
			tx_data[0] = (LSM6DSL_TAP_CFG | SPI_WRITE_DATA);
			tx_data[1] = (LSM6DSL_INTERRUPTS_ENABLE | LSM6DSL_INACT_EN_3 | LSM6DSL_TAP_Z_EN);
			if (rxtxSPI0(2, tx_data, rx_data, lsm6dslSpiCsPin)) {		//Check if SPI communication
				errorFlag = 1;
			}
			
		}
		else {
			errorFlag = 1;
		}
	}
	else {
		errorFlag = 1;
	}

	return errorFlag;
}
bool wakeLsm6dsl(void) {
	uint8_t tx_data[2];
	uint8_t rx_data[2];
	bool errorFlag = 0;

	if (initializedFlag) {
		if (serialType == LSM6DSL_SERIAL_PORT_SPI) {
			//Set 12.5Hz Gyroscope & Set To Lowest Scale 125dps - 4.375 mdps/LSB - Normal/Low power mode noise 75 mdps
			tx_data[0] = (LSM6DSL_CTRL2_G | SPI_WRITE_DATA);
			tx_data[1] = (LSM6DSL_ODR_G_12HZ | LSM6DSL_FS_125_125DPS);
			if (rxtxSPI0(2, tx_data, rx_data, lsm6dslSpiCsPin)) {		//Check if SPI communication
				errorFlag = 1;
			}

			//Set 12.5Hz Accelerometer & Set To Lowest Scale 2G - .061 mg/LSB - Normal/Low power mode noise 1.8 mg
			tx_data[0] = (LSM6DSL_CTRL1_XL | SPI_WRITE_DATA);
			tx_data[1] = (LSM6DSL_ODR_XL_12HZ | LSM6DSL_FS_XL_2G);
			if (rxtxSPI0(2, tx_data, rx_data, lsm6dslSpiCsPin)) {		//Check if SPI communication
				errorFlag = 1;
			}
		}
		else {
			errorFlag = 1;
		}
	}
	else {
		errorFlag = 1;
	}

	return errorFlag;
}

bool getLsm6dslAccelData(uint8_t* accelDataPtr) {
	uint8_t tx_data[8] = { 0 };
	uint8_t rx_data[8] = { 0 };
	bool errorFlag = 1;

	if (initializedFlag) {
		if (serialType == LSM6DSL_SERIAL_PORT_SPI) {
			tx_data[0] = (LSM6DSL_OUTX_L_XL | SPI_READ_DATA);
			if (rxtxSPI0(7, tx_data, rx_data, lsm6dslSpiCsPin) == 0) {
				errorFlag = 0;
			}

			//if(sizeof(accelDataPtr) / sizeof(accelDataPtr[0]) >= 6)
			for (int i = 0; i < 6; i++) {
				accelDataPtr[i] = rx_data[i + 1];
			}
		}
	}
	else {
		printUSART0("getLsm6dslAccelData - Not Initialized\n", 0);
	}
	return errorFlag;
}
bool getLsm6dslGyroData(uint8_t* gyroDataPtr) {
	uint8_t tx_data[7];
	uint8_t rx_data[7];
	bool errorFlag = 1;

	if (initializedFlag) {
		if (serialType == LSM6DSL_SERIAL_PORT_SPI) {
			tx_data[0] = (LSM6DSL_OUTX_L_G | SPI_READ_DATA);
			if(rxtxSPI0(7, tx_data, rx_data, lsm6dslSpiCsPin) == 0) {
				errorFlag = 0;
			}

			//if(sizeof(gyroDataPtr) / sizeof(gyroDataPtr[0]) >= 6)
			for (int i = 0; i < 6; i++) {
				gyroDataPtr[i] = rx_data[i + 1];
			}
		}
	}
	return errorFlag;
}
bool getLsm6dslTempData(uint8_t* tempDataPtr) {
	uint8_t tx_data[3];
	uint8_t rx_data[3];
	bool errorFlag = 1;

	if (initializedFlag) {
		if (serialType == LSM6DSL_SERIAL_PORT_SPI) {
			tx_data[0] = (LSM6DSL_OUT_TEMP_L | SPI_READ_DATA);
			if (rxtxSPI0(3, tx_data, rx_data, lsm6dslSpiCsPin) == 0) {
				errorFlag = 0;
			}

			for (int i = 0; i < 2; i++) {
				tempDataPtr[i] = rx_data[i + 1];	//tempDataPtr[0] = TEMP_L, tempDataPtr[1] = TEMP_H
			}
		}
	}
	return errorFlag;
}

